
ALL:
	$(CC) -g main.c -c -o main.o
	$(CC) -g fileparsers.c -c -o fileparsers.o
	$(CC) -g metrics.c -c -o metrics.o
	$(CC) -g main.o fileparsers.o metrics.o -o meshinfo

run: ALL
	./meshinfo highVd_v1.msh
