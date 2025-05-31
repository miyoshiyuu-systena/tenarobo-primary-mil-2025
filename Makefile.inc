USE_RASPIKE_ART=1

mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

APPL_CXXOBJS +=\
DrivingTask.o\
PerceptionTask.o\

SRCLANG := c++

ifdef CONFIG_EV3RT_APPLICATION

# Include libraries
include $(EV3RT_SDK_LIB_DIR)/libcpp-spike/Makefile

endif

APPL_DIRS += $(mkfile_path)task

INCLUDES += \
	-I$(mkfile_path)task\

COPTS += -std=gnu++11