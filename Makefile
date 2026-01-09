# ============================================================================
# FIRA - Fault Injection & Recovery Analysis
# Makefile for ATmega328P (Bare Metal)
# ============================================================================

# MCU Configuration
MCU         = atmega328p
F_CPU       = 16000000UL
BAUD        = 115200

# Toolchain (Homebrew on macOS)
CC          = avr-gcc
OBJCOPY     = avr-objcopy
OBJDUMP     = avr-objdump
SIZE        = avr-size
AVRDUDE     = avrdude

# Directories
SRC_DIR     = src
INC_DIR     = include
BUILD_DIR   = build

# Source files
SOURCES     = $(wildcard $(SRC_DIR)/*.c)
OBJECTS     = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

# Output
TARGET      = $(BUILD_DIR)/fira
ELF         = $(TARGET).elf
HEX         = $(TARGET).hex
MAP         = $(TARGET).map

# Compiler flags
CFLAGS      = -std=gnu99
CFLAGS     += -mmcu=$(MCU)
CFLAGS     += -DF_CPU=$(F_CPU)
CFLAGS     += -Os
CFLAGS     += -Wall -Wextra -Werror=return-type
CFLAGS     += -ffunction-sections -fdata-sections
CFLAGS     += -fshort-enums
CFLAGS     += -flto
CFLAGS     += -I$(INC_DIR)

# Linker flags
LDFLAGS     = -mmcu=$(MCU)
LDFLAGS    += -Wl,--gc-sections
LDFLAGS    += -Wl,-Map=$(MAP)
LDFLAGS    += -flto

# Programmer configuration (Arduino Uno bootloader)
PROGRAMMER  = arduino
PORT       ?= /dev/cu.usbmodem*

# ============================================================================
# TARGETS
# ============================================================================

.PHONY: all clean flash monitor size disasm

all: $(HEX) size

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "CC    $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Link
$(ELF): $(OBJECTS)
	@echo "LD    $@"
	@$(CC) $(LDFLAGS) $^ -o $@

# Create HEX file
$(HEX): $(ELF)
	@echo "HEX   $@"
	@$(OBJCOPY) -O ihex -R .eeprom $< $@

# Print size
size: $(ELF)
	@echo ""
	@echo "============================================================"
	@echo "                    MEMORY USAGE"
	@echo "============================================================"
	@$(SIZE) --format=avr --mcu=$(MCU) $(ELF)

# Flash to device
flash: $(HEX)
	@echo ""
	@echo "============================================================"
	@echo "                    FLASHING"
	@echo "============================================================"
	$(AVRDUDE) -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -U flash:w:$(HEX):i

# Serial monitor
monitor:
	@echo "Opening serial monitor at $(BAUD) baud..."
	@echo "Press Ctrl+A then Ctrl+\\ to exit"
	@screen $(PORT) $(BAUD)

# Alternative monitor using Python
pymonitor:
	@python3 -c "import serial; s=serial.Serial('$(PORT)', $(BAUD)); \
		import sys; \
		print('Connected. Press Ctrl+C to exit.'); \
		[print(s.readline().decode('utf-8', errors='ignore'), end='') for _ in iter(int, 1)]" \
		2>/dev/null || echo "Install pyserial: pip3 install pyserial"

# Generate disassembly
disasm: $(ELF)
	@$(OBJDUMP) -d -S $< > $(TARGET).lst
	@echo "Disassembly: $(TARGET).lst"

# Clean build files
clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR)

# Show help
help:
	@echo "FIRA Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build project (default)"
	@echo "  clean    - Remove build files"
	@echo "  flash    - Upload to device"
	@echo "  monitor  - Open serial monitor"
	@echo "  size     - Show memory usage"
	@echo "  disasm   - Generate disassembly"
	@echo ""
	@echo "Variables:"
	@echo "  PORT     - Serial port (default: /dev/cu.usbmodem*)"
	@echo ""
	@echo "Example:"
	@echo "  make flash PORT=/dev/cu.usbmodem14101"
