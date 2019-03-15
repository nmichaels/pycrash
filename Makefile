CC=gcc
CFLAGS=-Wall -Werror -std=gnu11 -Wextra -g -O3

all: pycrash

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: clean

clean:
	rm -f pycrash
