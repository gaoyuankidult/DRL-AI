#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cblas.h>
#include <cstdlib>
#include <cfloat> 

#include "layer.h"
#include "parameter.h"

Layer* GetLayer(NNParameter* nn_para, LayerParameter* layer_para)  {
  if (layer_para->type_ == "conv") {   
    return new ConvLayer(nn_para, layer_para);
  } else if (layer_para->type_ == "full_rect") {   
    return new FullRectLayer(nn_para, layer_para);  
  } else if (layer_para->type_ == "full_linear") {   
    return new FullLinearLayer(nn_para, layer_para);  
  } else {
    std::cout << "no each layer" << std::endl;
  }
}      

void Layer::Im2Col(double* data_im, int im_size, int ksize, int stride, double* data_col) {  
  int height_col = (im_size - ksize) / stride + 1;
  int width_col = (im_size - ksize) / stride + 1;
  int channels_col = ksize * ksize;
  for (int c = 0; c < channels_col; ++c) {
    int w_offset = c % ksize;
    int h_offset = (c / ksize) % ksize;
    int c_im = c / ksize / ksize;
    for (int h = 0; h < height_col; ++h) {
      for (int w = 0; w < width_col; ++w) {     
        int h_pad = h * stride + h_offset;
        int w_pad = w * stride + w_offset;   
        data_col[(c * height_col + h) * width_col + w] =
          data_im[(c_im * im_size + h_pad) * im_size + w_pad];
      }
    }
  }
}

void Layer::Col2Im(double* data_col, int im_size, int ksize, int stride, double* data_im) {  
  memset(data_im, 0, sizeof(double) * im_size * im_size);
  int height_col = (im_size - ksize) / stride + 1;
  int width_col = (im_size - ksize) / stride + 1;
  int channels_col = ksize * ksize;
  for (int c = 0; c < channels_col; ++c) {
    int w_offset = c % ksize;
    int h_offset = (c / ksize) % ksize;
    int c_im = c / ksize / ksize;
    for (int h = 0; h < height_col; ++h) {
      for (int w = 0; w < width_col; ++w) {
        int h_pad = h * stride + h_offset;
        int w_pad = w * stride + w_offset;
          data_im[(c_im * im_size + h_pad) * im_size + w_pad] +=
              data_col[(c * height_col + h) * width_col + w];
      }
    }
  }
}

double Layer::Rect(double x) {
  return std::max(0.0, x);
}

void Layer::Constants(double *x, int size, double value) {
  for (int i = 0; i < size; ++i) {
    x[i] = value;
  }
}

void Layer::Zeros(double *x, int size) {  
  memset(x, 0, size*sizeof(double));
}

void Layer::RandUniform(double *x, int size, double value) {
  for (int i = 0; i < size; ++i) {
    x[i] = value * (double) (rand() + 1.0) / (RAND_MAX+1.0);
  } 
}

void Layer::RandGauss(double *x, int size, double value) {    
  for (int i = 0; i < size; ++i) {
    double rand_1 = (double) (rand() + 1.0) / (RAND_MAX+1.0);
    double rand_2 = (double) (rand() + 1.0) / (RAND_MAX+1.0);
    x[i] = value * sqrt(-2*log(rand_1)) * cos(2*3.14159265*rand_2);
  } 
}
        
void ConvLayer::Init(Layer* bottom, Layer* top) {    
  if (bottom == NULL) {
    input_ = NULL;      
    input_im_size_ = nn_para_->im_size_;
    input_size_ = input_im_size_*input_im_size_;
    input_num_ = 4;
  } else if (bottom != NULL) {
    input_ = bottom->output_;
    input_im_size_ = bottom->output_im_size_;    
    input_size_ = bottom->output_size_;  
    input_num_ = bottom->output_num_;    
  }

  bottom_ = bottom;
  top_ = top;

  output_im_size_ = (input_im_size_ - filter_size_) / stride_ + 1;
  output_size_ = output_im_size_*output_im_size_;  

  output_ = new double[filter_num_*output_size_];
  delta_ = new double[filter_num_*output_size_];
  weight_ = new double[input_num_*filter_num_*filter_size_*filter_size_];
  bias_ = new double[input_num_*filter_num_];      
  grad_weight_ = new double[input_num_*filter_num_*filter_size_*filter_size_];
  grad_bias_ = new double[input_num_*filter_num_];
  // im_col_ = new double[output_im_size_*output_im_size_*filter_size_*filter_size_];
  // to be fixed *stride_??? not sure
  im_col_ = new double[output_im_size_*output_im_size_*(filter_size_ + 1)*(filter_size_ + 1)*stride_];
  ones_ = new double[output_im_size_*output_im_size_];

  RandGauss(weight_, input_num_*filter_num_*filter_size_*filter_size_, 0.001);
  Zeros(bias_, input_num_*filter_num_);        
  Zeros(delta_, filter_num_*output_size_);
  Constants(ones_, output_im_size_*output_im_size_, 1.0);
}

