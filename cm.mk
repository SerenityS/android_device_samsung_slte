## Specify phone tech before including full_phone
$(call inherit-product, vendor/cm/config/gsm.mk)

# Release name
PRODUCT_RELEASE_NAME := slte

# Inherit some common CM stuff.
$(call inherit-product, vendor/cm/config/common_full_phone.mk)

# Inherit device configuration
$(call inherit-product, device/samsung/slte/device_slte.mk)

## Device identifier. This must come after all inclusions
PRODUCT_DEVICE := slte
PRODUCT_NAME := cm_slte
PRODUCT_BRAND := samsung
PRODUCT_MODEL := slte
PRODUCT_MANUFACTURER := samsung
