
all: obex

obex: obex.o
	gcc -o $@ $^ -lopenobex
