MODULE := src/dnq_gpu 

MODULE_OBJS := \
	src/dnq_gpu/nn.o \
	src/dnq_gpu/layer.o \
	src/dnq_gpu/math_functions.o \

	
	
MODULE_DIRS += \
	src/dnq_gpu

# Include common rules 
include $(srcdir)/common.rules
