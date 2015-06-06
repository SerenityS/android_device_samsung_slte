#
# Copyright (C) 2013 The CyanogenMod Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := device/samsung/slte

$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_full.mk)

DEVICE_PACKAGE_OVERLAYS += $(LOCAL_PATH)/overlay

# Ramdisk
#TARGET_PROVIDES_INIT_RC := true
PRODUCT_PACKAGES += \
    fstab.universal5430 \
    init.universal5430.rc \
    init.universal5430.usb.rc \
    init.universal5430.wifi.rc \
    ueventd.universal5430.rc \
    init.sec.boot.sh
    #init.samsung.rc \

# Recovery
PRODUCT_PACKAGES += \
    init.recovery.universal5430.rc

# Permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.audio.low_latency.xml:system/etc/permissions/android.hardware.audio.low_latency.xml \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml \
    frameworks/native/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.nfc.hce.xml:system/etc/permissions/android.hardware.nfc.hce.xml \
    frameworks/native/data/etc/android.hardware.nfc.xml:system/etc/permissions/android.hardware.nfc.xml \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:system/etc/permissions/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:system/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:system/etc/permissions/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:system/etc/permissions/android.hardware.sensor.stepdetector.xml \
    frameworks/native/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.ethernet.xml:system/etc/permissions/android.hardware.ethernet.xml \
    frameworks/native/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml \
    frameworks/native/data/etc/android.software.sip.xml:system/etc/permissions/android.software.sip.xml \
    external/ant-wireless/antradio-library/com.dsi.ant.antradio_library.xml:system/etc/permissions/com.dsi.ant.antradio_library.xml \
    frameworks/native/data/etc/com.android.nfc_extras.xml:system/etc/permissions/com.android.nfc_extras.xml \
    frameworks/native/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml

# Graphics
PRODUCT_AAPT_CONFIG := normal
PRODUCT_AAPT_PREF_CONFIG := xhdpi

PRODUCT_PROPERTY_OVERRIDES += \
    ro.opengles.version=196608

PRODUCT_PACKAGES += \
    libion_exynos \
    hwcomposer.exynos5 \
    gralloc.exynos5

# Boot animation
TARGET_SCREEN_HEIGHT := 1280
TARGET_SCREEN_WIDTH := 720

# Nfc
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/nfc/libnfc-sec-hal.conf:system/etc/libnfc-sec-hal.conf \
    $(LOCAL_PATH)/configs/nfc/libnfc-brcm.conf:system/etc/libnfc-brcm.conf \
    $(LOCAL_PATH)/configs/nfc/nfcee_access.xml:system/etc/nfcee_access.xml

PRODUCT_PACKAGES += \
    com.android.nfc_extras \
    NfcNci \
    libnfc-nci \
    Tag

# Filesystem management tools
#PRODUCT_PACKAGES += \
#    make_ext4fs \
#    e2fsck \
#    setup_fs

# RIL
PRODUCT_PACKAGES += \
    libsecril-client \
    libsecril-client-sap

# dex-opt to cache
PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.dexopt-data-only=0

# Wifi
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/wifi/wpa_supplicant.conf:system/etc/wifi/wpa_supplicant.conf \
    $(LOCAL_PATH)/configs/wifi/p2p_supplicant_overlay.conf:system/etc/wifi/p2p_supplicant_overlay.conf \
    $(LOCAL_PATH)/configs/wifi/wpa_supplicant_overlay.conf:system/etc/wifi/wpa_supplicant_overlay.conf

PRODUCT_PACKAGES += \
    libnetcmdiface \
    macloader \
    dhcpcd.conf \
    hostapd \
    hostapd_default.conf \
    libwpa_client \
    wpa_supplicant

# Audio
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/audio/audio_effects.conf:system/etc/audio_effects.conf \
    $(LOCAL_PATH)/configs/audio/audio_policy.conf:system/etc/audio_policy.conf \
    $(LOCAL_PATH)/configs/audio/mixer_paths.xml:system/etc/mixer_paths.xml

PRODUCT_PACKAGES += \
    audio.a2dp.default \
    audio.usb.default \
    audio.r_submix.default \
    audio.primary.universal5430

# Bluetooth
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/bluetooth/bcm4350_prepatch.hcd:system/vendor/firmware/bcm4350_prepatch.hcd

# Media profile
PRODUCT_COPY_FILES += \
    frameworks/av/media/libstagefright/data/media_codecs_ffmpeg.xml:system/etc/media_codecs_ffmpeg.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:system/etc/media_codecs_google_audio.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_telephony.xml:system/etc/media_codecs_google_telephony.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_video.xml:system/etc/media_codecs_google_video.xml \
    $(LOCAL_PATH)/configs/media/media_codecs.xml:system/etc/media_codecs.xml \
    $(LOCAL_PATH)/configs/media/media_profiles.xml:system/etc/media_profiles.xml

# OMX
#PRODUCT_PACKAGES += \
#    libcsc \
#    libstagefrighthw
#    libExynosOMX_Core \
#    libOMX.Exynos.MP3.Decoder \
#    libOMX.Exynos.MPEG4.Decoder \
#    libOMX.Exynos.MPEG4.Encoder \
#    libOMX.Exynos.MPEG2.Decoder \

PRODUCT_PACKAGES += \
    libcsc \
    libExynosOMX_Core \
    libstagefrighthw

# Power
PRODUCT_PACKAGES += \
    power.universal5430

# Lights
PRODUCT_PACKAGES += \
    lights.universal5430

# Gps
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/gps/gps.conf:system/etc/gps.conf \
    $(LOCAL_PATH)/configs/gps/gps.xml:system/etc/gps.xml

# Camera
PRODUCT_PACKAGES += \
    camera.universal5430 \
    libhwjpeg

# Touchscreen
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/idc/Synaptics_HID_TouchPad.idc:/system/usr/idc/Synaptics_HID_TouchPad.idc \
    $(LOCAL_PATH)/configs/keylayout/gpio_keys_8.kl:/system/usr/keylayout/gpio_keys_8.kl \
    $(LOCAL_PATH)/configs/keylayout/sec_touchkey.kl:/system/usr/keylayout/sec_touchkey.kl

# Keystore
PRODUCT_PACKAGES += \
    keystore.exynos5

# Charger
PRODUCT_PACKAGES += \
    charger_res_images \
    charger

# Misc
PRODUCT_PACKAGES += \
    com.android.future.usb.accessory

# Packages
PRODUCT_PACKAGES += \
    SamsungServiceMode

# ANT+
PRODUCT_PACKAGES += \
    AntHalService \
    com.dsi.ant.antradio_library \
    libantradio

# Filesystem management tools
#PRODUCT_PACKAGES += \
#    fsck.f2fs \
#    mkfs.f2fs

# MobiCore setup
#PRODUCT_PACKAGES += \
#    libMcClient \
#    libMcRegistry \
#    libPaApi \
#    libgdmcprov \
#    mcDriverDaemon

# call dalvik heap config
$(call inherit-product-if-exists, frameworks/native/build/phone-xhdpi-2048-dalvik-heap.mk)

# call the proprietary setup
$(call inherit-product-if-exists, vendor/samsung/slte/slte-vendor.mk)
