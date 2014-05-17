#include "common.h"

bool IsNan(float *a, int size);
void PrintCUDA(float *a, int size);
void im2col_gpu(const float* data_im, const int channels,
                const int height, const int width, const int ksize, const int pad,
                const int stride, float* data_col);
void col2im_gpu(const float* data_col, const int channels,
                const int height, const int width, const int ksize, const int pad,
                const int stride, float* data_im);

void Rect(float *x, int size);
void RectDiff(float *output, float *delta, int size);
void AddElement(float *x, int size, int idx, float y);
void Constants(float *x, int size, float value);
void Zeros(float *x, int size);
void RandUniform(float *x, int size, float value, curandGenerator_t gen);
void RandGauss(float *x, int size, float value, curandGenerator_t gen, cublasHandle_t handle); 
void MaxOutput(float *x, int size, float *value, int *idx, cublasHandle_t handle);
