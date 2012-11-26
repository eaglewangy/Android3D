LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libpng

LOCAL_LDLIBS := -lz
LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,, \
	$(wildcard $(LOCAL_PATH)/../../../src/3rdParty/libpng/*.c))

$(info $(LOCAL_SRC_FILES)) 

include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_STATIC_LIBRARY)