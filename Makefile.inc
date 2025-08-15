USE_RASPIKE_ART=1

mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

APPL_CXXOBJS +=\
	IAssist.o\
	CalcBlackWhiteBorderError.o\
	CalcBlueWhiteBorderError.o\
	LaneTracingAssist.o\
	SlowlyAccelerateAssist.o\
	ICloser.o\
	BlackFloorCloser.o\
	BlueFloorCloser.o\
	CurveCloser.o\
	StraightCloser.o\
	OnRightEdgeCloser.o\
	OnLeftEdgeCloser.o\
	TimedCloser.o\
	GoStraightAction.o\
	GoCurveAction.o\
	PivotTurnAction.o\
	SpinTurnAction.o\
	HachikouAction.o\
	StopAction.o\
	LaneChangeAction.o\
	LaneChangeCompleteJudge.o\
	SinpleLaneChangeAction.o\
	ActionNode.o\
	Device.o\
	CalcCurveDriveSpeed.o\
	FrontArmDrive.o\
	TwinWheelDrive.o\
	Logger.o\
	config.o\
	ImageAnalysisServer.o\

SRCLANG := c++

ifdef CONFIG_EV3RT_APPLICATION

# Include libraries
include $(EV3RT_SDK_LIB_DIR)/libcpp-spike/Makefile

endif

APPL_DIRS += \
	$(mkfile_path)action \
	$(mkfile_path)assist \
	$(mkfile_path)closer \
	$(mkfile_path)device \
	$(mkfile_path)drive \
	$(mkfile_path)logger \
	$(mkfile_path)web-camera \

INCLUDES += \
	-I$(mkfile_path)action \
	-I$(mkfile_path)assist \
	-I$(mkfile_path)closer \
	-I$(mkfile_path)device \
	-I$(mkfile_path)drive \
	-I$(mkfile_path)logger \
	-I$(mkfile_path)web-camera \

COPTS += -std=gnu++11
