LOCAL_PATH := $(call my-dir)
MYPATH := $(LOCAL_PATH)

include $(CLEAR_VARS)

# Build NXNA...
NXNA_PATH := ../../../src
include ../../../src/Android.mk

# Build SDL
LOCAL_PATH := $(MYPATH)
include ../../../lib/SDL_13/Android.mk

LOCAL_PATH := $(MYPATH)
LOCAL_MODULE := main
LOCAL_MODULE_FILENAME := main
SDL_PATH := ../SDL

LOCAL_C_INCLUDES := $(NXNA_PATH)

# Add your application source files here...
LOCAL_SRC_FILES := ../../../Game1.cpp

LOCAL_STATIC_LIBRARIES := nxna 
LOCAL_SHARED_LIBRARIES := SDL2

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
