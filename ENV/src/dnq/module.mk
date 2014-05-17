MODULE := src/dnq 

MODULE_OBJS := \
	src/dnq/nn.o \
	src/dnq/layer.o \
	
	
MODULE_DIRS += \
	src/dnq

# Include common rules 
include $(srcdir)/common.rules
