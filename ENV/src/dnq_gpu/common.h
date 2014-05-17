#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <fstream>

#include <vector>
#include <string>

#include <cuda.h>
#include <cublas_v2.h>
#include <cuda_runtime.h>
#include <curand.h>

#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cstdlib>
#include <cfloat> 

#include <cblas.h>

using namespace std;

typedef std::vector<std::string> FileNames;
typedef unsigned char uInt8;

#define CUDA_KERNEL_LOOP(i, n) \
   for (int i = blockIdx.x * blockDim.x + threadIdx.x; \
        i < (n); \
        i += blockDim.x * gridDim.x)
#endif

