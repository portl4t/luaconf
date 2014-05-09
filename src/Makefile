.SUFFIXES: .c .o .lo

COMPILE = $(CC) -Wall -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -g
INC_PATH = -I/usr/local/include
LIB_PATH = -L/usr/local/lib -lm -ldl -llua-5.1

SHARED_OBJS = luaconf.lo

ALL_OBJS = $(SHARED_OBJS)

ALL_PRGS = 
ALL_LIBS = libluaconf.so
ALL_HEADERS = luaconf.h

all: $(ALL_OBJS) $(ALL_PRGS) $(ALL_LIBS)

libluaconf.so:
	$(COMPILE) -o $@ $< -shared $(SHARED_OBJS) $(LIB_PATH)
.c:
	$(COMPILE) -o $@ $< $(SHARED_OBJS) $(LIB_PATH) $(INC_PATH)
.c.o:
	$(COMPILE) -c -o $@ $<  $(INC_PATH)
.c.lo:
	$(COMPILE) -c -fPIC -o $@ $<  $(INC_PATH)
install:
	/bin/cp -f $(ALL_LIBS) /usr/local/lib
	/bin/cp -f $(ALL_HEADERS) /usr/local/include
clean:
	rm -f $(ALL_OBJS) $(ALL_PRGS) $(ALL_LIBS)

