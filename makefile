VPATH=src
FLAGS=-std=c11 -lrt
CC=clang

router: router.c 
	$(CC) $(FLAGS) -o bin/$@ $^

mail: mail.c
	$(CC) $(FLAGS) -o bin/$@ $^

clean:
	rm bin/*
