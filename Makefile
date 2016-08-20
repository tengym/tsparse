//CC	= gcc -c -Wall -Werror
CC	= gcc
//CC	= arm-hisiv200-linux-gcc  -c -Wall -Werror -O2
AR	= arm-hisiv200-linux-ar
LD	= arm-hisiv200-linux-ld
RUN	= arm-hisiv200-linux-run
DB	= arm-hisiv200-linux-gdb
LINK    = arm-hisiv200-linux-gcc
CPP     = arm-hisiv200-linux-cpp
CXX     = arm-hisiv200-linux-g++
RANLIB  = arm-hisiv200-linux-ranlib
AS      = arm-hisiv200-linux-as
STRIP   = arm-hisiv200-linux-strip


CFLAGS += -I./tscheck/include -I./bandwidth/include -I./staticfile/include 

LIBPATH = -L./lib/static/

LIB = -Wl,--start-group -static  -lpthread \
        -Wl,--end-group 
		
SOURCE 	= $(wildcard ./tscheck/src/*.c) $(wildcard ./bandwidth/src/*.c)  $(wildcard ./staticfile/src/*.c) $(wildcard ./app/*.c)
APPOBJS 	= $(patsubst %.c,%.o,$(SOURCE))  


APP = hiapp.elf

OBJS = $(APPOBJS)

all:$(APP)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(APP): $(OBJS)
	$(CC) -o $@ $^ $(LIBPATH) $(LIB)
clean:
	rm -f $(OBJS)

	-$(AT)rm -f $(APP)