void ConvLayer::Forward() {    
  Zeros(output_, filter_num_*output_size_);
  for (int i = 0; i < input_num_; ++i) {
    Im2Col(input_ + i*input_im_size_*input_im_size_, input_im_size_, filter_size_, stride_, im_col_);  
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
                filter_num_, output_im_size_*output_im_size_, filter_size_*filter_size_,
                1.0, weight_ + i*filter_num_*filter_size_*filter_size_, filter_size_*filter_size_, im_col_, output_im_size_*output_im_size_, 
                1, output_, output_im_size_*output_im_size_);  
  }
  
  for (int i = 0; i < filter_num_; ++i) {
    cblas_daxpy(output_im_size_*output_im_size_, bias_[i], ones_, 1, output_ + i*output_im_size_*output_im_size_, 1);
  }

  for (int i = 0; i < filter_num_*output_size_; ++i) {
    output_[i] = Rect(output_[i]);
  }
}

void ConvLayer::Backward() {  
  if (bottom_ == NULL) {
    return;
  }

  for (int i = 0; i < input_num_; ++i) {
    memset(im_col_, 0, output_im_size_*output_im_size_*filter_size_*filter_size_*sizeof(double));         
    cblas_dgemm(CblasRowMajor, CblasTrans, CblasNoTrans, 
                filter_size_*filter_size_, output_im_size_*output_im_size_,  filter_num_,
                1, weight_ + i*filter_num_*filter_size_*filter_size_, 
                filter_size_*filter_size_, delta_, output_im_size_*output_im_size_, 1, im_col_, output_im_size_*output_im_size_);  
    Col2Im(im_col_, input_im_size_, filter_size_, stride_, bottom_->delta_ + i*input_im_size_*input_im_size_);        
  }

  // applying the deriative of rectifier function
  for (int i = 0; i < input_num_*input_im_size_*input_im_size_; ++i) {
    if (bottom_->output_[i] < 0.0) {
      bottom_->delta_[i] = 0.0;
    }
  }
}  

void ConvLayer::GetGradient() {
  Zeros(grad_weight_, input_num_*filter_num_*filter_size_*filter_size_);
  Zeros(grad_bias_, input_num_*filter_num_);

  // Convolute input_ with delta_  
  int CONV_DIM = filter_size_ + 1;
  int FILTER_SIZE = output_im_size_;
  for (int i = 0; i < input_num_; ++i) {

    Im2Col(input_ + i*input_im_size_*input_im_size_, input_im_size_, FILTER_SIZE, stride_, im_col_);  

      cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
                filter_num_, CONV_DIM*CONV_DIM, FILTER_SIZE*FILTER_SIZE,
                (double)1/(nn_para_->mb_size_), delta_, FILTER_SIZE*FILTER_SIZE, im_col_, 
                CONV_DIM*CONV_DIM, 1, grad_weight_ + i*filter_num_*filter_size_*filter_size_, CONV_DIM*CONV_DIM);
  }

  // grad_bc2
  for (int i = 0; i < filter_num_; ++i) {
    cblas_dgemv(CblasRowMajor, CblasNoTrans, 1, output_im_size_*output_im_size_, (double)1/(nn_para_->mb_size_), 
                delta_ + i*output_im_size_*output_im_size_, output_im_size_*output_im_size_, ones_, 1, 1, &grad_bias_[i], 1);          
  }

}

void ConvLayer::Update() {
  cblas_daxpy(input_num_*filter_num_*filter_size_*filter_size_, - nn_para_->learn_rate_, grad_weight_, 1, weight_, 1);    
  cblas_daxpy(filter_num_, - nn_para_->learn_rate_, grad_bias_, 1, bias_, 1);    
}

