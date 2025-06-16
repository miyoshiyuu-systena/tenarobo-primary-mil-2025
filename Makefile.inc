USE_RASPIKE_ART=1

mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

APPL_CXXOBJS +=\
PerceptionTask.o\
Logger.o\
TwinWheelDrive.o\
ActionChain.o\
PerceptionDataAccess.o\
ModuleAccess.o\

SRCLANG := c++

ifdef CONFIG_EV3RT_APPLICATION

# Include libraries
include $(EV3RT_SDK_LIB_DIR)/libcpp-spike/Makefile

endif

APPL_DIRS += \
	$(mkfile_path)task \
	$(mkfile_path)logging \
	$(mkfile_path)organ \
	$(mkfile_path)action-chain \
	$(mkfile_path)share \

INCLUDES += \
	-I$(mkfile_path)task\
	-I$(mkfile_path)logging\
	-I$(mkfile_path)organ\
	-I$(mkfile_path)action-chain\
	-I$(mkfile_path)share\

COPTS += -std=gnu++11