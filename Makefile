MCU=attiny13
CC=avr-gcc
AT=TINY
OBJCOPY=avr-objcopy
F_CPU = 1200000


CFLAGS=-DTXTMODE -g -mmcu=$(MCU) -DAT$(AT) -Wall -I/usr/local/avr/include/avr -DF_CPU=$(F_CPU)UL -std=gnu99 -I../lib
CFLAGS=-D__AVR_LIBC_DEPRECATED_ENABLE__ -g -mmcu=$(MCU) -DAT$(AT) -Wall -I/usr/local/avr/include/avr -DF_CPU=$(F_CPU)UL -std=gnu99 -I../lib -DPALEVO

all: keypad.hex
keypad.hex : keypad.out
	$(OBJCOPY) -S -R .eeprom -R .fuse -O ihex keypad.out keypad.hex
	$(OBJCOPY) -S -j .fuse -O ihex keypad.out keypad.fuse.hex
	@echo -n "Size: "
	avr-size keypad.out
keypad.out : keypad.o
	$(CC) $(CFLAGS) -o keypad.out -Wl,-Map,keypad.map keypad.o
keypad.o : keypad.c
	$(CC) $(CFLAGS) -Os -c keypad.c
uartio.o : uartio.c
	$(CC) $(CFLAGS) -Os -c uartio.c
clean:
	rm -f *.o *.map *.out *.bin *.hex
install: keypad.hex
#	avrdude -p $(MCU) -c dragon_isp -P usb -V -D -U eeprom:r:keypad.eeprom.hex:i 
#	sleep 5
	avrdude -p $(MCU) -c dragon_isp -P usb -V -U flash:w:keypad.hex:i 

fuse:
#	avrdude -p $(MCU) dragon_isp -P usb -V -U lfuse:w:0x64:m -U hfuse:w:0xdf:m -U lock:w:0xff:m
#-U eeprom:w:keypad.eeprom.hex:i
