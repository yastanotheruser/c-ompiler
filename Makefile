objects = main.o lexer.o buffer.o lib/cmem.o
CFLAGS += -Ilib

.PHONY : all
all : c-ompiler

c-ompiler : $(objects)
	$(CC) $(CFLAGS) -o $@ $(objects)
main.o : lexer.h lib/cmem.h lib/cdefs.h
lexer.o : lib/cmem.h lib/cdefs.h
buffer.o : lib/cmem.h lib/cdefs.h
lib/cmem.o :

.PHONY : clean
clean :
	rm -rf c-ompiler $(objects)
