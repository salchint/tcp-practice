IDIR =../inc
CC=gcc
CFLAGS=-I$(IDIR) -Wall -pedantic -std=gnu99

ODIR=obj
LDIR =../lib

LIBS=-lm

_DEPS = errorReturn.h protocol.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o ../../inc/errorReturn.c ../../inc/protocol.c
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

roc2310: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
	touch roc2310

