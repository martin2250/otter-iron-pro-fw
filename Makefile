PROJECT = build/otter-iron-fw
BUILD_DIR = build
DEVICE=stm32f072cbt6
OPENCM3_DIR= ./libopencm3

CFILES = src/main.c

include $(OPENCM3_DIR)/mk/genlink-config.mk
include rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk
