LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := EvoAI
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libEvoAI-1.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := JsonBox

prebuilt_path := $(call local-prebuilt-path,$(LOCAL_SRC_FILES))
prebuilt := $(strip $(wildcard $(prebuilt_path)))

ifdef prebuilt
    include $(PREBUILT_SHARED_LIBRARY)
endif

include $(CLEAR_VARS)

LOCAL_MODULE := EvoAI-d
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libEvoAI-1-d.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := JsonBox_d

prebuilt_path := $(call local-prebuilt-path,$(LOCAL_SRC_FILES))
prebuilt := $(strip $(wildcard $(prebuilt_path)))

ifdef prebuilt
    include $(PREBUILT_SHARED_LIBRARY)
endif

include $(CLEAR_VARS)

LOCAL_MODULE := EvoAI-static
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libEvoAI-1.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := JsonBox-static

prebuilt_path := $(call local-prebuilt-path,$(LOCAL_SRC_FILES))
prebuilt := $(strip $(wildcard $(prebuilt_path)))

ifdef prebuilt
    include $(PREBUILT_STATIC_LIBRARY)
endif

include $(CLEAR_VARS)

LOCAL_MODULE := EvoAI-d-static
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libEvoAI-1-d.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := JsonBox_d-static

prebuilt_path := $(call local-prebuilt-path,$(LOCAL_SRC_FILES))
prebuilt := $(strip $(wildcard $(prebuilt_path)))

ifdef prebuilt
    include $(PREBUILT_STATIC_LIBRARY)
endif
