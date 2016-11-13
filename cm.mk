# Inherit device configuration
$(call inherit-product, device/samsung/sltexx/device.mk)

# Inherit some common CM stuff.
$(call inherit-product, vendor/cm/config/common_full_phone.mk)

# Inherit nfc enhanced configuration
$(call inherit-product, vendor/cm/config/nfc_enhanced.mk)

## Device identifier. This must come after all inclusions
PRODUCT_NAME := cm_sltexx
PRODUCT_DEVICE := sltexx
PRODUCT_BRAND := samsung
PRODUCT_MODEL := SM-G850F
PRODUCT_MANUFACTURER := samsung

PRODUCT_BUILD_PROP_OVERRIDES += \
    PRODUCT_NAME=sltexx
