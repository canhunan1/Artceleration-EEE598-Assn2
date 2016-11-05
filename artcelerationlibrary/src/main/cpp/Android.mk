LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES := native-lib.cpp
LOCAL_MODULE := edu_asu_msrs_artcelerationlibrary_TransformService_native-lib
include $(BUILD_SHARED_LIBRARY)