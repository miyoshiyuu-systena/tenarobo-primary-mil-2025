USE_RASPIKE_ART=1

mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

APPL_COBJS += \

APPL_LIBS += -lm

APPL_CXXOBJS +=\
DrivingTask.o\
PerceptionTask.o\
Clock.o\

SRCLANG := c++

APPL_DIRS += $(mkfile_path)task

INCLUDES += \
	-I$(mkfile_path)task\

COPTS += -std=gnu++11