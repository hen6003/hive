all: hive

CFLAGS = -I/usr/include/SDL2/

#hive.o: hive.c
	#cc -I/usr/include/SDL2/ -c

hive: hive.o
	cc hive.o -o hive -lSDL2 -lm

clean:
	rm -f hive *.o
