CC=gcc
CFLAGS= -O3 -Wall -g
LIBS= -lm -lgpiod
AR=ar

ALLBIN=settca6424a

all: ${ALLBIN}

clean:
	rm -f *.o ${ALLBIN}

settca6424a: i2c_tca6424a.o settca6424a.o
	$(CC) i2c_tca6424a.o settca6424a.o -o settca6424a ${LIBS}

