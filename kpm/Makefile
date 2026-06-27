CROSS_COMPILE ?= aarch64-linux-gnu-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
CFLAGS = -nostdlib -fPIC -O2

all: potato_hack.ko

potato_hack.ko: potato_hack.c
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(LD) -r $@.o -o $@
	rm $@.o

clean:
	rm -f *.ko *.o
