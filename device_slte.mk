$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_full.mk)

# The gps config appropriate for this device
$(call inherit-product, device/common/gps/gps_eu_supl.mk)

$(call inherit-product-if-exists, vendor/samsung/slte/slte-vendor.mk)

DEVICE_PACKAGE_OVERLAYS += device/samsung/slte/overlay

LOCAL_PATH := device/samsung/slte

###########################################################
### RAMDISK
###########################################################

PRODUCT_PACKAGES += \
    fstab.universal5430 \
    init.universal5430.rc \
    init.universal5430.usb.rc \
    init.universal5430.wifi.rc \
    ueventd.universal5430.rc

# This file gets automatically copied to the recovery root by CM.
PRODUCT_COPY_FILES += \
	$(LOCAL_PATH)/recovery/init.recovery.universal5430.rc:root/init.recovery.universal5430.rc

PRODUCT_PROPERTY_OVERRIDES += \
	ro.hardware = universal5430

$(call inherit-product, build/target/product/full.mk)

PRODUCT_BUILD_PROP_OVERRIDES += BUILD_UTC_DATE=0
PRODUCT_NAME := full_slte
PRODUCT_DEVICE := slte
