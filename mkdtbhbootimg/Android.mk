LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := mkbootimg.c
LOCAL_STATIC_LIBRARIES := libfdt libmincrypt
LOCAL_C_INCLUDES := $(LOCAL_PATH)/libfdt

#LOCAL_MODULE := mkdtbhbootimg
LOCAL_MODULE := dtbhTool

include $(BUILD_HOST_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := mkbootimg.c
LOCAL_STATIC_LIBRARIES := libfdt libmincrypt libcutils libc
LOCAL_C_INCLUDES := $(LOCAL_PATH)/libfdt
#LOCAL_MODULE := utility_mkdtbhbootimg
LOCAL_MODULE := utility_dtbhTool
LOCAL_MODULE_TAGS := eng
#LOCAL_MODULE_STEM := mkdtbhbootimg
LOCAL_MODULE_STEM := dtbhTool
LOCAL_MODULE_CLASS := UTILITY_EXECUTABLES
LOCAL_UNSTRIPPED_PATH := $(PRODUCT_OUT)/symbols/utilities
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/utilities
LOCAL_FORCE_STATIC_EXECUTABLE := true
include $(BUILD_EXECUTABLE)

$(call dist-for-goals,dist_files,$(LOCAL_BUILT_MODULE))

include $(LOCAL_PATH)/libfdt/Android.mk
