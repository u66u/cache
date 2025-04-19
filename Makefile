.FORCE:

clean:
	rm -f hashmap main

all: hashmap

hashmap: .FORCE main.c hashmap.c
	clang main.c hashmap.c -o main -std=c23

.PHONY: clean
