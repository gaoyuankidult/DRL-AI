#include "layer.h"

Layer* GetLayer(NNParameter* nn_para, LayerParameter* layer_para)  {
  if (layer_para->type_ == "conv") {   
    return new ConvLayer(nn_para, layer_para);
  } else if (layer_para->type_ == "full_rect") {   
    return new FullRectLayer(nn_para, layer_para);  
  } else if (layer_para->type_ == "full_linear") {   
    return new FullLinearLayer(nn_para, layer_para);  
  } else {
    cerr << "no each layer" << endl;
    return NULL;
  }
}      
        
void ConvLayer::Init(Layer* bottom, Layer* top, curandGenerator_t gen, cublasHandle_t handle) {    
  if (bottom == NULL) {
    input_ = NULL;      
    input_im_size_ = nn_para_->im_size_;
    input_size_ = input_im_size_*input_im_size_;
    input_num_ = 4;
  } else if (bottom != NULL) {
    input_ = bottom->output_;
    if (input_ != bottom->output_) {
	    cerr << "Init input == bottom output";
      exit(0);
    }

    input_im_size_ = bottom->output_im_size_;    
    input_size_ = bottom->output_size_;  
    input_num_ = bottom->output_num_;    
  }

  bottom_ = bottom;
  top_ = top;

  gen_ = gen;
  handle_ = handle;

  output_im_size_ = (input_im_size_ - filter_size_) / stride_ + 1;
  output_size_ = output_im_size_*output_im_size_;  
  cudaMalloc(&input_, filter_num_*input_size_*sizeof(float));
  cudaMalloc(&output_, filter_num_*output_size_*sizeof(float));
  cudaMalloc(&delta_, filter_num_*output_size_*sizeof(float));
  cudaMalloc(&weight_, input_num_*filter_num_*filter_size_*filter_size_*sizeof(float));
  cudaMalloc(&bias_, filter_num_*sizeof(float));
  cudaMalloc(&grad_weight_, input_num_*filter_num_*filter_size_*filter_size_*sizeof(float));
  cudaMalloc(&grad_bias_, filter_num_*sizeof(float));
  cudaMalloc(&im_col_, output_im_size_*output_im_size_*(filter_size_+1)*(filter_size_+1)*stride_*sizeof(float));
  cudaMalloc(&ones_, output_im_size_*output_im_size_*sizeof(float));

  weight_cpu_ = (float*) malloc(input_num_*filter_num_*filter_size_*filter_size_*sizeof(float));
  bias_cpu_ = (float*) malloc(filter_num_*sizeof(float));

  RandGauss(weight_, input_num_*filter_num_*filter_size_*filter_size_, 0.001, gen_, handle_);
  Constants(bias_, filter_num_, 0.0);        
  Zeros(delta_, filter_num_*output_size_);
  Constants(ones_, output_im_size_*output_im_size_, 1.0);

}

void ConvLayer::Forward() {    
  Zeros(output_, filter_num_*output_size_);

  for (int i = 0; i < input_num_; ++i) {
    im2col_gpu(input_ + i*input_im_size_*input_im_size_, 1, input_im_size_, input_im_size_, filter_size_, 0, stride_, im_col_);

    float alpha = 1.0;
    float beta = 1.0;
    cublasSgemm(handle_, CUBLAS_OP_N, CUBLAS_OP_N,
      output_im_size_*output_im_size_, filter_num_, filter_size_*filter_size_, 
      &alpha, im_col_, output_im_size_*output_im_size_, weight_ + i*filter_num_*filter_size_*filter_size_, filter_size_*filter_size_, 
      &beta, output_, output_im_size_*output_im_size_);
  }
 
  for (int i = 0; i < filter_num_; ++i) {
    float alpha = 1.0;
    float beta = 1.0;
    cublasSgemv(handle_, CUBLAS_OP_N, 1, output_im_size_*output_im_size_, 
      &alpha, ones_, 1, &bias_[i], 1, &beta, output_ + i*output_im_size_*output_im_size_, 1);
  }

  Rect(output_, filter_num_*output_size_);
}

