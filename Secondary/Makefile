# Arduino Nano - pure avr-gcc/avrdude build, no Arduino IDE/core required
#
# NOTE on BAUDRATE: classic Nanos with the "old bootloader" (most common
# on cheap clones) flash at 57600. Nanos with the "new bootloader" use
# 115200, same as an Uno. If `make upload` times out or reports
# "not in sync", try switching this value.
MCU = atmega328p
F_CPU = 16000000UL
PORT = /dev/ttyUSB0
PROGRAMMER = arduino
BAUDRATE = 11520

CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

CFLAGS = -Wall -Wextra -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU) -std=gnu11 \
         -ffunction-sections -fdata-sections
LDFLAGS = -mmcu=$(MCU) -Wl,--gc-sections -lm

OBJCOPYFLAGS = -O ihex -R .eeprom

SRCS = Secondary.c gpio.h stepper.c
SRCS := $(filter %.c,$(SRCS))
OBJS = $(SRCS:.c=.o)

all: secondary.hex

secondary.elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

secondary.hex: secondary.elf
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

upload: secondary.hex
	$(AVRDUDE) -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -b $(BAUDRATE) -U flash:w:$<:i

size: secondary.elf
	avr-size --format=avr --mcu=$(MCU) $<

clean:
	rm -f *.o *.elf *.hex

.PHONY: all upload size clean
