#
# Makefile for GEOS lib
# for cc65
#
# Maciej 'YTM/Elysium' Witkowiak

OBJ_DIRS=common conio devel disk dlgbox file graph menuicon memory mousesprite process runtime system

all:
	@for i in $(OBJ_DIRS); do $(MAKE) -C $$i; done
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
	@$(RM) ../geos.lib 

