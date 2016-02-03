USE_CAMERA_STUB := true

# inherit from the proprietary version
-include vendor/samsung/slte/BoardConfigVendor.mk

LOCAL_PATH := device/samsung/slte

TARGET_ARCH := arm
TARGET_NO_BOOTLOADER := true

# Platform
TARGET_BOARD_PLATFORM := exynos5
TARGET_SLSI_VARIANT := cm
TARGET_SOC := exynos5430

# CPU
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_VARIANT := cortex-a15
TARGET_CPU_SMP := true
ARCH_ARM_HAVE_TLS_REGISTER := true

# RENDERSCRIPT
BOARD_OVERRIDE_RS_CPU_VARIANT_32 := cortex-a15
#OVERRIDE_RS_DRIVER := libRSDriverArm.so

# Enable dex-preoptimization to speed up first boot sequence
ifeq ($(HOST_OS),linux)
  ifeq ($(TARGET_BUILD_VARIANT),user)
    ifeq ($(WITH_DEXPREOPT),)
      WITH_DEXPREOPT := true
    endif
  endif
endif
WITH_DEXPREOPT_BOOT_IMG_ONLY ?= true

# BOOT
TARGET_BOOTLOADER_BOARD_NAME := universal5430

BOARD_KERNEL_BASE := 0x10000000
BOARD_KERNEL_PAGESIZE := 2048
#BOARD_KERNEL_CMDLINE := The bootloader ignores the cmdline from the boot.img
#BOARD_KERNEL_SEPARATED_DT := true
# Extracted with libbootimg
BOARD_MKBOOTIMG_ARGS := --ramdisk_offset 0x01000000 --tags_offset 0x00000100 --dt device/samsung/slte/dtb.img

# fix this up by examining /proc/mtd on a running device
BOARD_BOOTIMAGE_PARTITION_SIZE := 0x00a00000
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 0x00c00000
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 2401239040
BOARD_USERDATAIMAGE_PARTITION_SIZE := 28219277312
BOARD_FLASH_BLOCK_SIZE := 131072

#TARGET_PREBUILT_KERNEL := device/samsung/slte/kernel
TARGET_KERNEL_CONFIG := cm_exynos5430-slte_defconfig
TARGET_KERNEL_SOURCE := kernel/samsung/slte
# hardware/samsung_slsi/exynos/libhdmi_legacy
TARGET_LINUX_KERNEL_VERSION := 3.10

BOARD_HAS_NO_SELECT_BUTTON := true

# Use these flags if the board has a ext4 partition larger than 2gb
BOARD_HAS_LARGE_FILESYSTEM := true
TARGET_USERIMAGES_USE_EXT4 := true

### INCLUDE OVERRIDES
TARGET_SPECIFIC_HEADER_PATH := $(LOCAL_PATH)/include

### GRAPHICS
USE_OPENGL_RENDERER := true
# hwcomposer insignal
BOARD_HDMI_INCAPABLE := true
# frameworks/native/services/surfaceflinger
# Android keeps 2 surface buffers at all time in case the hwcomposer
# misses the time to swap buffers (in cases where it takes 16ms or
# less). Use 3 to avoid timing issues.
NUM_FRAMEBUFFER_SURFACE_BUFFERS := 3

# GSC
#BOARD_USES_ONLY_GSC0_GSC1 := true

# BOOT ANIMATION
TARGET_BOOTANIMATION_PRELOAD := true
TARGET_BOOTANIMATION_TEXTURE_CACHE := true

### OMX (insignal)
BOARD_USE_DMA_BUF := true
BOARD_USE_IMPROVED_BUFFER := true
BOARD_USE_STOREMETADATA := true
BOARD_USE_METADATABUFFERTYPE := true
BOARD_USE_ANB_OUTBUF_SHARE := true
BOARD_USE_S3D_SUPPORT := true

# HEVC support in libvideocodec
BOARD_USE_HEVC_HWIP := true

BOARD_USE_GSC_RGB_ENCODER := true
BOARD_USE_ENCODER_RGBINPUT_SUPPORT := true

BOARD_USE_VP8ENC_SUPPORT := true
BOARD_USE_HEVCDEC_SUPPORT := true

BOARD_USE_WMA_CODEC := true
BOARD_USE_ALP_AUDIO := true
BOARD_USE_SEIREN_AUDIO := true

# WIFI
BOARD_WPA_SUPPLICANT_DRIVER      := NL80211
WPA_SUPPLICANT_VERSION           := VER_0_8_X
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_bcmdhd
BOARD_HOSTAPD_DRIVER             := NL80211
BOARD_HOSTAPD_PRIVATE_LIB        := lib_driver_cmd_bcmdhd
BOARD_WLAN_DEVICE                := bcmdhd
WIFI_DRIVER_FW_PATH_PARAM        := "/sys/module/dhd/parameters/firmware_path"
WIFI_DRIVER_NVRAM_PATH_PARAM     := "/sys/module/dhd/parameters/nvram_path"
WIFI_DRIVER_NVRAM_PATH           := "/etc/wifi/nvram_net.txt"
WIFI_DRIVER_FW_PATH_STA          := "/etc/wifi/bcmdhd_sta.bin"
WIFI_DRIVER_FW_PATH_AP           := "/etc/wifi/bcmdhd_apsta.bin"
# MACLOADER
BOARD_HAVE_SAMSUNG_WIFI := true

