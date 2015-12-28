# Release name
PRODUCT_RELEASE_NAME := slte

# Inherit some common CM stuff.
$(call inherit-product, vendor/cm/config/common_full_phone.mk)

# Inherit nfc enhanced configuration
$(call inherit-product, vendor/cm/config/nfc_enhanced.mk)

# Inherit device configuration
$(call inherit-product, device/samsung/slte/device_slte.mk)

# Boot animation
TARGET_SCREEN_HEIGHT := 1280
TARGET_SCREEN_WIDTH := 720

## Device identifier. This must come after all inclusions
PRODUCT_NAME := cm_slte
PRODUCT_DEVICE := slte

PRODUCT_BUILD_PROP_OVERRIDES += \
	PRODUCT_NAME=slteskt \
	PRODUCT_MODEL=SM-G850S \
	PRODUCT_MANUFACTURER=samsung \
	TARGET_DEVICE=slte \
	BUILD_FINGERPRINT=samsung/slteskt/slteskt:5.0.2/LRX22G/G850SKSU2COJ2:user/release-keys \
	PRIVATE_BUILD_DESC="slteskt-user 5.0.2 LRX22G G850SKSU2COJ2 release-keys"
