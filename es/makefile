CC = g++
CFLAGS = -Wall -Wextra -std=c++11

TARGETS = echo-server

all: $(TARGETS)

echo-server: echo-server.cpp
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)