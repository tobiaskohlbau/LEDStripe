CC = avr-gcc
CC_OBJ = avr-objcopy
DOWNLOADER = avrdude
PROGRAMMER = arduino
SERIAL = /dev/ttyACM0
BAUD = 115200

SOURCES = main.c
OBJECTS = $(SOURCES:.c=.o)
PROJECTNAME = led

MMCU = atmega328p
F_CPU = 16000000UL

CFLAGS = -mmcu=$(MMCU) -Os -DF_CPU=$(F_CPU)


all: $(PROJECTNAME).hex

%.hex: %.elf
	$(CC_OBJ) -O ihex -R .eeprom $< $(PROJECTNAME).hex

%.elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


program: $(PROJECTNAME).hex
	$(DOWNLOADER) -F -V -e -c $(PROGRAMMER) -p $(MMCU) -P $(SERIAL) -b $(BAUD) -U flash:w:$(PROJECTNAME).hex
