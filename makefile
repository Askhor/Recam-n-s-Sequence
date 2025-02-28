run: recaman.elf
	clear
	./recaman.elf 0 100

recaman: recaman.c
	gcc recaman.c -o recaman.elf
