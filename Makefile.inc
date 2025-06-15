USE_RASPIKE_ART=1

mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

APPL_CXXOBJS +=\
PerceptionTask.o\
Logger.o\

SRCLANG := c++

ifdef CONFIG_EV3RT_APPLICATION

# Include libraries
include $(EV3RT_SDK_LIB_DIR)/libcpp-spike/Makefile

endif

APPL_DIRS += $(mkfile_path)task $(mkfile_path)logging

INCLUDES += \
	-I$(mkfile_path)task\
	-I$(mkfile_path)logging\

COPTS += -std=gnu++11