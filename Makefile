all: hive

hive: hive.o
	cc hive.o -o hive -lcairo -lX11 -lm

clean:
	rm -f hive *.o
