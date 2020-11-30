make: compile_insert compile_consult compile_consult_all

compile_insert: insert.c
	gcc -Wall -g insert.c -o insert_r -lrt -lpthread
	
compile_consult: consult.c
	gcc -Wall -g consult.c -o consult_r -lrt -lpthread
	
compile_consult_all: consult_all.c
	gcc -Wall -g consult_all.c -o consult_all_r -lrt -lpthread	

insert: insert_r
	./insert_r

consult: consult_r
	./consult_r
	
consult_all: consult_all_r
	./consult_all_r
	
clean:
	rm -rf insert_r consult_r consult_all_r
