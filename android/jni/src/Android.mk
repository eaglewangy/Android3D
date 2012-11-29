LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := android3d_jni

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../include \
                    $(LOCAL_PATH)/../../../src/3rdParty/glm

LOCAL_CFLAGS := -DANDROID_NDK

LOCAL_SRC_FILES := android3d_jni.cpp

LOCAL_STATIC_LIBRARIES := android3d

LOCAL_LDLIBS :=  -llog -lGLESv2 -landroid -lEGL

include $(BUILD_SHARED_LIBRARY)
