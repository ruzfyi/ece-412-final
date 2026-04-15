# Makefile
MCU      = atmega328p
PROGRAMMER = xplainedmini
F_CPU    = 16000000UL

CC       = avr-gcc
OBJCOPY  = avr-objcopy
CFLAGS   = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra -std=c11
TARGET   = main

all: $(TARGET).hex

$(TARGET).elf: main.c
	$(CC) $(CFLAGS) -o $@ $<

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	avr-size --mcu=$(MCU) -C $<

flash: $(TARGET).hex
	avrdude -c $(PROGRAMMER) -p $(MCU) -U flash:w:$<

fuses:
	avrdude -c $(PROGRAMMER) -p $(MCU) -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h

clean:
	rm -f *.elf *.hex *.o *.obj

.PHONY: all flash fuses clean
