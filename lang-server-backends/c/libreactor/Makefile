PROG    = libreactor-ffead-cpp
CFLAGS  = -std=gnu11 -Wall -Wunused-result -Wextra -Wpedantic -O3
LDFLAGS = -pthread
LDADD   = -lreactor -ldynamic -flto -lffead-framework

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

$(PROG): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LDADD)

clean:
	rm -f $(PROG) $(OBJS)