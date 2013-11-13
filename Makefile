tetris : *.h *.c
	gcc -g -o $@ *.c -lpthread

clean :
	rm -fr tetris

