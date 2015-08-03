all:
	gcc xor.c -o xor
debug:
	gcc -ggdb3 -Wall xor.c -o xor
clean:
	rm xor
