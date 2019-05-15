LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := memtool

LOCAL_SRC_FILES := MemManipulator.c

LOCAL_CC := /usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-gcc

LOCAL_LDLIBS := -llog

include $(BUILD_EXECUTABLE)

