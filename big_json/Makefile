CC = gcc
CXX = g++
CFLAGS = -Werror -Wall -g -O3 -m64 
#CFLAGS = -O0 -ggdb -fsanitize=address
INCLUDES = -I./
LFLAGS = -L./
LIBS = /usr/local/lib/libsimdjson.a
OBJS = $(SRCS:.c=.o)

all: write_json read_json

write_json: write_json.c
	$(CC) $(CFLAGS) $(INCLUDES) $(LFLAGS) $^ -o $@

read_json: read_json.cc
	$(CXX) $(CFLAGS) $(INCLUDES) $(LFLAGS) $(LIBS) $^ -o $@ $(LIBS) 


read_json_stream: read_json_stream.cc
	$(CXX) $(CFLAGS) $(INCLUDES) $(LFLAGS) $(LIBS) $^ -o $@ $(LIBS) 

clean:
	$(RM) *.o *~ write_json read_json

