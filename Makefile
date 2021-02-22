all: hive

hive: hive.o render.o window.o
	cc hive.o render.o window.o -o hive -lcairo -lX11 -lm

clean:
	rm -f hive *.o