#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h> 
#include <cblas.h>
#include <string.h>
#include <float.h>
#include <math.h>

#define IM_SIZE 32
#define FILE_NAME_SIZE 50
#define BATCH_SIZE 10000
#define BATCH_NUM 5

#define CHANNEL_NUM 3
#define FILTER_SIZE_1 5
#define FILTER_NUM_1 10
#define CONV_DIM_1 (IM_SIZE - FILTER_SIZE_1 + 1)
#define POOL_DIM_1 2
#define OUTPUT_DIM_1 (CONV_DIM_1 / POOL_DIM_1)
#define FILTER_SIZE_2 5
#define FILTER_NUM_2 10
#define CONV_DIM_2 (OUTPUT_DIM_1 - FILTER_SIZE_2 + 1)
#define POOL_DIM_2 2
#define OUTPUT_DIM_2 (CONV_DIM_2 / POOL_DIM_2)
#define CLASS_NUM 10
#define EPOCH 1000

#define SAMPLE_SIZE (BATCH_NUM * BATCH_SIZE)
#define MB_SIZE 100

#define R 0.0005
#define MOM 0.9

void LoadCIFAR10(uint8_t **data_im_RGB, uint8_t *labels) {  
  FILE *im_file;
  int i, j, im_idx = 0;

  char file_name[5][30];
  strcpy(file_name[0], "./data/data_batch_1.bin");
  strcpy(file_name[1], "./data/data_batch_2.bin");
  strcpy(file_name[2], "./data/data_batch_3.bin");
  strcpy(file_name[3], "./data/data_batch_4.bin");
  strcpy(file_name[4], "./data/data_batch_5.bin");    
  
  for (i = 0; i < BATCH_NUM; ++i) {
    im_file = fopen(file_name[i], "r");
    for (j = 0; j < BATCH_SIZE; ++j) {      
      fread(&labels[im_idx], 1, 1, im_file);
      fread(data_im_RGB[im_idx], 1, IM_SIZE*IM_SIZE*CHANNEL_NUM, im_file);  
      im_idx++;
    }        
    fclose(im_file);
  }
  printf("Data loading succeeded\n");
}

void RGB2Gray(uint8_t *im_RGB, double *im_gray) {
  int i;
  for (i = 0; i < IM_SIZE*IM_SIZE; ++i) {
    im_gray[i] = 0.2989*im_RGB[i] 
               + 0.5870*im_RGB[i+IM_SIZE*IM_SIZE]
               + 0.1140*im_RGB[i+2*IM_SIZE*IM_SIZE];
  }
}

void Im2Col(double* im, int im_size, int filter_size, double* im_col) {
  int c, h, w;
  int height_col = im_size - filter_size + 1;
  int width_col = im_size - filter_size + 1;
  int channels_col = filter_size * filter_size;
  for (c = 0; c < channels_col; ++c) {
    int w_offset = c % filter_size;
    int h_offset = (c / filter_size) % filter_size;
    int c_im = c / filter_size / filter_size;
    for (h = 0; h < height_col; ++h) {
      for (w = 0; w < width_col; ++w) {
        im_col[(c * height_col + h) * width_col + w] =
            im[(c_im * im_size + h + h_offset) * im_size
                + w + w_offset];
      }
    }
  }
}

void Col2Im(double* im_col, int im_size, int filter_size, double* im) {
  int c, h, w;  
  int height_col = im_size - filter_size + 1;
  int width_col = im_size - filter_size + 1;
  int channels_col = filter_size*filter_size;
  memset(im, 0, im_size*im_size*sizeof(double));
  for (c = 0; c < channels_col; ++c) {
    int w_offset = c % filter_size;
    int h_offset = (c / filter_size) % filter_size;
    int c_im = c / filter_size / filter_size;
    for (h = 0; h < height_col; ++h) {
      for (w = 0; w < width_col; ++w) {
        im[(c_im * im_size + h  + h_offset) * im_size + w 
           + w_offset] += im_col[(c * height_col + h) * width_col + w];
      }
    }
  }
}


