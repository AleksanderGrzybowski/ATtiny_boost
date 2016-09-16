all:
	avr-gcc -mmcu=attiny13 -Wall -Os -o main.elf main.c  -lm
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	sudo avrdude  -p attiny13 -c usbasp  -P usb -B 1 -U flash:w:main.hex