void ConvLayer::WriteWeights(std::ofstream& file_out) {
  for (int i = 0; i < input_num_*filter_num_*filter_size_*filter_size_; ++i) {
    file_out << weight_[i] << " ";
  }

  for (int i = 0; i < input_num_*filter_num_; ++i) {
    file_out << bias_[i] << " ";
  }
}

void ConvLayer::ReadWeights(std::ifstream& file_in) {
  std::cout << "ConvLayer read weights" << std::endl;
  for (int i = 0; i < input_num_*filter_num_*filter_size_*filter_size_; ++i) {
    file_in >> weight_[i];
  }

  for (int i = 0; i < input_num_*filter_num_; ++i) {
    file_in >> bias_[i];
  }

  std::cout << weight_[0] << " " << weight_[input_num_*filter_num_*filter_size_*filter_size_-1] << " " << std::endl;
  std::cout << bias_[0] << " " << bias_[input_num_*filter_num_-1] << " " << std::endl;
  std::cout << std::endl;
}

void FullRectLayer::Init(Layer* bottom, Layer* top) {
  if (bottom == NULL) {
    input_ = NULL;      
    input_im_size_ = nn_para_->im_size_;
    input_size_ = output_im_size_*output_im_size_;
    input_num_ = 1;
  } else {
    input_ = bottom->output_;
    input_im_size_ = bottom->output_im_size_;      
    input_size_ = bottom->output_size_;  
    input_num_ = bottom->output_num_;    
  }

  bottom_ = bottom;
  top_ = top;

  output_im_size_ = 0;
  output_num_ = 1;    

  output_ = new double[output_size_];
  delta_ = new double[output_size_];
  weight_ = new double[input_num_*input_size_*output_size_];
  bias_ = new double[output_size_];    
  grad_weight_ = new double[input_num_*input_size_*output_size_];
  grad_bias_ = new double[output_size_];
  
  Zeros(delta_, output_size_);
  RandGauss(weight_, input_num_*input_size_*output_size_, 0.001);
  Zeros(bias_, output_size_);  

}

void FullRectLayer::Forward() {
  Zeros(output_, output_size_);
  cblas_dgemv(CblasRowMajor, CblasNoTrans, output_size_, input_num_*input_size_, 1.0, 
              weight_, input_num_*input_size_, input_, 1, 0.0, output_, 1);   

  cblas_daxpy(output_size_, 1, bias_, 1, output_, 1);    

  for (int i = 0; i < output_size_; ++i) {
    output_[i] = Rect(output_[i]);
  }

}

void FullRectLayer::Backward() {
  if (top_ == NULL) {
    // delta_d = o3 - yLayer* bottom
    memcpy(delta_, output_, output_size_*sizeof(double));
    delta_[action_] -= reward_;
  }

  cblas_dgemv(CblasRowMajor, CblasTrans, output_size_, input_size_*input_num_, 1.0,
              weight_, input_size_*input_num_, delta_, 1, 0.0, bottom_->delta_, 1);


  for (int i = 0; i < input_size_*input_num_; ++i) {
    if (bottom_->output_[i] < 0.0) {
      bottom_->delta_[i] = 0.0;
    }
  }

}

void FullRectLayer::GetGradient() {
  Zeros(grad_weight_, input_size_*output_size_);
  Zeros(grad_bias_, output_size_);
  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
              input_num_*input_size_, output_size_, 1,
              (double)1/(nn_para_->mb_size_), input_ , 1, delta_, output_size_, 1, grad_weight_, output_size_);  

  cblas_daxpy(output_size_, (double)1/(nn_para_->mb_size_), delta_, 1, grad_bias_, 1);    
}

void FullRectLayer::Update() {
  cblas_daxpy(input_num_*input_size_*output_size_, - nn_para_->learn_rate_, grad_weight_, 1, weight_, 1);    
  cblas_daxpy(output_size_, - nn_para_->learn_rate_, grad_bias_, 1, bias_, 1);    
}

void FullRectLayer::WriteWeights(std::ofstream& file_out) {
  for (int i = 0; i < input_num_*input_size_*output_size_; ++i) {
    file_out << weight_[i] << " ";
  }

  for (int i = 0; i < output_size_; ++i) {
    file_out << bias_[i] << " ";
  }
}

