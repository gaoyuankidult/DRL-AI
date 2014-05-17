#include <iostream>
#include <sstream> 
#include <fstream>
#include <vector>
#include <string>
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <cblas.h>
#include <memory>

#include "nn.h"
#include "layer.h"
#include "common.h"
#include "parameter.h"

int main() {

  NNParameter* nn_para = new NNParameter;

  nn_para->im_size_ = 32;
  nn_para->data_batch_num_ = 5;
  nn_para->data_batch_size_ = 10000;    
  nn_para->sample_size_ = nn_para->data_batch_num_ * nn_para->data_batch_size_;
  nn_para->channel_num_ = 3; 
  nn_para->class_num_ = 10;

  nn_para->learn_rate_ = 0.001;
  nn_para->mb_size_ = 50;
  nn_para->epoch_num_ = 10;
  
  // input data file names
  for (int i = 0; i < nn_para->data_batch_num_; ++i) {
    std::stringstream data_batch_idx;
    data_batch_idx << i + 1;
    std::string this_file_name = "./data/data_batch_" + data_batch_idx.str() + ".bin";
    nn_para->file_names_.push_back(this_file_name);
  }

  // layer parameters  
  LayerParameter* conv1 = new LayerParameter;
  LayerParameter* pool1 = new LayerParameter;
  LayerParameter* conv2 = new LayerParameter;
  LayerParameter* pool2 = new LayerParameter;
  LayerParameter* full1 = new LayerParameter;  

  conv1->type_ = "conv";  
  conv1->input_im_size_ = nn_para->im_size_;
  conv1->input_size_ = (conv1->input_im_size_)*(conv1->input_im_size_);
  conv1->filter_size_ = 5;
  conv1->filter_num_ = 10;    

  pool1->type_ = "pool";  
  pool1->pool_dim_ = 2;    
  
  conv2->type_ = "conv";  
  conv2->filter_size_ = 5;
  conv2->filter_num_ = 10;    
  
  pool2->type_ = "pool";  
  pool2->pool_dim_ = 2;
  
  full1->type_ = "full";  
  full1->output_size_ = nn_para->class_num_;

  nn_para->layer_para_.push_back(conv1);
  nn_para->layer_para_.push_back(pool1);
  nn_para->layer_para_.push_back(conv2);
  nn_para->layer_para_.push_back(pool2);
  nn_para->layer_para_.push_back(full1);
  
  NN* nn = new NN(nn_para);
  
  nn->Init();
  nn->LoadData();
  nn->Train();
  
  // ConvLayer* layer_ptr = dynamic_cast<ConvLayer*>(nn->layers_[0]);

  // nn->RandGauss(layer_ptr->weight_,10);


  // for (int i = 0; i < 10; ++i)
  // {
  //   std::cout << layer_ptr->weight_[i] << std::endl;
  // }
  
  // }
  
  

  delete nn;



	return 0;
}
