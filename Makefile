objects = main.o parser.o lexer.o stack.o buffer.o lib/cmem.o
CFLAGS += -Ilib

.PHONY : all
all : c-ompiler

c-ompiler : $(objects)
	$(CC) $(CFLAGS) -o $@ $(objects)
main.o : parser.h lexer.h lib/cmem.h lib/cdefs.h
parser.o : stack.h lexer.h
lexer.o : lib/cmem.h lib/cdefs.h
stack.o : lib/cmem.h lib/cdefs.h
buffer.o : lib/cmem.h lib/cdefs.h
lib/cmem.o :

.PHONY : clean
clean :
	rm -rf c-ompiler $(objects)
