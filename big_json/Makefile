CC = gcc
CXX = g++
CFLAGS = -Werror -Wall -g -O3 -m64
#CFLAGS = -O0 -ggdb -fsanitize=address
OBJS = $(SRCS:.c=.o)

all: write_json read_json read_json_stream

write_json: write_json.c
	$(CC) $(CFLAGS) $^ -o $@

read_json: read_json.cc
	$(CXX) $(CFLAGS) $^ -o $@

read_json_stream: read_json_stream.cc
	$(CXX) $(CFLAGS) $^ -o $@

clean:
	$(RM) *.o *~ write_json read_json

