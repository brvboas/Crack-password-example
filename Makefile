CC = gcc
#CFLAGS = -g
CFLAGS = -Wall -g -pthread
LDFLAGS = -lcrypt

PROGRAMS = crack-passwd

all: $(PROGRAMS)

clean:
	rm -f *.o *~ $(PROGRAMS)
