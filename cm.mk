$(call inherit-product, device/samsung/slte/full_slte.mk)

# Enhanced NFC
$(call inherit-product, vendor/cm/config/nfc_enhanced.mk)

# Inherit some common CM stuff.
$(call inherit-product, vendor/cm/config/common_full_phone.mk)

PRODUCT_NAME := cm_slte
PRODUCT_DEVICE := slte

PRODUCT_BUILD_PROP_OVERRIDES += \
	PRODUCT_NAME=sltexx \
	TARGET_DEVICE=slte \
	BUILD_FINGERPRINT=samsung/sltexx/slte:5.0.2/LRX22G/G850FXXU2COD4:user/release-keys \
	PRIVATE_BUILD_DESC="sltexx-user 5.0.2 LRX22G G850FXXU2COD4 release-keys"
