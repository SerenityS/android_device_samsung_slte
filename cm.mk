# Release name
PRODUCT_RELEASE_NAME := slte

# Inherit some common CM stuff.
$(call inherit-product, vendor/cm/config/common_full_phone.mk)

# Inherit nfc enhanced configuration
$(call inherit-product, vendor/cm/config/nfc_enhanced.mk)

# Inherit device configuration
$(call inherit-product, device/samsung/slte/device_slte.mk)

## Device identifier. This must come after all inclusions
PRODUCT_NAME := cm_slte
PRODUCT_DEVICE := slte
PRODUCT_BRAND := samsung
PRODUCT_MODEL := SM-G850F
PRODUCT_MANUFACTURER := samsung
