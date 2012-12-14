LOCAL_PATH := $(call my-dir)
###########################
#
# android3d static library
#
###########################

include $(CLEAR_VARS)

#$(info "building android3d library...")
#$(info $(LOCAL_SRC_FILES))

LOCAL_MODULE := libandroid3d

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../include \
                    $(LOCAL_PATH)/../../../src/3rdParty/glm \
                    $(LOCAL_PATH)/../../../src/3rdParty/libpng \
                    $(LOCAL_PATH)/../../../src/3rdParty/libjpeg \

$(info $(LOCAL_PATH))
LOCAL_STATIC_LIBRARIES := png jpeg
LOCAL_LDLIBS    := -llog -lGLESv2 -landroid -lEGL -lz
LOCAL_CFLAGS := -DANDROID_NDK -Werror 
LOCAL_SRC_FILES := \
        $(subst $(LOCAL_PATH)/,, \
        $(wildcard $(LOCAL_PATH)/../../../src/*.cpp)) 
LOCAL_SRC_FILES += $(subst $(LOCAL_PATH)/,, \
        $(wildcard $(LOCAL_PATH)/../../../src/math/*.cpp))

include $(BUILD_STATIC_LIBRARY)

