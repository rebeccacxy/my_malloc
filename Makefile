FLAGS=-g -Wall -Wpedantic -Wno-unused-result -O3 -pthread -Wno-deprecated-declarations -DVERBOSE

$(shell mkdir -p bin)

malloc_with_brk:
	gcc $(FLAGS) -o bin/$@ test/my_malloc.t.c src/$@.c -I include 