PROJECT = build/otter-iron-fw
BUILD_DIR = build
DEVICE=stm32f072cbt6
OPENCM3_DIR= ./libopencm3

CFILES = src/main.c \
         src/oled.c \
         src/menu.c \
         src/font.c \
         src/adc.c \
         src/buttons.c \
         src/time.c

include $(OPENCM3_DIR)/mk/genlink-config.mk
include rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk

.PHONY: size
size: $(PROJECT).elf
	$(PREFIX)size $<