void Conv2_corr(double *im, double *Wc, int im_size, int filter_size, int filter_num, double alpha, double *output) {
  int conv_dim = im_size - filter_size + 1;
  double *im_col = malloc(conv_dim*conv_dim*filter_size*filter_size*sizeof(double));    
  Im2Col(im, im_size, filter_size, im_col);  
  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
              filter_num, conv_dim*conv_dim, filter_size*filter_size,
              alpha, Wc, filter_size*filter_size, im_col, conv_dim*conv_dim, 1, output, conv_dim*conv_dim);
  free(im_col);
}

void MaxPool(double *im, int im_size, int pool_dim, double *pooled, int *poolIdx) {  
  int i, j, m, n;
  int output_dim = im_size / pool_dim;
  int max_idx = 0;
  int this_idx = 0;
  double max_value = 0.0;  
  double this_value = 0.0;
  for (i = 0; i < output_dim; ++i) {
    for (j = 0; j < output_dim; ++j) {
      max_value = -DBL_MAX;
      max_idx = 0;
      for (m = 0; m < pool_dim; ++m) {
        for (n = 0; n < pool_dim; ++n) {
          this_idx = (i*pool_dim + m)*im_size + j*pool_dim + n;
          this_value = im[this_idx];
          if (this_value > max_value) {
            max_value = this_value;
            max_idx = this_idx;
          }
        }              
      }
      pooled[i*output_dim + j] = max_value;
      poolIdx[i*output_dim + j] = max_idx;      
    }
  }
}

void UpSample(double *pooled, int *pool_idx, int output_dim, int pool_dim, double *up) {
  int i;
  for (i = 0; i < output_dim*output_dim; ++i) {
    up[pool_idx[i]] = pooled[i];
  }
}

void RandPerm(int *randperm, int size) {
  int i, j, t;
  for (i = 0; i < size; ++i) {
    randperm[i] = i;
  }
  for (i = 0; i < size; ++i) {
    j = rand()%(size - i) + i;
    t = randperm[j];
    randperm[j] = randperm[i];
    randperm[i] = t;
  }
}

void Ones(double *x, int size) {
  int i;
  for (i = 0; i < size; ++i) {
    x[i] = 1.0;
  }
}

double RandGauss() {
  static double V1, V2, S;
  static int phase = 0;
  double X;

  if (phase == 0) {
    do {
      double U1 = (double)rand() / RAND_MAX;
      double U2 = (double)rand() / RAND_MAX;

      V1 = 2 * U1 - 1;
      V2 = 2 * U2 - 1;
      S = V1 * V1 + V2 * V2;
      } while (S >= 1 || S == 0);

    X = V1 * sqrt(-2 * log(S) / S);
  } else {
    X = V2 * sqrt(-2 * log(S) / S);
  }

  phase = 1 - phase;

  return X;
}

int main() {     
  int i, j;
  srand (time(NULL));

  // Load data
  uint8_t **data_im_RGB = malloc(SAMPLE_SIZE*sizeof(uint8_t*));
  for (i = 0; i < SAMPLE_SIZE; ++i) {
  	data_im_RGB[i] = malloc(IM_SIZE*IM_SIZE*CHANNEL_NUM*sizeof(uint8_t));
  }
  uint8_t *labels = malloc(SAMPLE_SIZE*sizeof(uint8_t));
  LoadCIFAR10(data_im_RGB, labels);
  
  // Transform images from RGB to grayscale 
  double **data_im = malloc(SAMPLE_SIZE*sizeof(double*));
  for (i = 0; i < SAMPLE_SIZE; ++i) {
    data_im[i] = malloc(IM_SIZE*IM_SIZE*sizeof(double));
    RGB2Gray(data_im_RGB[i], data_im[i]);      
  }

  double data_mean = 0;
  for (i = 0; i < SAMPLE_SIZE; ++i) {
    for (j = 0; j < IM_SIZE*IM_SIZE; ++j) {
      data_mean += (double) data_im_RGB[i][j]/(IM_SIZE*IM_SIZE*SAMPLE_SIZE);
    }
  }
  for (i = 0; i < SAMPLE_SIZE; ++i) {
    for (j = 0; j < IM_SIZE*IM_SIZE; ++j) {
      data_im_RGB[i][j] -= data_mean;
    }
  }

  for (i = 0; i < SAMPLE_SIZE; ++i) {
    free(data_im_RGB[i]);
  }
  free(data_im_RGB);

  // Initialize parameters
  double *Wc1 = malloc(FILTER_NUM_1*FILTER_SIZE_1*FILTER_SIZE_1*sizeof(double));
  double *Wc2 = malloc(FILTER_NUM_1*FILTER_NUM_2*FILTER_SIZE_2*FILTER_SIZE_2*sizeof(double));
  double *Wd = malloc(CLASS_NUM*FILTER_NUM_2*OUTPUT_DIM_2*OUTPUT_DIM_2*sizeof(double));
  double *bc1 = malloc(FILTER_NUM_1*sizeof(double));  
  double *bc2 = malloc(FILTER_NUM_2*sizeof(double));
  double *bd = malloc(CLASS_NUM*sizeof(double));

  double *grad_Wc1 = malloc(FILTER_NUM_1*FILTER_SIZE_1*FILTER_SIZE_1*sizeof(double));
  double *grad_Wc2 = malloc(FILTER_NUM_1*FILTER_NUM_2*FILTER_SIZE_2*FILTER_SIZE_2*sizeof(double));  
  double *grad_Wd = malloc(CLASS_NUM*FILTER_NUM_2*OUTPUT_DIM_2*OUTPUT_DIM_2*sizeof(double));
  double *grad_bc1 = malloc(FILTER_NUM_1*sizeof(double));
  double *grad_bc2 = malloc(FILTER_NUM_2*sizeof(double));
  double *grad_bd = malloc(CLASS_NUM*sizeof(double));  

  double *vel_Wc1 = malloc(FILTER_NUM_1*FILTER_SIZE_1*FILTER_SIZE_1*sizeof(double));
  double *vel_Wc2 = malloc(FILTER_NUM_1*FILTER_NUM_2*FILTER_SIZE_2*FILTER_SIZE_2*sizeof(double));  
  double *vel_Wd = malloc(CLASS_NUM*FILTER_NUM_2*OUTPUT_DIM_2*OUTPUT_DIM_2*sizeof(double));
  double *vel_bc1 = malloc(FILTER_NUM_1*sizeof(double));
  double *vel_bc2 = malloc(FILTER_NUM_2*sizeof(double));
  double *vel_bd = malloc(CLASS_NUM*sizeof(double));

  for (i = 0; i < FILTER_NUM_1*FILTER_SIZE_1*FILTER_SIZE_1; ++i) {
    Wc1[i] = 0.001*((double)rand() / (double)RAND_MAX - 0.5);
    // Wc1[i] = 0.001*RandGauss();
    // Wc1[i] = 0.001*i;
  }
  for (i = 0; i < FILTER_NUM_1*FILTER_NUM_2*FILTER_SIZE_2*FILTER_SIZE_2; ++i) {
    Wc2[i] = 0.001*RandGauss();
    // Wc2[i] = 0.001*((double)rand() / (double)RAND_MAX - 0.5);
    // Wc2[i] = 0.001*i;
  }  
  for (i = 0; i < CLASS_NUM*FILTER_NUM_2*OUTPUT_DIM_2*OUTPUT_DIM_2; ++i) {
    // Wd[i] = 0.001*RandGauss();
    Wd[i] = 0.001*((double)rand() / (double)RAND_MAX - 0.5);
    // Wd[i] = 0.001*i;
  }

  // memset(bc1, 0, FILTER_NUM_1*sizeof(double));
  // memset(bc2, 0, FILTER_NUM_2*sizeof(double));
  // memset(bd, 0, CLASS_NUM*sizeof(double));  
  Ones(bc1, FILTER_NUM_1);
  Ones(bc2, FILTER_NUM_2);
  Ones(bd, CLASS_NUM);

  // Training  
  int e = 0, s = 0, mb = 0;
    
  double *o1 = malloc(FILTER_NUM_1*CONV_DIM_1*CONV_DIM_1*sizeof(double)); 
  double *o1pooled = malloc(FILTER_NUM_1*OUTPUT_DIM_1*OUTPUT_DIM_1*sizeof(double));
  int *o1pool_idx = malloc(FILTER_NUM_1*OUTPUT_DIM_1*OUTPUT_DIM_1*sizeof(int));
  double *o2 = malloc(FILTER_NUM_2*CONV_DIM_2*CONV_DIM_2*sizeof(double));    
  double *o2pooled = malloc(FILTER_NUM_2*OUTPUT_DIM_2*OUTPUT_DIM_2*sizeof(double));
  int *o2pool_idx = malloc(FILTER_NUM_2*OUTPUT_DIM_2*OUTPUT_DIM_2*sizeof(int));
  double *o3 = malloc(CLASS_NUM*sizeof(double));
  
  double *ones_o1 = malloc(CONV_DIM_1*CONV_DIM_1*sizeof(double));
  double *ones_o2 = malloc(CONV_DIM_2*CONV_DIM_2*sizeof(double));
  double *ones_o3 = malloc(CLASS_NUM*sizeof(double));
  Ones(ones_o1, CONV_DIM_1*CONV_DIM_1);
  Ones(ones_o2, CONV_DIM_2*CONV_DIM_2);
  Ones(ones_o3, CLASS_NUM);

  double *delta_c1 = malloc(FILTER_NUM_1*CONV_DIM_1*CONV_DIM_1*sizeof(double));    
  double *delta_s1 = malloc(FILTER_NUM_1*OUTPUT_DIM_1*OUTPUT_DIM_1*sizeof(double));
  double *delta_c2 = malloc(FILTER_NUM_2*CONV_DIM_2*CONV_DIM_2*sizeof(double));    
  double *delta_s2 = malloc(FILTER_NUM_2*OUTPUT_DIM_2*OUTPUT_DIM_2*sizeof(double));
  double *delta_d = malloc(CLASS_NUM*sizeof(double));  

  double *delta_s1_col = malloc(CONV_DIM_2*CONV_DIM_2*FILTER_SIZE_2*FILTER_SIZE_2*sizeof(double));

  int *randperm = malloc(SAMPLE_SIZE*sizeof(int));

  double error = 0;

  time_t s_t = clock();
  for (e = 0; e < EPOCH; ++e) {    
    RandPerm(randperm, SAMPLE_SIZE);
 
    error = 0;
  
    for (s = 0; s < 51;) {   
      memset(grad_Wc1, 0, FILTER_NUM_1*FILTER_SIZE_1*FILTER_SIZE_1*sizeof(double));
      memset(grad_Wc2, 0, FILTER_NUM_1*FILTER_NUM_2*FILTER_SIZE_2*FILTER_SIZE_2*sizeof(double));
      memset(grad_Wd, 0, CLASS_NUM*FILTER_NUM_2*OUTPUT_DIM_2*OUTPUT_DIM_2*sizeof(double));
      memset(grad_bc1, 0, FILTER_NUM_1*sizeof(double));
      memset(grad_bc2, 0, FILTER_NUM_2*sizeof(double));
      memset(grad_bd, 0, CLASS_NUM*sizeof(double));    

      memset(vel_Wc1, 0, FILTER_NUM_1*FILTER_SIZE_1*FILTER_SIZE_1*sizeof(double));
      memset(vel_Wc2, 0, FILTER_NUM_1*FILTER_NUM_2*FILTER_SIZE_2*FILTER_SIZE_2*sizeof(double));
      memset(vel_Wd, 0, CLASS_NUM*FILTER_NUM_2*OUTPUT_DIM_2*OUTPUT_DIM_2*sizeof(double));
      memset(vel_bc1, 0, FILTER_NUM_1*sizeof(double));
      memset(vel_bc2, 0, FILTER_NUM_2*sizeof(double));
      memset(vel_bd, 0, CLASS_NUM*sizeof(double));    

      for (mb = 0; mb < MB_SIZE && s < SAMPLE_SIZE; ++mb) {  

        // Feed Forward            
        memset(o1, 0, CONV_DIM_1*CONV_DIM_1*FILTER_NUM_1*sizeof(double));  
        memset(o1pooled, 0, FILTER_NUM_1*OUTPUT_DIM_1*OUTPUT_DIM_1*sizeof(double));    
        memset(o2, 0, FILTER_NUM_2*CONV_DIM_2*CONV_DIM_2*sizeof(double));  
        memset(o2pooled, 0, FILTER_NUM_2*OUTPUT_DIM_2*OUTPUT_DIM_2*sizeof(double));    
        memset(o3, 0, CLASS_NUM*sizeof(double));
        
        Conv2_corr(data_im[s], Wc1, IM_SIZE, FILTER_SIZE_1, FILTER_NUM_1, 1, o1);           
       
        for (i = 0; i < FILTER_NUM_1; ++i) {        
          cblas_daxpy(CONV_DIM_1*CONV_DIM_1, bc1[i], ones_o1, 1, o1 + i*CONV_DIM_1*CONV_DIM_1, 1);  // add bias          
          MaxPool(o1 + i*CONV_DIM_1*CONV_DIM_1, CONV_DIM_1, POOL_DIM_1, 
                  o1pooled + i*OUTPUT_DIM_1*OUTPUT_DIM_1, o1pool_idx + i*OUTPUT_DIM_1*OUTPUT_DIM_1);      
          
          Conv2_corr(o1pooled + i*OUTPUT_DIM_1*OUTPUT_DIM_1, Wc2 + i*FILTER_NUM_2*FILTER_SIZE_2*FILTER_SIZE_2,  
                OUTPUT_DIM_1, FILTER_SIZE_2, FILTER_NUM_2, 1, o2);                
        }     
        
        for (i = 0; i < FILTER_NUM_2; ++i) {    
          cblas_daxpy(CONV_DIM_2*CONV_DIM_2, bc2[i], ones_o2, 1, o2 + i*CONV_DIM_2*CONV_DIM_2, 1);  // add bias
          MaxPool(o2 + i*CONV_DIM_2*CONV_DIM_2, CONV_DIM_2, POOL_DIM_2, 
                  o2pooled + i*OUTPUT_DIM_2*OUTPUT_DIM_2, o2pool_idx + i*OUTPUT_DIM_2*OUTPUT_DIM_2);            
        }
        
        cblas_dgemv(CblasRowMajor, CblasNoTrans, CLASS_NUM, OUTPUT_DIM_2*OUTPUT_DIM_2*FILTER_NUM_2, 1.0, 
                    Wd, OUTPUT_DIM_2*OUTPUT_DIM_2*FILTER_NUM_2, o2pooled, 1, 0.0, o3, 1);                
        
        // Back Propagation
        memset(delta_c1, 0, CONV_DIM_1*CONV_DIM_1*FILTER_NUM_1*sizeof(double));  
        memset(delta_s1, 0, FILTER_NUM_1*OUTPUT_DIM_1*OUTPUT_DIM_1*sizeof(double));    
        memset(delta_c2, 0, FILTER_NUM_2*CONV_DIM_2*CONV_DIM_2*sizeof(double));  
        memset(delta_s2, 0, FILTER_NUM_2*OUTPUT_DIM_2*OUTPUT_DIM_2*sizeof(double));    
        memset(delta_d, 0, CLASS_NUM*sizeof(double));
        
        // delta_d = o3 - y
        memcpy(delta_d, o3, CLASS_NUM*sizeof(double));       
        delta_d[labels[0]] -= 1;

        for (i = 0; i < CLASS_NUM; ++i) {
          error += delta_d[i] * delta_d[i];
        }

        // delta_s2
        cblas_dgemv(CblasRowMajor, CblasTrans, CLASS_NUM, OUTPUT_DIM_2*OUTPUT_DIM_2*FILTER_NUM_2, 1.0,
                    Wd, OUTPUT_DIM_2*OUTPUT_DIM_2*FILTER_NUM_2, delta_d, 1, 0.0, delta_s2, 1);

        // delta_c2
        for (i = 0; i < FILTER_NUM_2; ++i) {
          UpSample(delta_s2 + i*OUTPUT_DIM_2*OUTPUT_DIM_2, o2pool_idx + i*OUTPUT_DIM_2*OUTPUT_DIM_2,
                   OUTPUT_DIM_2, POOL_DIM_2, delta_c2 + i*CONV_DIM_2*CONV_DIM_2);
        }                       

        // delta_s1
        for (i = 0; i < FILTER_NUM_1; ++i) {
          memset(delta_s1_col, 0, CONV_DIM_2*CONV_DIM_2*FILTER_SIZE_2*FILTER_SIZE_2*sizeof(double));         
          cblas_dgemm(CblasRowMajor, CblasTrans, CblasNoTrans, 
                      FILTER_SIZE_2*FILTER_SIZE_2, CONV_DIM_2*CONV_DIM_2,  FILTER_NUM_2,
                      1, Wc2 + i*FILTER_NUM_2*FILTER_SIZE_2*FILTER_SIZE_2, 
                      FILTER_SIZE_2*FILTER_SIZE_2, delta_c2, CONV_DIM_2*CONV_DIM_2, 1, delta_s1_col, CONV_DIM_2*CONV_DIM_2);  
          Col2Im(delta_s1_col, OUTPUT_DIM_1, FILTER_SIZE_2, delta_s1 + i*OUTPUT_DIM_1*OUTPUT_DIM_1);        

          // delta_c1
          UpSample(delta_s1 + i*OUTPUT_DIM_1*OUTPUT_DIM_1, o1pool_idx + i*OUTPUT_DIM_1*OUTPUT_DIM_1,
                   OUTPUT_DIM_1, POOL_DIM_1, delta_c1 + i*CONV_DIM_1*CONV_DIM_1);
        }  

        // grad_Wd
        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
                    FILTER_NUM_2*OUTPUT_DIM_2*OUTPUT_DIM_2, CLASS_NUM, 1,
                    (double)1/MB_SIZE, o2pooled , 1, delta_d, CLASS_NUM, 1, grad_Wd, CLASS_NUM);  
                
        // grad_bd
        cblas_daxpy(CLASS_NUM, (double)1/MB_SIZE, delta_d, 1, grad_bd, 1);  
 
        // grad_Wc2
        for (i = 0; i < FILTER_NUM_1; ++i) {
          Conv2_corr(o1pooled + i*OUTPUT_DIM_1*OUTPUT_DIM_1, delta_c2,  
                     OUTPUT_DIM_1, CONV_DIM_2, FILTER_NUM_2, (double)1/MB_SIZE, grad_Wc2 + i*FILTER_NUM_2*FILTER_SIZE_2*FILTER_SIZE_2);             
        }

        // grad_bc2
        for (i = 0; i < FILTER_NUM_2; ++i) {
          cblas_dgemv(CblasRowMajor, CblasNoTrans, 1, CONV_DIM_2*CONV_DIM_2, (double)1/MB_SIZE, 
                      delta_c2 + i*CONV_DIM_2*CONV_DIM_2, CONV_DIM_2*CONV_DIM_2, ones_o2, 1, 1, &grad_bc2[i], 1);          
        }

        // grad_Wc1
        Conv2_corr(data_im[s], delta_c1,  
                   IM_SIZE, CONV_DIM_1, FILTER_NUM_1, (double)1/MB_SIZE, grad_Wc1);     
 
        // grad_bc1
        for (i = 0; i < FILTER_NUM_1; ++i) {
          cblas_dgemv(CblasRowMajor, CblasNoTrans, 1, CONV_DIM_1*CONV_DIM_1, (double)1/MB_SIZE, 
                      delta_c1 + i*CONV_DIM_1*CONV_DIM_1, CONV_DIM_1*CONV_DIM_1, ones_o1, 1, 1, &grad_bc1[i], 1);          
        }                    
              
        ++s; // next sample data
      }     

      // update weights 
      cblas_daxpy(FILTER_NUM_1*FILTER_SIZE_1*FILTER_SIZE_1, (MOM - 1), vel_Wc1, 1, vel_Wc1, 1);
      cblas_daxpy(FILTER_NUM_1*FILTER_SIZE_1*FILTER_SIZE_1, -R, grad_Wc1, 1, vel_Wc1, 1);
      cblas_daxpy(FILTER_NUM_2*FILTER_SIZE_2*FILTER_SIZE_2, (MOM - 1), vel_Wc2, 1, vel_Wc2, 1);
      cblas_daxpy(FILTER_NUM_2*FILTER_SIZE_2*FILTER_SIZE_2, -R, grad_Wc2, 1, vel_Wc2, 1);
      cblas_daxpy(CLASS_NUM*FILTER_NUM_2*OUTPUT_DIM_2*OUTPUT_DIM_2, (MOM - 1), vel_Wd, 1, vel_Wd, 1);
      cblas_daxpy(CLASS_NUM*FILTER_NUM_2*OUTPUT_DIM_2*OUTPUT_DIM_2, -R, grad_Wd, 1, vel_Wd, 1);
      cblas_daxpy(FILTER_NUM_1, (MOM - 1), vel_bc1, 1, vel_bc1, 1);
      cblas_daxpy(FILTER_NUM_1, -R, grad_bc1, 1, vel_bc1, 1);
      cblas_daxpy(FILTER_NUM_2, (MOM - 1), vel_bc2, 1, vel_bc2, 1);
      cblas_daxpy(FILTER_NUM_2, -R, grad_bc2, 1, vel_bc2, 1);
      cblas_daxpy(CLASS_NUM, (MOM - 1), vel_bd, 1, vel_bd, 1);
      cblas_daxpy(CLASS_NUM, -R, grad_bd, 1, vel_bd, 1);

      cblas_daxpy(FILTER_NUM_1*FILTER_SIZE_1*FILTER_SIZE_1, 1, vel_Wc1, 1, Wc1, 1);    
      cblas_daxpy(FILTER_NUM_2*FILTER_SIZE_2*FILTER_SIZE_2, 1, vel_Wc2, 1, Wc2, 1);
      cblas_daxpy(CLASS_NUM*FILTER_NUM_2*OUTPUT_DIM_2*OUTPUT_DIM_2, 1, vel_Wd, 1, Wd, 1);    
      cblas_daxpy(FILTER_NUM_1, 1, vel_bc1, 1, bc1, 1);      
      cblas_daxpy(FILTER_NUM_2, 1, vel_bc2, 1, bc2, 1);      
      cblas_daxpy(CLASS_NUM, 1, vel_bd, 1, bd, 1);          

      
    }
    printf("%lf\n", error);   
  }

  time_t e_t = clock();

  printf("%lf\n", (double) (e_t-s_t)/CLOCKS_PER_SEC);

  free(o1);
  free(o1pooled);
  free(o1pool_idx);
  free(o2);
  free(o2pooled);
  free(o2pool_idx);
  free(o3);

  free(Wc1);
  free(Wc2);
  free(Wd);
  free(bc1);
  free(bc2);
  free(bd);

  free(grad_Wc1);
  free(grad_Wc2);
  free(grad_Wd);
  free(grad_bc1);
  free(grad_bc2);
  free(grad_bd);

  free(vel_Wc1);
  free(vel_Wc2);
  free(vel_Wd);
  free(vel_bc1);
  free(vel_bc2);
  free(vel_bd);

  free(delta_c1);
  free(delta_s1);
  free(delta_c2);
  free(delta_s2);
  free(delta_d);
  free(delta_s1_col);

  free(randperm);
  free(labels);
     
  for (i = 0; i < SAMPLE_SIZE; ++i) {
    free(data_im[i]);    
  }
  free(data_im);

  return 0;
}
