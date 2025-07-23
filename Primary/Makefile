# Arduino Uno R3 Makefile

# Configuration
MCU = atmega328p
F_CPU = 16000000UL
PORT = /dev/ttyACM0
PROGRAMMER = arduino
BAUDRATE = 115200

# Tools
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

# Compiler flags
CFLAGS = -Wall -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU)
CFLAGS += -I/usr/share/arduino/hardware/arduino/avr/cores/arduino
CFLAGS += -I/usr/share/arduino/hardware/arduino/avr/variants/standard

# Object copy flags
OBJCOPYFLAGS = -j .text -j .data -O ihex

# AVRDUDE flags
AVRDUDEFLAGS = -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -b $(BAUDRATE)

# Default target
all: Primary.hex

# Create hex file from elf
Primary.hex: Primary.elf
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

# Create elf file from object
Primary.elf: Primary.o
	$(CC) $(CFLAGS) -o $@ $<

# Compile C source to object
Primary.o: Primary.c
	$(CC) $(CFLAGS) -c $<

# Upload to Arduino
upload: Primary.hex
	$(AVRDUDE) $(AVRDUDEFLAGS) -U flash:w:$<:i

# Clean build files
clean:
	rm -f *.o *.elf *.hex

# Show help
help:
	@echo "Usage:"
	@echo "  make          - Compile Primary.c"
	@echo "  make upload   - Compile and upload to Arduino"
	@echo "  make clean    - Remove build files"
	@echo "  make help     - Show this help"

.PHONY: all upload clean help
