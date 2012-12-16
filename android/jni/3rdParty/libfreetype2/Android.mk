LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

FREETYPE_SRC_PATH := $(LOCAL_PATH)/../../../../src/3rdParty/freetype2-android/

LOCAL_MODULE := libfreetype2

LOCAL_CFLAGS := -DANDROID_NDK \
		-DFT2_BUILD_LIBRARY=1 \
		-include $(FREETYPE_SRC_PATH)include/ft2build.h \
		-DAF_SCRIPT_LATIN2

LOCAL_C_INCLUDES := $(FREETYPE_SRC_PATH)include $(FREETYPE_SRC_PATH)src

#$(info $(LOCAL_C_INCLUDES)) 

LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,, \
	$(wildcard $(FREETYPE_SRC_PATH)src/*/*.c))

LOCAL_LDLIBS := -ldl -llog

include $(BUILD_STATIC_LIBRARY)
