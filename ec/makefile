CC = g++
CFLAGS = -Wall -Wextra -std=c++11

TARGETS = echo-client

all: $(TARGETS)

echo-client: echo-client.cpp
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)
