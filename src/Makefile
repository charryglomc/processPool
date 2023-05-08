SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
ELF:=process_pool
CC:=gcc
CFLAGS:=-g -Wall 
$(ELF):$(OBJS)
		gcc -o $@ $^ -pthread 
clean:
		rm -rf $(OBJS) $(ELF)

