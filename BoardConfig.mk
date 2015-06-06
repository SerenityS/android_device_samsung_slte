LOCAL_PATH := device/samsung/slte

# Assert
TARGET_OTA_ASSERT_DEVICE := slte,sltexx

# Platform
BOARD_VENDOR := samsung
TARGET_BOARD_PLATFORM := exynos5
TARGET_SLSI_VARIANT := insignal
TARGET_SOC := exynos5430

# Architecture
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_SMP := true
TARGET_CPU_VARIANT := cortex-a15
ARCH_ARM_HAVE_TLS_REGISTER := true
#OVERRIDE_RS_DRIVER := libRSDriverArm.so

# Bootloader
TARGET_BOOTLOADER_BOARD_NAME := universal5430
TARGET_NO_BOOTLOADER := true
TARGET_NO_RADIOIMAGE := true

# Partitions
BOARD_BOOTIMAGE_PARTITION_SIZE := 0x00a00000
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 0x00c00000
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 2401239040
BOARD_USERDATAIMAGE_PARTITION_SIZE := 28219277312
BOARD_FLASH_BLOCK_SIZE := 131072

# FIMG2D
BOARD_USES_SKIA_FIMGAPI := true
BOARD_USES_NEON_BLITANTIH := true

# Kernel
#TARGET_PREBUILT_KERNEL := device/samsung/slte/kernel
TARGET_KERNEL_SOURCE := kernel/samsung/slte
TARGET_KERNEL_CONFIG := cm_exynos5430-slte_defconfig
BOARD_KERNEL_BASE := 0x10000000
BOARD_KERNEL_PAGESIZE := 2048
#BOARD_KERNEL_CMDLINE := The bootloader ignores the cmdline from the boot.img
BOARD_KERNEL_SEPARATED_DT := false
#BOARD_MKBOOTIMG_ARGS := --ramdisk_offset 0x01000000 --tags_offset 0x00000100 --dt device/samsung/slte/dtb.img
BOARD_CUSTOM_BOOTIMG_MK := $(LOCAL_PATH)/mkbootimg.mk
BOARD_MKBOOTIMG_ARGS := --kernel_offset 0x00008000 --ramdisk_offset 0x01000000 --tags_offset 0x00000100 --dt out/target/product/$(TARGET_DEVICE)/dt.img

# Include path
TARGET_SPECIFIC_HEADER_PATH := $(LOCAL_PATH)/include

# Audio
BOARD_USES_LIBMEDIA_WITH_AUDIOPARAMETER := true
#COMMON_GLOBAL_CFLAGS += -DSAMPLE_RATE_48K
#AUDIO_FEATURE_LOW_LATENCY_PRIMARY := true
#TARGET_HAVE_DYN_A2DP_SAMPLERATE := true
#BOARD_USES_ALSA_AUDIO := true

# ANT+
BOARD_ANT_WIRELESS_DEVICE := "vfs-prerelease"

# Bluetooth
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_BCM := true
BOARD_BLUEDROID_VENDOR_CONF := $(LOCAL_PATH)/bluetooth/libbt_vndcfg.txt
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := $(LOCAL_PATH)/bluetooth

# Camera
# COMMON_GLOBAL_CFLAGS += -DUSE_MEMORY_HEAP_ION
BOARD_NEEDS_MEMORYHEAPION := true
COMMON_GLOBAL_CFLAGS += -DDISABLE_HW_ID_MATCH_CHECK
COMMON_GLOBAL_CFLAGS += -DSAMSUNG_CAMERA_HARDWARE
COMMON_GLOBAL_CFLAGS += -DSAMSUNG_DVFS

# Media
#COMMON_GLOBAL_CFLAGS += -DUSE_NATIVE_SEC_NV12TILED # use format from fw/native
COMMON_GLOBAL_CFLAGS += -DWIDEVINE_PLUGIN_PRE_NOTIFY_ERROR

