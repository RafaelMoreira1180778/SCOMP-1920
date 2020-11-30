main: main.o
	gcc -Wall -lm -g main.o -o main -lrt

main.o: main.c
	gcc -Wall -lm -g -c main.c -o main.o 
	
gdb: main
	gdb -tui main

run: main
	./main

vg: main
	valgrind ./main

clean:
	rm main.o main
