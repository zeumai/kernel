.PHONY: all
all: lisp

lisp: $(wildcard *.c) $(wildcard */*.c)
	gcc -Wall -o $@ $^
