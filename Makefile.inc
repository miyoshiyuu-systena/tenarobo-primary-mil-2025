USE_RASPIKE_ART=1

mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

APPL_CXXOBJS +=\
PerceptionTask.o\
Logger.o\
TwinWheelDrive.o\
TwinWheelLogic.o\
ActionNode.o\
PerceptionDataAccess.o\
ModuleAccess.o\
RunUntilWallDetectAction.o\
Turn180Action.o\
AroundBottleEdgeAction.o\
StartOnPressureSensorAction.o\
GenerateInfinityWanderAroundAction.o\
LineTraceAction.o\
LineTraceTimedAction.o\
CircleAction.o\
TurnAction.o\
StraightGoAction.o\
CalcPidControl.o\
CalcErrorOnBlackWhiteBorder.o\
CalcErrorOnBlueWhiteBorder.o\
IsOnBlack.o\
IsOnBlue.o\
CameraManager.o\

SRCLANG := c++

ifdef CONFIG_EV3RT_APPLICATION

# Include libraries
include $(EV3RT_SDK_LIB_DIR)/libcpp-spike/Makefile

endif

APPL_DIRS += \
	$(mkfile_path)task \
	$(mkfile_path)logging \
	$(mkfile_path)organ \
	$(mkfile_path)action \
	$(mkfile_path)share \
	$(mkfile_path)logic \
	$(mkfile_path)judge \
	$(mkfile_path)webcamera \

INCLUDES += \
	-I$(mkfile_path)task\
	-I$(mkfile_path)logging\
	-I$(mkfile_path)organ\
	-I$(mkfile_path)action\
	-I$(mkfile_path)share\
	-I$(mkfile_path)logic\
	-I$(mkfile_path)judge\
	-I$(mkfile_path)webcamera\
	-I/usr/include/opencv4\
	-I/usr/include/opencv4/opencv2\

COPTS += -std=gnu++11

# OpenCVがV4L2を使用するように環境変数を設定
export OPENCV_VIDEOIO_PRIORITY_V4L2=1
export OPENCV_VIDEOIO_DEBUG=1
export OPENCV_VIDEOIO_GSTREAMER=0

# OpenCVライブラリの追加
LIBS += -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio