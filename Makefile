all:
	avr-gcc -mmcu=attiny13 -Wall -Os -o a.elf a.c  -lm
	avr-objcopy -j .text -j .data -O ihex a.elf a.hex
	sudo avrdude  -p attiny13 -c usbasp  -P usb -B 1 -U flash:w:a.hex
