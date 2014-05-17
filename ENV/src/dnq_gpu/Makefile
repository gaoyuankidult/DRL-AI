NVCC = nvcc
INCLUDE = -arch=sm_20 -c -Xcompiler -fPIC -lcublas

all:
	$(NVCC) $(INCLUDE) math_functions.cu layer.cu nn.cu

clean: rm *.o	
