all: main

main: main.o shell.o
	gcc -o main main.o shell.o

shell.o: shell.c shell.h
	gcc -c shell.c

main.o: main.c shell.h
	gcc -c main.c

.PHONY: all run clean

run:
	./main

clean:
	rm *.o
	rm main
