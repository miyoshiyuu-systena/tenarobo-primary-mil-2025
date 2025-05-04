USE_RASPIKE_ART=1

mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

APPL_CXXOBJS += \
LineTracer.o

APPL_LIBS += -lm -lstdc++

APPL_DIRS += $(mkfile_path)LineTracer

INCLUDES += \
    -I$(mkfile_path)LineTracer\