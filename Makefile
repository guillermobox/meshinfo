
CFLAGS = -O2 -Wall
LDFLAGS = -lm

ALL: meshinfo

clean:
	rm -f *.o meshinfo

meshinfo: main.o fileparsers.o metrics.o
	$(CC) $^ $(LDFLAGS) -o $@

.PHONY: clean