#COMMON_GLOBAL_CFLAGS += -DTARGET_ENABLE_QC_AV_ENHANCEMENTS
#BOARD_USE_SAMSUNG_COLORFORMAT := true

# Scaler
BOARD_USES_SCALER := true

# Charger
BOARD_BATTERY_DEVICE_NAME := battery
# system/core/init/Android.mk
BOARD_CHARGING_MODE_BOOTING_LPM := /sys/class/power_supply/battery/batt_lp_charging
# system/core/healthd/Android.mk
BOARD_CHARGER_ENABLE_SUSPEND := true
BOARD_CHARGER_SHOW_PERCENTAGE := true
CHARGING_ENABLED_PATH := /sys/class/power_supply/battery/batt_lp_charging

# Graphics
USE_OPENGL_RENDERER := true
BOARD_EGL_CFG := $(LOCAL_PATH)/configs/egl/egl.cfg
BOARD_HDMI_INCAPABLE := true
NUM_FRAMEBUFFER_SURFACE_BUFFERS := 5

# Enable QC's libm optimizations
TARGET_USE_QCOM_BIONIC_OPTIMIZATION := true

# OpenMAX Video
BOARD_USE_STOREMETADATA := true
BOARD_USE_METADATABUFFERTYPE := true
BOARD_USE_S3D_SUPPORT := true
BOARD_USE_DMA_BUF := true
BOARD_USE_ANB_OUTBUF_SHARE := true
BOARD_USE_GSC_RGB_ENCODER := true
BOARD_USE_IMPROVED_BUFFER := true
BOARD_USE_CSC_HW := false
BOARD_USE_H264_PREPEND_SPS_PPS := false
BOARD_USE_QOS_CTRL := false
BOARD_USE_VP8ENC_SUPPORT := true
BOARD_USE_ENCODER_RGBINPUT_SUPPORT := true
BOARD_USE_DUALDPB_MODE := true

# Include an expanded selection of fonts
EXTENDED_FONT_FOOTPRINT := true

# Hardware
BOARD_HARDWARE_CLASS := $(LOCAL_PATH)/cmhw

# PowerHAL
TARGET_POWERHAL_VARIANT := slte

# Nfc
BOARD_NFC_CHIPSET := pn547
BOARD_NFC_HAL_SUFFIX := universal5430

# RIL
BOARD_PROVIDES_LIBRIL := true
BOARD_MODEM_TYPE := xmm7260
BOARD_RIL_CLASS := ../../../device/samsung/slte/ril

# Wifi
BOARD_HAVE_SAMSUNG_WIFI := true
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

# Selinux
BOARD_SEPOLICY_DIRS := \
	device/samsung/slte/sepolicy

BOARD_SEPOLICY_UNION := \
	app.te \
	cpboot-daemon.te \
	domain.te \
	exyrngd.te \
	file_contexts \
	file.te \
	macloader.te \
	recovery.te \
	ueventd.te

# SurfaceFlinger
BOARD_USES_SYNC_MODE_FOR_MEDIA := true

# Webkit
ENABLE_WEBGL := true

# Init
TARGET_NR_SVC_SUPP_GIDS := 20

# WFD
BOARD_USES_WFD_SERVICE := true
BOARD_USES_WFD := true

# Recovery
#TARGET_RECOVERY_FSTAB := $(LOCAL_PATH)/rootdir/etc/fstab.universal5430
TARGET_RECOVERY_PIXEL_FORMAT := BRGA_8888
TARGET_USERIMAGES_USE_EXT4 := true
BOARD_USE_CUSTOM_RECOVERY_FONT := \"roboto_23x41.h\"
BOARD_HAS_NO_SELECT_BUTTON := true
BOARD_HAS_NO_REAL_SDCARD := true
BOARD_RECOVERY_SWIPE := true
RECOVERY_GRAPHICS_USE_LINELENGTH := true
RECOVERY_SDCARD_ON_DATA := true

# inherit from the proprietary version
-include vendor/samsung/slte/BoardConfigVendor.mk	
