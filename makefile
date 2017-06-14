CC=gcc;
CFLAGS=-Wall -std=c99
all: main.c
	gcc main.c -o dependency_resolver -std=c99 -I.