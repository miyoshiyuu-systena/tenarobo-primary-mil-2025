USE_RASPIKE_ART=1

mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

APPL_CXXOBJS +=\
	DrivingTask.o\
	PerceptionTask.o\
	Logger.o\

SRCLANG := c++

ifdef CONFIG_EV3RT_APPLICATION

# Include libraries
include $(EV3RT_SDK_LIB_DIR)/libcpp-spike/Makefile\

endif

APPL_DIRS +=\
	$(mkfile_path)package/spdlog/include\
	$(mkfile_path)task\
	$(mkfile_path)utils\

INCLUDES += \
	-I$(mkfile_path)package/spdlog/include\
	-I$(mkfile_path)task\
	-I$(mkfile_path)utils\

COPTS += -std=gnu++11

# --- ここから Spdlog のリンク設定を追加 (重要!) ---

# Spdlogライブラリが生成されたディレクトリのパスを指定
# (通常は spdlog の build ディレクトリ)
SPDLOG_LIB_DIR := $(mkfile_path)package/spdlog/build

# リンカオプションにライブラリの検索パスを追加
# -Lオプションで、リンカがライブラリファイルを探すディレクトリを指定します
LDFLAGS += -L$(SPDLOG_LIB_DIR)

# リンクするライブラリの名前を追加
# -lspdlog は libspdlog.a (または libspdlog.so) をリンクするという意味です。
# Spdlogはスレッドセーフな実装であるため、多くの場合 -lpthread (POSIX Threads) も必要になります。
LDLIBS += -lspdlog -lpthread

# --- ここまで追加 ---