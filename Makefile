all: game

CFLAGS = -I/usr/include/SDL2/

#game.o: game.c
	#cc -I/usr/include/SDL2/ -c

game: game.o
	cc game.o -o game -lSDL2 -lm

clean:
	rm -f game *.o