void FullRectLayer::ReadWeights(std::ifstream& file_in) {
  std::cout << "FullRectLayer read weights" << std::endl;
  for (int i = 0; i < input_num_*input_size_*output_size_; ++i) {
    file_in >> weight_[i];
  }

  for (int i = 0; i < output_size_; ++i) {
    file_in >> bias_[i];
  }
  std::cout << weight_[0] << " " << weight_[input_num_*input_size_*output_size_-1] << " " << std::endl;
  std::cout << bias_[0] << " " << bias_[output_size_-1] << " " << std::endl;
  std::cout << std::endl ;
}

void FullLinearLayer::Init(Layer* bottom, Layer* top) {
  if (bottom == NULL) {
    input_ = NULL;      
    input_im_size_ = nn_para_->im_size_;
    input_size_ = output_im_size_*output_im_size_;
    input_num_ = 1;
  } else {
    input_ = bottom->output_;
    input_im_size_ = bottom->output_im_size_;      
    input_size_ = bottom->output_size_;  
    input_num_ = bottom->output_num_;    
  }

  bottom_ = bottom;
  top_ = top;

  output_im_size_ = 0;
  output_num_ = 1;    

  output_ = new double[output_size_];
  delta_ = new double[output_size_];
  weight_ = new double[input_num_*input_size_*output_size_];
  bias_ = new double[output_size_];    
  grad_weight_ = new double[input_num_*input_size_*output_size_];
  grad_bias_ = new double[output_size_];
  
  Zeros(delta_, output_size_);
  RandGauss(weight_, input_num_*input_size_*output_size_, 0.0001);
  Zeros(bias_, output_size_);  

}

void FullLinearLayer::Forward() {
  Zeros(output_, output_size_);
  cblas_dgemv(CblasRowMajor, CblasNoTrans, output_size_, input_num_*input_size_, 1.0, 
              weight_, input_num_*input_size_, input_, 1, 0.0, output_, 1);   

  cblas_daxpy(output_size_, 1, bias_, 1, output_, 1);    
}

void FullLinearLayer::Backward() {
  if (top_ == NULL) {
    // delta_d = o3 - yLayer* bottom
    memcpy(delta_, output_, output_size_*sizeof(double));
    delta_[action_] -= reward_;
  }

  cblas_dgemv(CblasRowMajor, CblasTrans, output_size_, input_size_*input_num_, 1.0,
              weight_, input_size_*input_num_, delta_, 1, 0.0, bottom_->delta_, 1);

}

void FullLinearLayer::GetGradient() {
  Zeros(grad_weight_, input_size_*output_size_);
  Zeros(grad_bias_, output_size_);
  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
              input_num_*input_size_, output_size_, 1,
              (double)1/(nn_para_->mb_size_), input_ , 1, delta_, output_size_, 1, grad_weight_, output_size_);  

  cblas_daxpy(output_size_, (double)1/(nn_para_->mb_size_), delta_, 1, grad_bias_, 1);    

  printCUDA2(grad_weight_, 10);
  printCUDA2(input_, 10);
}

void FullLinearLayer::Update() {
  cblas_daxpy(input_num_*input_size_*output_size_, - nn_para_->learn_rate_, grad_weight_, 1, weight_, 1);    
  cblas_daxpy(output_size_, - nn_para_->learn_rate_, grad_bias_, 1, bias_, 1);    
}


void FullLinearLayer::WriteWeights(std::ofstream& file_out) {
  for (int i = 0; i < input_num_*input_size_*output_size_; ++i) {
    file_out << weight_[i] << " ";
  }

  for (int i = 0; i < output_size_; ++i) {
    file_out << bias_[i] << " ";
  }

}

void FullLinearLayer::ReadWeights(std::ifstream& file_in) {
  std::cout << "FullLinearLayer read weights" << std::endl;
  for (int i = 0; i < input_num_*input_size_*output_size_; ++i) {
    file_in >> weight_[i];
  }

  for (int i = 0; i < output_size_; ++i) {
    file_in >> bias_[i];
  }

  std::cout << weight_[0] << " " << weight_[input_num_*input_size_*output_size_-1] << " " << std::endl;
  std::cout << bias_[0] << " " << bias_[output_size_-1] << " " << std::endl;
  std::cout << std::endl;
}
