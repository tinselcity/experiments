CC = gcc
#CC = clang
CFLAGS = -Werror -Wall -g -O2
#CFLAGS = -Wall -Werror -O0 -ggdb -fsanitize=address
INCLUDES = -I./
LFLAGS = -L./
#DEBUG_LIBS = -lasan
LIBS = -luring -ltcmalloc
SRCS = ndebug.c ext/http-parser/http_parser.c mime.c date.c
OBJS = $(SRCS:.c=.o)

.PHONY: depend clean

all: hignx_epoll hignx_uring

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

hignx_epoll: hignx_epoll.c $(OBJS)
	$(CC) $(DEBUG_LIBS) $(CFLAGS) $(INCLUDES) -o $@ hignx_epoll.c $(OBJS) $(LFLAGS)

hignx_uring: hignx_uring.c $(OBJS)
	$(CC) $(DEBUG_LIBS) $(CFLAGS) $(INCLUDES) -o $@ hignx_uring.c $(OBJS) -luring

clean:
	$(RM) *.o *~ hignx_epoll hignx_uring

depend: $(SRCS)
	makedepend $(INCLUDES) $^
