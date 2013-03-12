
.PHONY: clean

ALL: meshinfo

clean:
	rm -f *.o meshinfo

meshinfo: *.c
	$(CC) -g main.c -c -o main.o
	$(CC) -g fileparsers.c -c -o fileparsers.o
	$(CC) -g metrics.c -c -o metrics.o
	$(CC) -g main.o fileparsers.o metrics.o -lm -o meshinfo
