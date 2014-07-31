CFG_QUITE ?= 1
CFG_DEBUG ?= 1
CFG_DEBUG_MEM ?= 1

#CFLAGS 
CC ?= gcc

OBJS = mem.o test.o

ifeq ($(strip $(CFG_DEBUG)),1)
	CFLAGS += -DDEBUG -g -Wall
endif

ifeq ($(strip $(CFG_DEBUG_MEM)),1)
	CFLAGS += -DDEBUG_MEM
endif

Q = 
ifeq ($(strip $(CFG_QUITE)),1)
Q = @
endif

all:$(OBJS)
	@echo "[ LD $^ --> test.out	]"
	$(Q)$(CC) $^ -o test.out

$(filter %.o,$(OBJS)):%.o:%.c
	@echo "[ CC $^	--> $@	]"
	$(Q)$(CC) -c $(CFLAGS) $^ -o $@

.PHONY:clean
clean:
	@echo "Cleaning..."
	@-rm -rf *.o test.out
