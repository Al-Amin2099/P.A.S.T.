# Arduino Uno R3 Makefile
# Configuration
MCU = atmega328p
F_CPU = 16000000UL
PORT = /dev/ttyACM0
PROGRAMMER = arduino
BAUDRATE = 115200

# Arduino paths (adjust these paths if needed)
ARDUINO_DIR = /usr/share/arduino
ARDUINO_CORE_DIR = $(ARDUINO_DIR)/hardware/arduino/avr/cores/arduino
ARDUINO_VARIANT_DIR = $(ARDUINO_DIR)/hardware/arduino/avr/variants/standard
ARDUINO_LIB_DIR = $(ARDUINO_DIR)/libraries

# Tools
CC = avr-gcc
CXX = avr-g++
OBJCOPY = avr-objcopy
AVRDUDE = avrdude
AR = avr-ar

# Compiler flags
CPPFLAGS = -DF_CPU=$(F_CPU) -DARDUINO=10819 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR
CPPFLAGS += -I$(ARDUINO_CORE_DIR) -I$(ARDUINO_VARIANT_DIR) -I/usr/share/arduino/hardware/arduino/avr/libraries/Wire/src

CFLAGS = -Wall -Os -mmcu=$(MCU) $(CPPFLAGS) -std=gnu11 -ffunction-sections -fdata-sections
CXXFLAGS = -Wall -Os -mmcu=$(MCU) $(CPPFLAGS) -std=gnu++11 -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics

# Linker flags
LDFLAGS = -mmcu=$(MCU) -Wl,--gc-sections

# Object copy flags
OBJCOPYFLAGS = -j .text -j .data -O ihex

# AVRDUDE flags
AVRDUDEFLAGS = -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -b $(BAUDRATE)

# Arduino core source files
ARDUINO_CORE_SRCS = $(wildcard $(ARDUINO_CORE_DIR)/*.c)
ARDUINO_CORE_CPPSRCS = $(wildcard $(ARDUINO_CORE_DIR)/*.cpp)
ARDUINO_CORE_OBJS = $(ARDUINO_CORE_SRCS:.c=.o) $(ARDUINO_CORE_CPPSRCS:.cpp=.o)

# Wire library
WIRE_DIR = /usr/share/arduino/hardware/arduino/avr/libraries/Wire/src
WIRE_SRCS = $(wildcard $(WIRE_DIR)/*.cpp)
WIRE_OBJS = $(WIRE_SRCS:.cpp=.o)

# Default target
all: Primary.hex

# Preprocess .ino file to .cpp
Primary.cpp: Primary.ino
	@echo "// Auto-generated from Primary.ino" > $@
	@echo '#include "Arduino.h"' >> $@
	@echo "" >> $@
	@# Extract function declarations
	@grep -E "^[a-zA-Z_][a-zA-Z0-9_]*\s+[a-zA-Z_][a-zA-Z0-9_]*\s*\(" Primary.ino | \
		sed 's/{.*//g' | sed 's/$$/ ;/' >> $@ || true
	@echo "" >> $@
	@cat Primary.ino >> $@

# Create hex file from elf
Primary.hex: Primary.elf
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

# Create elf file from object and core library
Primary.elf: Primary.o core.a
	$(CXX) $(LDFLAGS) -o $@ Primary.o core.a -lm

# Compile .ino (via .cpp) source to object
Primary.o: Primary.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build Arduino core library
core.a: $(ARDUINO_CORE_OBJS) $(WIRE_OBJS)
	$(AR) rcs $@ $(ARDUINO_CORE_OBJS) $(WIRE_OBJS)

# Compile Arduino core C files
$(ARDUINO_CORE_DIR)/%.o: $(ARDUINO_CORE_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile Arduino core C++ files  
$(ARDUINO_CORE_DIR)/%.o: $(ARDUINO_CORE_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Upload to Arduino
upload: Primary.hex
	$(AVRDUDE) $(AVRDUDEFLAGS) -U flash:w:$<:i

# Compile Wire library files
$(WIRE_DIR)/%.o: $(WIRE_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f *.o *.elf *.hex *.a Primary.cpp
	rm -f $(ARDUINO_CORE_DIR)/*.o

# Show help
help:
	@echo "Usage:"
	@echo "  make          - Compile Primary.ino"
	@echo "  make upload   - Compile and upload to Arduino"
	@echo "  make clean    - Remove build files"
	@echo "  make help     - Show this help"

.PHONY: all upload clean help