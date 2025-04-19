.FORCE:

clean:
	rm -f hashmap main server

hashmap: .FORCE main.c hashmap.c
	clang main.c hashmap.c cache.c -o main -std=c2x

server: .FORCE server.c hashmap.c cache.c
	clang server.c hashmap.c cache.c -o server -std=c2x

all: hashmap server

.PHONY: clean
