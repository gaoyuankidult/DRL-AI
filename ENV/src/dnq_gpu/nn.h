#ifndef NN_H
#define NN_H

#include "parameter.h"
#include "layer.h"
#include "common.h"
#include "math_functions.h"

class Layer;

class NN {
 public:               
  NN(NNParameter* nn_para) {
    im_size_ = nn_para->im_size_;
    channel_num_ = nn_para->channel_num_; 
    action_num_ = nn_para->action_num_;
    mb_size_ = nn_para->mb_size_;  
    learn_rate_ = nn_para->learn_rate_;
    nn_para_ = nn_para;    
  }

  ~NN() {}

  void Init();
  void AddMemIm(float *im);
  void AddMemActionReward(int action, float reward);
  void Forward(int idx);
  void Train();
  void GetOutput();

  void WriteWeights(const char* file_name);
  void ReadWeights(const char* file_name);

    
  int im_size_;
  int channel_num_; 
  int action_num_;
  int layer_num_;
  int mb_size_;  
  float learn_rate_;

  float *output_cpu_;
  float *im_memory_;
  float *im_memory_pre_;
  float *reward_memory_;
  int *action_memory_;
  
  int memory_idx_;
  int memory_size_;
  bool memory_full_;

  NNParameter* nn_para_;
  vector<Layer*> layers_;

  curandGenerator_t gen_;
  cublasHandle_t handle_;
};


#endif
