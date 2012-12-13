CC	 	:=	gcc

TARGET		:=	matmul
MAIN		:=	main.c
SOURCES 	:=	$(wildcard source/*.c)
INCLUDE		:= 	./include
OBJECTS		:=	$(SOURCES:.c=.o)
DEPS		:=	$(OBJECTS:.o=.d)
CFLAGS 		:= 	-Wall -pedantic -std=c99  -I$(INCLUDE) -Wvariadic-macros 
LIBS		:=	-lm -lpthread
CPUCOUNT	:=	$(shell cat /proc/cpuinfo | grep processor | wc -l)

CFLAGS		+= 	-D__NCORES__=$(CPUCOUNT)

.PHONY:		clean depend .depend

all: clean matmul

-include $(DEPS)

%.o:	%.c
	@echo Compiling $<...
	$(CC) -c -g $(CFLAGS) -MMD -o $@ $<

matmul:		$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LIBS) 

clean:
	rm $(OBJECTS) $(DEPS) $(TARGET)
