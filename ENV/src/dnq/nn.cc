#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cfloat>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <cblas.h>

#include "nn.h"
#include "layer.h"
#include "common.h"

void NN::Init() {     
  for (int i = 0; i < nn_para_->layer_para_.size(); ++i) {
    Layer* new_layer = GetLayer(nn_para_, nn_para_->layer_para_[i]);    
    layers_.push_back(new_layer);    
  }

  for (int i = 0; i < nn_para_->layer_para_.size(); ++i) {
    if (i == 0) {    
      layers_[i]->Init(NULL, layers_[i+1]);  
    } else if (i == nn_para_->layer_para_.size() - 1) {
      layers_[i]->Init(layers_[i-1], NULL);  
    } else {      
      layers_[i]->Init(layers_[i-1], layers_[i+1]);  
    }
  }

  std::cout << "Network Initialization succeeded" << std::endl;
}

void NN::RandPerm(int *randperm, int size) {  
  for (int i = 0; i < size; ++i) {
    randperm[i] = i;
  }
  for (int i = 0; i < size; ++i) {
    int j = rand()%(size - i) + i;
    int t = randperm[j];
    randperm[j] = randperm[i];
    randperm[i] = t;
  }
}


void NN::Forward(double* im) {
  layers_[0]->input_ = im;

  for (int j = 0; j < layers_.size(); ++j) {        
    layers_[j]->Forward();               
  }   
}

void NN::Train(double* im, double y, int act) {

  layers_[0]->input_ = im;

  for (int j = 0; j < layers_.size(); ++j) {        
    layers_[j]->Forward();               
  }      

  layers_[layers_.size() - 1]->action_ = act;
  layers_[layers_.size() - 1]->reward_ = y;
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

void NN::WriteWeights(const char* file_name) {
  std::ofstream file_out(file_name);
  for (int i = 0; i < layers_.size(); ++i) {
    layers_[i]->WriteWeights(file_out);
  }

  file_out.close();
}

void NN::ReadWeights(const char* file_name) {
  std::ifstream file_in(file_name);

  if (!file_in.is_open()) {
    std::cout << "No weights file to read" << std::endl;
    return;
  }

  for (int i = 0; i < layers_.size(); ++i) {
    layers_[i]->ReadWeights(file_in);
  }

  file_in.close();
}

