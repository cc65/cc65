#
# Makefile for GEOS lib
# for cc65
#
# Maciej 'YTM/Alliance' Witkowiak

export CC	= ../../../src/cc65
export CFLAGS	= -O
export AS      	= ../../../src/ca65/ca65
export ASFLAGS	=
AR      	= ../../src/ar65/ar65


OBJ_DIRS=disk dlgbox file graph menuicon memory mousesprite process system

all:
	@for i in devel $(OBJ_DIRS); do $(MAKE) -C $$i; done
	@mv devel/crt0.o ../geos.o
	@for i in $(OBJ_DIRS); do $(AR) a ../geos.lib $$i/*.o; done

rebuild: zap all clean


.PHONY: clean
clean:
	@for i in $(OBJ_DIRS); do \
	    cd $$i;				\
	    $(MAKE) clean;			\
	    cd ..;				\
	done

.PHONY: zap
zap:	clean
	@rm -f ../geos.lib ../geos.o

