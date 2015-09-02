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

PRODUCT_BUILD_PROP_OVERRIDES += \
	PRODUCT_NAME=sltexx \
	PRODUCT_MODEL=SM-G850F \
	PRODUCT_MANUFACTURER=samsung \
	TARGET_DEVICE=slte \
	BUILD_FINGERPRINT=samsung/sltexx/slte:5.0.2/LRX22G/G850FXXU2COD4:user/release-keys \
	PRIVATE_BUILD_DESC="sltexx-user 5.0.2 LRX22G G850FXXU2COD4 release-keys"
