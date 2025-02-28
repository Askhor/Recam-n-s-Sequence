run: recaman.elf
	clear
	./recaman.elf 0 10

recaman.elf: recaman.c
	gcc recaman.c -o recaman.elf
