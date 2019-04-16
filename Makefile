CC = gcc

all:  c8vm

c8vm:  main.o cache.o
	$(CC) -o c8vm main.o c8.o -lm -g

main.o:  main.c c8.h
	$(CC) -c main.c

cache.o:  c8.c c8.h
	$(CC) -c c8.c

clean:
	rm -f *.o c8vm
