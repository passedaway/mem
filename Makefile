CFG_DEBUG ?= 1

#CFLAGS 
CC ?= gcc

OBJS = mem.o test.o

ifeq ($(strip $(CFG_DEBUG)),1)
	CFLAGS += -DDEBUG -g -Wall
endif

all:$(OBJS)
	@echo "[ LD $^ --> test.out	]"
	@$(CC) $^ -o test.out

$(filter %.o,$(OBJS)):%.o:%.c
	@echo "[ CC $^	--> $@	]"
	@$(CC) -c $(CFLAGS) $^ -o $@

.PHONY:clean
clean:
	@-rm -rf *.o test.out