void ConvLayer::Backward() {  
  if (bottom_ == NULL) {
    return;
  }

  for (int i = 0; i < input_num_; ++i) {
    Zeros(im_col_, output_im_size_*output_im_size_*filter_size_*filter_size_);

    float alpha = 1.0;
    float beta = 1.0; 
    cublasSgemm(handle_,CUBLAS_OP_N, CUBLAS_OP_T, 
                output_im_size_*output_im_size_, filter_size_*filter_size_, filter_num_,
                &alpha, delta_, output_im_size_*output_im_size_, weight_ + i*filter_num_*filter_size_*filter_size_, filter_size_*filter_size_, 
		            &beta, im_col_, output_im_size_*output_im_size_);  

    col2im_gpu(im_col_, 1, input_im_size_, input_im_size_, filter_size_, 0,  stride_, bottom_->delta_ + i*input_im_size_*input_im_size_);        
  }

  // applying the deriative of rectifier function
  RectDiff(bottom_->output_, bottom_->delta_, input_num_*input_im_size_*input_im_size_);
}  

void ConvLayer::GetGradient() {
  Zeros(grad_weight_, input_num_*filter_num_*filter_size_*filter_size_);        
  Zeros(grad_bias_, filter_num_);        

  // Convolute input_ with delta_  
  int CONV_DIM = filter_size_ + 1;
  int FILTER_SIZE = output_im_size_;
  for (int i = 0; i < input_num_; ++i) {

    im2col_gpu(input_ + i*input_im_size_*input_im_size_, 1, input_im_size_, input_im_size_, FILTER_SIZE, 0, stride_, im_col_);  

    float alpha = 1.0;
    float beta = 1.0;
    cublasSgemm(handle_, CUBLAS_OP_N, CUBLAS_OP_N, 
              CONV_DIM*CONV_DIM, filter_num_, FILTER_SIZE*FILTER_SIZE,
              &alpha, im_col_, CONV_DIM*CONV_DIM, delta_, 
              FILTER_SIZE*FILTER_SIZE, &beta, grad_weight_ + i*filter_num_*filter_size_*filter_size_, CONV_DIM*CONV_DIM);
  }

  // grad_bc2
  for (int i = 0; i < filter_num_; ++i) {
    float alpha = 1.0;
    float beta = 1.0;
    cublasSgemv(handle_, CUBLAS_OP_T, output_im_size_*output_im_size_, 1, &alpha, 
                delta_ + i*output_im_size_*output_im_size_, output_im_size_*output_im_size_, ones_, 1, &beta, &grad_bias_[i], 1);          
  }

}

void ConvLayer::Update() {
  float alpha = - nn_para_->learn_rate_;
  cublasSaxpy(handle_, input_num_*filter_num_*filter_size_*filter_size_, &alpha, grad_weight_, 1, weight_, 1);    
  cublasSaxpy(handle_, filter_num_, &alpha, grad_bias_, 1, bias_, 1);    
}

