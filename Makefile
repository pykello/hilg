
LDFLAGS=-lncurses
CFLAGS=-I .

box_mover: hilg.o

all: box_mover

clean:
	rm -f hilg.o
	rm -f box_mover
