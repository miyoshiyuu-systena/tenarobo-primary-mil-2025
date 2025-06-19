USE_RASPIKE_ART=1

mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

APPL_CXXOBJS +=\
	IAssist.o\
	CalcBlackWhiteBorderError.o\
	CalcBlueWhiteBorderError.o\
	LaneTracingAssist.o\
	ICloser.o\
	BlackFloorCloser.o\
	BlueFloorCloser.o\
	TimedCloser.o\
	GoStraightAction.o\
	HachikouAction.o\
	ActionNode.o\
	Device.o\
	CalcCurveDriveSpeed.o\
	FrontArmDrive.o\
	TwinWheelDrive.o\
	Logger.o\
	Perception.o\
	CameraManager.o\

SRCLANG := c++

ifdef CONFIG_EV3RT_APPLICATION

# Include libraries
include $(EV3RT_SDK_LIB_DIR)/libcpp-spike/Makefile

endif

APPL_DIRS += \
	$(mkfile_path)action-chain \
	$(mkfile_path)action-chain/action \
	$(mkfile_path)action-chain/action/assist \
	$(mkfile_path)action-chain/action/closer \
	$(mkfile_path)device \
	$(mkfile_path)drive \
	$(mkfile_path)logger \
	$(mkfile_path)perception \
	$(mkfile_path)web-camera \

INCLUDES += \
	-I$(mkfile_path)action-chain \
	-I$(mkfile_path)action-chain/action \
	-I$(mkfile_path)action-chain/action/assist \
	-I$(mkfile_path)action-chain/action/closer \
	-I$(mkfile_path)device \
	-I$(mkfile_path)drive \
	-I$(mkfile_path)logger \
	-I$(mkfile_path)perception \
	-I$(mkfile_path)web-camera \
	-I/usr/include/opencv4\
	-I/usr/include/opencv4/opencv2\

COPTS += -std=gnu++11

# OpenCVがV4L2を使用するように環境変数を設定
export OPENCV_VIDEOIO_PRIORITY_V4L2=1
export OPENCV_VIDEOIO_DEBUG=1
export OPENCV_VIDEOIO_GSTREAMER=0

# OpenCVライブラリの追加
LIBS += -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio