CFG_DEBUG ?= 1

#CFLAGS 
CC ?= gcc

OBJS = mem.o main.o

ifeq ($(strip $(CFG_DEBUG)),1)
	CFLAGS += -DDEBUG -g
endif

all:$(OBJS)
	@echo "[ LD $^ --> test	]"
	@$(CC) $^ -o test

$(filter %.o,$(OBJS)):%.o:%.c
	@echo "[ CC $^	--> $@	]"
	@$(CC) -c $(CFLAGS) $^ -o $@

.PHONY:clean
clean:
	@-rm -rf *.o test