# BLUETOOTH
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_BCM := true
BOARD_HAVE_SAMSUNG_BLUETOOTH := true
BOARD_BLUEDROID_VENDOR_CONF := $(LOCAL_PATH)/bluetooth/libbt_vndcfg.txt
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := $(LOCAL_PATH)/bluetooth

### NFC
BOARD_NFC_CHIPSET := pn547
BOARD_NFC_HAL_SUFFIX := $(TARGET_BOOTLOADER_BOARD_NAME)

### CAMERA
# frameworks/av/services/camera/libcameraservice
BOARD_NEEDS_MEMORYHEAPION := true
# hardware/samsung_slsi-cm/exynos5/libgscaler
BOARD_USES_SCALER := true
BOARD_USES_DT := true
BOARD_USES_DT_SHORTNAME := true
# frameworks/av/camera, camera blob support
COMMON_GLOBAL_CFLAGS += -DSAMSUNG_CAMERA_HARDWARE
# frameworks/av/media/libstagefright, for libwvm.so
COMMON_GLOBAL_CFLAGS += -DADD_LEGACY_ACQUIRE_BUFFER_SYMBOL
# device specific gralloc header
COMMON_GLOBAL_CFLAGS += -DEXYNOS5_ENHANCEMENTS
# frameworks/av/media/libstagefright
COMMON_GLOBAL_CFLAGS += -DUSE_NATIVE_SEC_NV12TILED
BOARD_USE_SAMSUNG_CAMERAFORMAT_NV21 := true

### LIGHTS
TARGET_PROVIDES_LIBLIGHT := false

### POWER
TARGET_POWERHAL_VARIANT := samsung

### CHARGER
# system/core/init/Android.mk
BOARD_CHARGING_MODE_BOOTING_LPM := /sys/class/power_supply/battery/batt_lp_charging
# system/core/healthd/Android.mk
BOARD_CHARGER_ENABLE_SUSPEND := true
BOARD_CHARGER_SHOW_PERCENTAGE := true
RED_LED_PATH := "/sys/class/leds/led_r/brightness"
GREEN_LED_PATH := "/sys/class/leds/led_g/brightness"
BLUE_LED_PATH := "/sys/class/leds/led_b/brightness"
BACKLIGHT_PATH := "/sys/class/backlight/panel/brightness"
CHARGING_ENABLED_PATH := /sys/class/power_supply/battery/batt_lp_charging

### RIL
BOARD_VENDOR := samsung
BOARD_PROVIDES_LIBRIL := true
# hardware/samsung/ril
BOARD_MODEM_TYPE := xmm7260
# RIL.java overwrite
BOARD_RIL_CLASS := ../../../$(LOCAL_PATH)/ril
# frameworks/native/libs/binder/Parcel.cpp
COMMON_GLOBAL_CFLAGS += -DDISABLE_ASHMEM_TRACKING

### SENSORS
TARGET_NO_SENSOR_PERMISSION_CHECK := true

### FONTS
EXTENDED_FONT_FOOTPRINT := true

### WEBKIT
ENABLE_WEBGL := true

### CMHW
BOARD_HARDWARE_CLASS += hardware/samsung/cmhw

# SELINUX
BOARD_SEPOLICY_DIRS := \
	device/samsung/slte/sepolicy

###########################################################
### CYANOGEN RECOVERY
###########################################################

TARGET_RECOVERY_FSTAB := $(LOCAL_PATH)/ramdisk/fstab.universal5430
BOARD_HAS_DOWNLOAD_MODE := true

###########################################################
### TWRP RECOVERY
###########################################################

#RECOVERY_VARIANT := twrp
#TARGET_RECOVERY_FSTAB := $(LOCAL_PATH)/twrp.fstab

TW_THEME := portrait_hdpi

# Use our own init.rc without setting up functionfs
TARGET_RECOVERY_PIXEL_FORMAT := "BRGA_8888"
TARGET_RECOVERY_DEVICE_MODULES += prebuilt_file_contexts init.recovery.usb.rc

TW_BRIGHTNESS_PATH := /sys/class/backlight/panel/brightness
TW_MAX_BRIGHTNESS := 255

BOARD_HAS_NO_REAL_SDCARD := true
RECOVERY_GRAPHICS_USE_LINELENGTH := true
RECOVERY_SDCARD_ON_DATA := true

TW_NO_REBOOT_BOOTLOADER := true
TW_HAS_DOWNLOAD_MODE := true

TW_INCLUDE_L_CRYPTO := true

# Provide our own init.recovery.usb.rc
TW_EXCLUDE_DEFAULT_USB_INIT := true

# The kernel has exfat support.
TW_NO_EXFAT_FUSE := true
TW_EXCLUDE_SUPERSU := true
