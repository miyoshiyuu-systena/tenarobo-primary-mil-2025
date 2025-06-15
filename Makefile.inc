USE_RASPIKE_ART=1

mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

APPL_CXXOBJS +=\
	DrivingTask.o\
	PerceptionTask.o\
	logging.o\

SRCLANG := c++

ifdef CONFIG_EV3RT_APPLICATION

# Include libraries
include $(EV3RT_SDK_LIB_DIR)/libcpp-spike/Makefile\
include $(mkfile_path)package/spdlog/build/Makfile\

endif

APPL_DIRS +=\
	$(mkfile_path)task\
	$(mkfile_path)utils\

INCLUDES += \
	-I$(mkfile_path)task\
	-I$(mkfile_path)utils\

COPTS += -std=gnu++11