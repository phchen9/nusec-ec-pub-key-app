CFLAGS	?= -O2
WARNING_CFLAGS ?= -Wall -W
LDFLAGS ?=

MBEDTLS_BUILD_DIR ?= mbedtls

LOCAL_CFLAGS = $(WARNING_CFLAGS) -I$(MBEDTLS_BUILD_DIR)/include -D_FILE_OFFSET_BITS=64
LOCAL_LDFLAGS = -L$(MBEDTLS_BUILD_DIR)/library \
		-lmbedtls \
		-lmbedx509 \
		-lmbedcrypto

DEP = $(MBEDTLS_BUILD_DIR)/library/libmbedcrypto.a \
		$(MBEDTLS_BUILD_DIR)/library/libmbedx509.a \
		$(MBEDTLS_BUILD_DIR)/library/libmbedtls.a

TARGET_EXEC = ec_pub_key_app


.PHONY: all clean

all: $(TARGET_EXEC)

$(DEP):
	$(MAKE) -C $(MBEDTLS_BUILD_DIR)/library

ec_pub_key_app: src/ec_pub_key_app.c $(DEP)
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) src/ec_pub_key_app.c $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

clean:
	rm -f $(TARGET_EXEC)