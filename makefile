run: recaman.elf
	clear
	./recaman.elf 0 2

recaman.elf: recaman.c
	gcc recaman.c -o recaman.elf -O3
