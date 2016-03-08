
CFLAGS = -O2 -Wall -g
LDFLAGS = -lm -g

ALL: meshinfo

clean:
	rm -f *.o meshinfo

meshinfo: main.o fileparsers.o metrics.o
	$(CC) $^ $(LDFLAGS) -o $@

.PHONY: clean
