#ifndef LAYER_H
#define LAYER_H
#include "parameter.h"
#include "common.h"
#include "math_functions.h"

class Layer {
 public:
  Layer(NNParameter* nn_para, LayerParameter* layer_para) {
    nn_para_ = nn_para;
  };

  ~Layer() {};

  virtual void Init(Layer* top, Layer* bottom, curandGenerator_t gen, cublasHandle_t handle) {};  
  virtual void Forward() {};
  virtual void Backward() {};
  virtual void GetGradient() {} ;
  virtual void Update() {};
  virtual void WriteWeights(ofstream &file_out) {};
  virtual void ReadWeights(ifstream &file_in) {};
  
  string type_;

  int input_im_size_;
  int input_size_;
  int input_num_;  
  int output_im_size_;
  int output_size_;
  int output_num_;  

  int action_;
  float y_;

  NNParameter* nn_para_;

  Layer* top_;
  Layer* bottom_;
  float* input_;
  float* output_;
  float* delta_;
  float* weight_;
  float* weight_cpu_;
  float* bias_;  
  float* bias_cpu_;  
  float* grad_weight_;
  float* grad_bias_;

  curandGenerator_t gen_;
  cublasHandle_t handle_; 
};


Layer* GetLayer(NNParameter* nn_para, LayerParameter *layer_para);

class ConvLayer : public Layer {  
 public: 
  ConvLayer(NNParameter* nn_para, LayerParameter *layer_para) : Layer(nn_para, layer_para) {
    type_ = "conv";    
    filter_num_ = layer_para->filter_num_;
    filter_size_ = layer_para->filter_size_;  
    stride_ = layer_para->stride_;    
    output_num_ = filter_num_;    
  };

  ~ConvLayer() {    
    delete[] output_;
    delete[] delta_;
    delete[] weight_;
    delete[] bias_;
    delete[] grad_weight_;
    delete[] grad_bias_;
  };  

  void Init(Layer* bottom, Layer* top, curandGenerator_t gen, cublasHandle_t handle);
  void Forward();
  void Backward();
  void GetGradient();
  void Update();
  void WriteWeights(ofstream& file_out);
  void ReadWeights(ifstream& file_in);
  
  int filter_num_;
  int filter_size_;      
  int stride_;
  float* im_col_;
  float* im_col_getgradient_;
  float* ones_;
};

class FullRectLayer : public Layer {  
 public:   
  FullRectLayer(NNParameter* nn_para, LayerParameter *layer_para) : Layer(nn_para, layer_para) {    
    type_ = "full_rect";
    output_im_size_ = 0;
    output_size_ = layer_para->output_size_;
    output_num_ = 1;
  };

  ~FullRectLayer() {
    delete[] output_;
    delete[] delta_;    
    delete[] weight_;
    delete[] bias_;  
    delete[] grad_weight_;
    delete[] grad_bias_;
  };      

  void Init(Layer* bottom, Layer* top, curandGenerator_t gen, cublasHandle_t handle);
  void Forward();
  void Backward();
  void GetGradient();
  void Update();
  void WriteWeights(ofstream &file_out);
  void ReadWeights(ifstream &file_in);
  
};

class FullLinearLayer : public Layer {  
 public:   
  FullLinearLayer(NNParameter* nn_para, LayerParameter *layer_para) : Layer(nn_para, layer_para) {    
    type_ = "full_linear";
    output_im_size_ = 0;
    output_size_ = layer_para->output_size_;
    output_num_ = 1;
  };

  ~FullLinearLayer() {
    delete[] output_;
    delete[] delta_;    
    delete[] weight_;
    delete[] bias_;  
    delete[] grad_weight_;
    delete[] grad_bias_;
  };      

  void Init(Layer* bottom, Layer* top, curandGenerator_t gen, cublasHandle_t handle);
  void Forward();
  void Backward();
  void GetGradient();
  void Update();
  void WriteWeights(ofstream &file_out);
  void ReadWeights(ifstream &file_in);
    
};

#endif
