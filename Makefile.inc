USE_RASPIKE_ART=1

mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

APPL_COBJS += \

APPL_LIBS += -lm

APPL_CXXOBJS +=\
LineTracer.o\

SRCLANG := c++

APPL_DIRS += $(mkfile_path)LineTracer

INCLUDES += \
	-I$(mkfile_path)LineTracer\

COPTS += -std=gnu++11