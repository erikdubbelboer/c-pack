
all: test example


test: test.c pack.c pack.h
	gcc -O3 -Wall -pedantic -std=c99 -D_GNU_SOURCE $^ -o $@
	./test


example: example.c pack.c pack.h
	gcc -O3 -Wall -pedantic -std=c99 -D_GNU_SOURCE $^ -o $@


clean:
	rm -f test example


# always perform the actions to build test and clean
.PHONY: test clean

