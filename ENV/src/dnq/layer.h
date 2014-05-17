#ifndef LAYER_H
#define LAYER_H
#include <vector>
#include <string>

#include "parameter.h"
#include "common.h"
#include "nn.h"

class Layer {
 public:
  Layer(NNParameter* nn_para, LayerParameter* layer_para) {
    nn_para_ = nn_para;
  };

  ~Layer() {};

  void Im2Col(double* im, int im_size, int filter_size, int stride, double* im_col);
  void Col2Im(double* im_col, int im_size, int filter_size, int stride, double* im);
  void Conv2(double *im, double *Wc, int im_size, int filter_size, int filter_num, double alpha, double *output);
  double Rect(double x);
  void Constants(double *x, int size, double value);
  void Zeros(double *x, int size);
  void RandUniform(double *x, int size, double value);
  void RandGauss(double *x, int size, double value); 

  virtual void Init(Layer* top, Layer* bottom) {};  
  virtual void Forward() {};
  virtual void Backward() {};
  virtual void GetGradient() {} ;
  virtual void Update() {};
  virtual void WriteWeights(std::ofstream& file_out) {};
  virtual void ReadWeights(std::ifstream& file_in) {};
  
  std::string type_;

  int input_im_size_;
  int input_size_;
  int input_num_;  
  int output_im_size_;
  int output_size_;
  int output_num_;  

  int action_;
  int reward_;

  NNParameter* nn_para_;

  double* input_;
  double* output_;
  double* delta_;
  Layer* top_;
  Layer* bottom_;
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

  void Init(Layer* bottom, Layer* top);
  void Forward();
  void Backward();
  void GetGradient();
  void Update();
  void WriteWeights(std::ofstream& file_out);
  void ReadWeights(std::ifstream& file_in);
  
  
  int filter_num_;
  int filter_size_;      
  int stride_;
    double* weight_;
    double* bias_;  
    double* grad_weight_;
    double* grad_bias_;
  double* im_col_;
  double* im_col_getgradient_;
  double* ones_;
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

  void Init(Layer* bottom, Layer* top);
  void Forward();
  void Backward();
  void GetGradient();
  void Update();
  void WriteWeights(std::ofstream& file_out);
  void ReadWeights(std::ifstream& file_in);
  
  double* weight_;
  double* bias_;  
  double* grad_weight_;
  double* grad_bias_;
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

  void Init(Layer* bottom, Layer* top);
  void Forward();
  void Backward();
  void GetGradient();
  void Update();
  void WriteWeights(std::ofstream& file_out);
  void ReadWeights(std::ifstream& file_in);
  
  
  double* weight_;
  double* bias_;  
  double* grad_weight_;
  double* grad_bias_;
};

#endif
