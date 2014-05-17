#include "nn.h"

void NN::Init() {   
  
  curandCreateGenerator(&gen_, CURAND_RNG_PSEUDO_DEFAULT);
  curandSetPseudoRandomGeneratorSeed(gen_, 132);
  cublasCreate(&handle_);

  for (int i = 0; i < nn_para_->layer_para_.size(); ++i) {
    cout << "getting layer: " << i << endl;
    Layer* new_layer = GetLayer(nn_para_, nn_para_->layer_para_[i]);    

    if (new_layer == NULL) {
      cerr << "empty layer" << endl;
    }

    layers_.push_back(new_layer);    
  }

  for (int i = 0; i < nn_para_->layer_para_.size(); ++i) {
    if (i == 0) {    
      layers_[i]->Init(NULL, layers_[i+1], gen_, handle_);  
    } else if (i == nn_para_->layer_para_.size() - 1) {
      layers_[i]->Init(layers_[i-1], NULL, gen_, handle_);  
    } else {      
      layers_[i]->Init(layers_[i-1], layers_[i+1], gen_, handle_);  
      layers_[i]->input_ = layers_[i-1]->output_;

      if (i > 0) { 
        if (layers_[i]->input_ != layers_[i-1]->output_) { 
	        cerr << i << " input != last output" << endl;
        }
      }
    }
  }
  
  memory_size_ = 1000;
  memory_idx_ = 0;
  memory_full_ = false;
  action_memory_ = (int*)malloc(memory_size_*sizeof(int));
  reward_memory_ = (float*)malloc(memory_size_*sizeof(float));
  cudaMalloc(&im_memory_, memory_size_*im_size_*im_size_*channel_num_*sizeof(float));
  output_cpu_ = (float*)malloc(action_num_*sizeof(float));
  
  cout << "Network Initialization succeeded" << endl;
}

void NN::AddMemIm(float *im) {
  int offset = im_size_*im_size_*channel_num_;
  cudaMemcpy(&im_memory_[memory_idx_ * offset], im, offset*sizeof(float), cudaMemcpyHostToDevice);
}

void NN::AddMemActionReward(int action, float reward) {
  action_memory_[memory_idx_] = action;
  reward_memory_[memory_idx_] = reward;
  
  (memory_idx_)++;
  memory_idx_ = memory_idx_ % memory_size_;
}

void NN::Forward(int idx) {
  int offset = im_size_*im_size_*channel_num_;
  layers_[0]->input_ = &im_memory_[idx * offset];

  for (int j = 0; j < layers_.size(); ++j) {        
    layers_[j]->Forward();          

    if (j > 0) {
      if (layers_[j]->input_ != layers_[j-1]->output_) {	 
        cerr << j << " input != last output" << endl;
        exit(1);
      }
    }
  }   
}

void NN::Train() {
  int idx;
  if (memory_full_ == true) {
    idx = rand() % (memory_size_ - 1) + 1;
  } else {
    if (memory_idx_ == 0) {
      return;
    } else {
      idx = rand() % memory_idx_ + 1;
    }
  }

  layers_[layers_.size() - 1]->action_ = action_memory_[idx];

  if (reward_memory_[idx] == -10) {
    layers_[layers_.size() - 1]->y_ = -10;
  } else {
    Forward(idx);
    int max_idx;
    float max_value;
    MaxOutput(layers_[layers_.size() - 1]->output_, action_num_, &max_value, &max_idx, handle_);
    layers_[layers_.size() - 1]->y_ = reward_memory_[idx] + 0.1*max_value;
  }

  Forward(idx - 1);
  for (int j = layers_.size() - 1; j >= 0; --j) {
    layers_[j]->Backward();                
  }      

  for (int j = 0; j < layers_.size(); ++j) {   
    layers_[j]->GetGradient();               
  }

  for (int j = 0; j < layers_.size(); ++j) {   
    layers_[j]->Update();             
  }    
}

void NN::GetOutput() {
  cudaMemcpy(output_cpu_, layers_[layers_.size() - 1]->output_, action_num_*sizeof(float), cudaMemcpyDeviceToHost);
}


void NN::WriteWeights(const char* file_name) {
  ofstream file_out(file_name);
  for (int i = 0; i < layers_.size(); ++i) {
    layers_[i]->WriteWeights(file_out);
  }

  file_out.close();
}

void NN::ReadWeights(const char* file_name) {
  ifstream file_in(file_name);

  if (!file_in.is_open()) {
    cout << "No weights file to read" << endl;
    return;
  }

  for (int i = 0; i < layers_.size(); ++i) {
    layers_[i]->ReadWeights(file_in);
  }

  file_in.close();
}

