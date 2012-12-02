LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libjpeg

LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,, \
	$(wildcard $(LOCAL_PATH)/../../../../src/3rdParty/libjpeg/*.c))

#$(info $(LOCAL_SRC_FILES)) 

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)