void ConvLayer::WriteWeights(ofstream &file_out) {
  cudaMemcpy(weight_cpu_, weight_, input_num_*filter_num_*filter_size_*filter_size_*sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(bias_cpu_, bias_, filter_num_*sizeof(float), cudaMemcpyDeviceToHost);
  for (int i = 0; i < input_num_*filter_num_*filter_size_*filter_size_; ++i) {
    file_out << weight_cpu_[i] << " ";
  }

  for (int i = 0; i < filter_num_; ++i) {
    file_out << bias_cpu_[i] << " ";
  }
}

void ConvLayer::ReadWeights(ifstream &file_in) {
  cout << "ConvLayer read weights" << endl;
  for (int i = 0; i < input_num_*filter_num_*filter_size_*filter_size_; ++i) {
    file_in >> weight_cpu_[i];
  }

  for (int i = 0; i < filter_num_; ++i) {
    file_in >> bias_cpu_[i];
  }
  
  cudaMemcpy(weight_ , weight_cpu_, input_num_*filter_num_*filter_size_*filter_size_*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(bias_, bias_cpu_, filter_num_*sizeof(float), cudaMemcpyHostToDevice);
}

void FullRectLayer::Init(Layer* bottom, Layer* top, curandGenerator_t gen, cublasHandle_t handle) {
  if (bottom == NULL) {
    input_ = NULL;      
    input_im_size_ = nn_para_->im_size_;
    input_size_ = input_im_size_*input_im_size_;
    input_num_ = 1;
  } else {
    input_ = bottom->output_;
    input_im_size_ = bottom->output_im_size_;      
    input_size_ = bottom->output_size_;  
    input_num_ = bottom->output_num_;    
  }

  bottom_ = bottom;
  top_ = top;

  gen_ = gen;
  handle_ = handle;

  output_im_size_ = 0;
  output_num_ = 1;    

  cudaMalloc(&output_, output_size_*sizeof(float));
  cudaMalloc(&delta_, output_size_*sizeof(float));
  cudaMalloc(&weight_, input_num_*input_num_*input_size_*output_size_*sizeof(float));
  cudaMalloc(&bias_, output_size_*sizeof(float));
  cudaMalloc(&grad_weight_, input_num_*input_size_*output_size_*sizeof(float));
  cudaMalloc(&grad_bias_, output_size_*sizeof(float));

  weight_cpu_ = (float*)malloc(input_num_*input_num_*input_size_*output_size_*sizeof(float));
  bias_cpu_ = (float*)malloc(output_size_*sizeof(float));

  RandGauss(weight_, input_num_*input_num_*input_size_*output_size_, 0.001, gen_, handle_);
  Constants(bias_, output_size_, 0.0);
  Zeros(delta_, output_size_);

  
}

void FullRectLayer::Forward() {
  Zeros(output_, output_size_);        
  float alpha = 1.0;
  float beta = 0.0;
  cublasSgemv(handle_, CUBLAS_OP_T, input_num_*input_size_, output_size_, &alpha, 
              weight_, input_num_*input_size_, input_, 1, &beta, output_, 1);   

  cublasSaxpy(handle_, output_size_, &alpha, bias_, 1, output_, 1);    
  Rect(output_, output_size_);
}

void FullRectLayer::Backward() {
  if (top_ == NULL) {
    // delta_d = o3 - yLayer* bottom
    cudaMemcpy(delta_, output_, output_size_*sizeof(float), cudaMemcpyDeviceToDevice);
    AddElement(delta_, output_size_, action_, -y_);
  }

  float alpha = 1.0;
  float beta = 0.0;

  cublasSgemv(handle_, CUBLAS_OP_N, input_size_*input_num_, output_size_, &alpha,
              weight_, input_size_*input_num_, delta_, 1, &beta, bottom_->delta_, 1);

  RectDiff(bottom_->output_, bottom_->delta_, input_num_*input_size_);
}

void FullRectLayer::GetGradient() {
  Zeros(grad_weight_, input_num_*input_size_*output_size_);        
  Zeros(grad_bias_, output_size_);        

  float alpha = 1.0; 
  float beta = 1.0;
  cublasSgemm(handle_, CUBLAS_OP_N, CUBLAS_OP_N, 
              output_size_, input_num_*input_size_, 1,
              &alpha, delta_, output_size_, input_, 1, &beta, grad_weight_, output_size_);  

  cublasSaxpy(handle_, output_size_, &alpha, delta_, 1, grad_bias_, 1);    

}

void FullRectLayer::Update() {
  float alpha = - nn_para_->learn_rate_;
  cublasSaxpy(handle_, output_size_, &alpha, grad_bias_, 1, bias_, 1);    
}

void FullRectLayer::WriteWeights(ofstream &file_out) {
  cudaMemcpy(weight_cpu_, weight_, input_num_*input_size_*output_size_*sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(bias_cpu_, bias_, output_size_*sizeof(float), cudaMemcpyDeviceToHost);

  for (int i = 0; i < input_num_*input_size_*output_size_; ++i) {
    file_out << weight_[i] << " ";
  }

  for (int i = 0; i < output_size_; ++i) {
    file_out << bias_[i] << " ";
  }
}

void FullRectLayer::ReadWeights(ifstream &file_in) {
  cout << "FullRectLayer read weights" << endl;
  for (int i = 0; i < input_num_*input_size_*output_size_; ++i) {
    file_in >> weight_cpu_[i];
  }

  for (int i = 0; i < output_size_; ++i) {
    file_in >> bias_cpu_[i];
  }

  cudaMemcpy(weight_, weight_cpu_, input_num_*input_size_*output_size_*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(bias_, bias_cpu_, output_size_*sizeof(float), cudaMemcpyHostToDevice);
}

void FullLinearLayer::Init(Layer* bottom, Layer* top, curandGenerator_t gen, cublasHandle_t handle) {
  if (bottom == NULL) {
    input_ = NULL;      
    input_im_size_ = nn_para_->im_size_;
    input_size_ = input_im_size_*input_im_size_;
    input_num_ = 1;
  } else {
    input_ = bottom->output_;
    input_im_size_ = bottom->output_im_size_;      
    input_size_ = bottom->output_size_;  
    input_num_ = bottom->output_num_;    
  }

  bottom_ = bottom;
  top_ = top;

  gen_ = gen;
  handle_ = handle;

  output_im_size_ = 0;
  output_num_ = 1;    

  cudaMalloc(&output_, output_size_*sizeof(float));
  cudaMalloc(&delta_, output_size_*sizeof(float));
  cudaMalloc(&weight_, input_num_*input_size_*output_size_*sizeof(float));
  cudaMalloc(&bias_, output_size_*sizeof(float));
  cudaMalloc(&grad_weight_, input_num_*input_size_*output_size_*sizeof(float));
  cudaMalloc(&grad_bias_, output_size_*sizeof(float));

  weight_cpu_ = (float*)malloc(input_num_*input_num_*input_size_*output_size_*sizeof(float));
  bias_cpu_ = (float*)malloc(output_size_*sizeof(float));

  RandGauss(weight_, input_num_*input_num_*input_size_*output_size_, 0.001, gen_, handle_);
  Constants(bias_, output_size_, 0);
  Zeros(delta_, output_size_);
  Zeros(output_, output_size_);        
}

void FullLinearLayer::Forward() {
  float alpha = 1.0;
  float beta = 0.0;
  cublasSgemv(handle_, CUBLAS_OP_T, input_num_*input_size_, output_size_, &alpha, 
              weight_, input_num_*input_size_, input_, 1, &beta, output_, 1);   

  cublasSaxpy(handle_, output_size_, &alpha, bias_, 1, output_, 1);    
}

void FullLinearLayer::Backward() {
  if (top_ == NULL) {
    // delta_d = o3 - y 
    cudaMemcpy(delta_, output_, output_size_*sizeof(float), cudaMemcpyDeviceToDevice);
    AddElement(delta_, output_size_, action_, -y_);
  }

  float alpha = 1.0;
  float beta = 0.0;
  cublasSgemv(handle_, CUBLAS_OP_N, input_size_*input_num_, output_size_, &alpha,
              weight_, input_size_*input_num_, delta_, 1, &beta, bottom_->delta_, 1);
}

void FullLinearLayer::GetGradient() {
  Zeros(grad_weight_, input_num_*input_size_*output_size_);        
  Zeros(grad_bias_, output_size_);        

  float alpha = 1.0; 
  float beta = 1.0;
  cublasSgemm(handle_, CUBLAS_OP_N, CUBLAS_OP_N, 
              output_size_, input_num_*input_size_, 1,
              &alpha, delta_, output_size_, input_, 1, &beta, grad_weight_, output_size_);  

  cublasSaxpy(handle_, output_size_, &alpha, delta_, 1, grad_bias_, 1);    

}

void FullLinearLayer::Update() {
  float alpha = - nn_para_->learn_rate_;
  cublasSaxpy(handle_, input_num_*input_size_*output_size_, &alpha, grad_weight_, 1, weight_, 1);    
  cublasSaxpy(handle_, output_size_, &alpha, grad_bias_, 1, bias_, 1);    

}

void FullLinearLayer::WriteWeights(ofstream &file_out) {
  cudaMemcpy(weight_cpu_, weight_, input_num_*input_size_*output_size_*sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(bias_cpu_, bias_, output_size_*sizeof(float), cudaMemcpyDeviceToHost);

  for (int i = 0; i < input_num_*input_size_*output_size_; ++i) {
    file_out << weight_cpu_[i] << " ";
  }

  for (int i = 0; i < output_size_; ++i) {
    file_out << bias_cpu_[i] << " ";
  }

}

void FullLinearLayer::ReadWeights(ifstream &file_in) {
  cout << "FullLinearLayer read weights" << endl;
  for (int i = 0; i < input_num_*input_size_*output_size_; ++i) {
    file_in >> weight_cpu_[i];
  }

  for (int i = 0; i < output_size_; ++i) {
    file_in >> bias_cpu_[i];
  }

  cudaMemcpy(weight_, weight_cpu_, input_num_*input_size_*output_size_*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(bias_, bias_cpu_, output_size_*sizeof(float), cudaMemcpyHostToDevice);
}
