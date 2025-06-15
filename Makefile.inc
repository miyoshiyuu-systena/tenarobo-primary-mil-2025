USE_RASPIKE_ART=1

mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

APPL_CXXOBJS +=\
	DrivingTask.o\
	PerceptionTask.o\
	logging.o\

SRCLANG := c++

ifdef CONFIG_EV3RT_APPLICATION

# Include libraries
include $(EV3RT_SDK_LIB_DIR)/libcpp-spike/Makefile

endif

APPL_DIRS +=\
	$(mkfile_path)task\
	$(mkfile_path)utils\

INCLUDES += \
	-I$(mkfile_path)task\
	-I$(mkfile_path)utils\

COPTS += -std=gnu++11

# Boostライブラリのルートディレクトリを指定 (例: /usr/local/boost_1_84_0)
BOOST_ROOT = /path/to/your/boost_installation

# Boostヘッダファイルのパス
INCLUDES += -I$(BOOST_ROOT)

# Boost.Filesystemライブラリへのリンク (OSによってライブラリ名が異なる場合があります)
# Linuxの場合:
LDFLAGS += -L$(BOOST_ROOT)/lib -lboost_filesystem -lboost_system

# Windowsの場合 (MinGW-w64など):
# LDFLAGS += -L$(BOOST_ROOT)/lib -lboost_filesystem-mt -lboost_system-mt # or similar depending on your boost build