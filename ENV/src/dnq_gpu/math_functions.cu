#include "math_functions.h"

bool IsNan(float *a, int size) {
  bool is_nan = false;

  float *b = (float*) malloc(size*sizeof(float));
  cudaMemcpy(b, a, size*sizeof(float), cudaMemcpyDeviceToHost);

  for (int i = 0; i < size; ++i)
    if (b[i] != b[i])
      is_nan = true;

  return is_nan;
}

void PrintCUDA(float *a, int size) {
  float *b = (float*) malloc(size*sizeof(float));
  cudaMemcpy(b, a, size*sizeof(float), cudaMemcpyDeviceToHost);
  for (int i = 0; i < size; ++i)
    cout << b[i] << " ";
  free(b);
}

__global__ void im2col_gpu_kernel_caffe(const int n, const float* data_im,
                const int height, const int width, const int ksize, const int pad,
                const int stride, const int height_col, const int width_col,
          float* data_col) {
CUDA_KERNEL_LOOP(index, n) {
    int w_out = index % width_col;
    index /= width_col;
    int h_out = index % height_col;
    int channel_in = index / height_col;
    int channel_out = channel_in * ksize * ksize;
    int h_in = h_out * stride - pad;
    int w_in = w_out * stride - pad;
    data_col += (channel_out * height_col + h_out) * width_col + w_out;
    data_im += (channel_in * height + h_in) * width + w_in;
    for (int i = 0; i < ksize; ++i) {
      for (int j = 0; j < ksize; ++j) {
        int h = h_in + i;
        int w = w_in + j;
        *data_col = (h >= 0 && w >= 0 && h < height && w < width) ?
        data_im[i * width + j] : 0;
        data_col += height_col * width_col;
      }
    }
  }
}

void im2col_gpu(const float* data_im, const int channels,
    const int height, const int width, const int ksize, const int pad,
        const int stride, float* data_col) {
    // We are going to launch channels * height_col * width_col kernels, each
      // kernel responsible for copying a single-channel grid.
  int height_col = (height + 2 * pad - ksize) / stride + 1;
  int width_col = (width + 2 * pad - ksize) / stride + 1;
  int num_kernels = channels * height_col * width_col;
  im2col_gpu_kernel_caffe<<<(num_kernels+255)/256, 256>>>(num_kernels, data_im, height, width, ksize, pad, stride, height_col,
                width_col, data_col);
}

__global__ void col2im_gpu_kernel(const int n, const float* data_col,
                                  const int height, const int width, const int channels, const int ksize,
                                  const int pad, const int stride, const int height_col, const int width_col,
                                  float* data_im) {
  CUDA_KERNEL_LOOP(index, n) {
    float val = 0;
    int w = index % width + pad;
    int h = (index / width) % height + pad;
    int c = index / (width * height);
    // compute the start and end of the output
    int w_col_start = (w < ksize) ? 0 : (w - ksize) / stride + 1;
    int w_col_end = min(w / stride + 1, width_col);
    int h_col_start = (h < ksize) ? 0 : (h - ksize) / stride + 1;
    int h_col_end = min(h / stride + 1, height_col);
                      // equivalent implementation
    int offset = (c * ksize * ksize + h * ksize + w) * height_col * width_col;
    int coeff_h_col = (1 - stride * ksize * height_col) * width_col;
    int coeff_w_col = (1 - stride * height_col * width_col);
    for (int h_col = h_col_start; h_col < h_col_end; ++h_col) {
      for (int w_col = w_col_start; w_col < w_col_end; ++w_col) {
        val += data_col[offset + h_col * coeff_h_col + w_col * coeff_w_col];
      }
    }
    data_im[index] = val;
  }
}

void col2im_gpu(const float* data_col, const int channels,
                const int height, const int width, const int ksize, const int pad,
                const int stride, float* data_im) {
  int height_col = (height + 2 * pad - ksize) / stride + 1;
  int width_col = (width + 2 * pad - ksize) / stride + 1;
  int num_kernels = channels * height * width;
  col2im_gpu_kernel<<<(num_kernels+255)/256, 256>>>(
  num_kernels, data_col, height, width, channels, ksize, pad, stride,
  height_col, width_col, data_im);
}

__global__ void RectKernel(float *x, int size) {
  int i = blockIdx.x*blockDim.x + threadIdx.x;
  if (i < size) {
    x[i] = (x[i] > 0) ? x[i]:0;
  }
}

void Rect(float *x, int size) {
  RectKernel<<<(size+255)/256, 256>>>(x, size);
}

__global__ void RectDiffKernel(float *output, float *delta, int size) {
  int i = blockIdx.x*blockDim.x + threadIdx.x;
  if (i < size) {
    if (output[i] < 0.0) {
      delta[i] = 0.0;
    }
  }
}

void RectDiff(float *output, float *delta, int size) {
  RectDiffKernel<<<(size+255)/256, 256>>>(output, delta, size);
}

__global__ void ConstantsKernel(float *x, int size, float value) {
  int i = blockIdx.x*blockDim.x + threadIdx.x;
  if (i < size) {
    x[i] = value;
  }
}

void Constants(float *x, int size, float value) {
  ConstantsKernel<<<(size+255)/256, 256>>>(x, size, value);
}

__global__ void AddElementKernel(float *x, int idx, float y) {
  int i = blockIdx.x*blockDim.x + threadIdx.x;
  if (i == idx) {
    x[i] += y;    
  }
}

void AddElement(float *x, int size, int idx, float y) {
  AddElementKernel<<<(size + 255)/256, 256>>>(x, idx, y);
}

void MaxOutput(float *x, int size, float *value, int *idx, cublasHandle_t handle) {
  // index of cublasIsamax starts from 1 not 0
  cublasIsamax(handle, size, x, 1, idx);
   --(*idx);
  cudaMemcpy(value, &x[*idx], sizeof(float), cudaMemcpyDeviceToHost); 
}

void Zeros(float *x, int n) {  
  cudaMemset(x, 0, n*sizeof(float));
}

void RandUniform(float *x, int size, float value, curandGenerator_t gen) {
  curandGenerateUniform(gen, x, size);
}

void RandGauss(float *x, int size, float value, curandGenerator_t gen, cublasHandle_t handle) {    
  curandGenerateNormal(gen, x, size, 0, 1);
  cublasSscal(handle, size, &value, x, 1);
}