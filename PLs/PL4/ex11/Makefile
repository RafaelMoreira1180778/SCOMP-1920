main: main.o
	gcc -Wall -g main.o -o main -lrt -lpthread

main.o: main.c 
	gcc -Wall -g -c main.c

run: main
	./main
	
gdb: main
	gdb -tui main
	
vg: main
	valgrind ./main

clean:
	rm *.o main
