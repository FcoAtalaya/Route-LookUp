SRC = my_route_lookup.c my_route_lookup.h io.c io.h utils.c utils.h main.c
CFLAGS = -Wall -O3

all: my_route_lookup

my_route_lookup: $(SRC)
	gcc $(CFLAGS) $(SRC) -g -o my_route_lookup -lm

.PHONY: clean

clean:
	rm -f my_route_lookup

#RL Lab 2020 Switching UC3M
