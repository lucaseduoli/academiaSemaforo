CC = gcc
CFLAGS = -g -pthread -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast

PROGRAMS = academia

all: $(PROGRAMS)

clean:
	rm -f *.o *~ $(PROGRAMS)
