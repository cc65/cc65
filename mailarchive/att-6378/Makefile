sources = foo.c bar.c

.PHONY: all
all: foobar

ifneq ($(MAKECMDGOALS),clean)
-include $(sources:.c=.d)
endif

clean:
	rm -f $(sources:.c=.d) $(sources:.c=.o) foobar

%.d: %.c
ifndef CC65_TARGET
	@set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
endif

%.o: %.c
ifndef CC65_TARGET
	$(CC) -c $(CFLAGS) $< -o $@
else
	$(CC65_HOME)/bin/cc65 -t $(CC65_TARGET) $(CFLAGS) -I $(CC65_HOME)/include --create-dep $< -o $(@:.o=.s)
	@$(CC65_HOME)/bin/ca65 -t $(CC65_TARGET) $(ASFLAGS) $(@:.o=.s) -o $@; \
	sed 's!.s:!.o:!' < $(@:.o=.u) > $(@:.o=.d); \
	rm -f $(@:.o=.s) $(@:.o=.u)
endif

foobar: $(sources:.c=.o)
ifndef CC65_TARGET
	$(CC) $(LDFLAGS) $^ -o $@
else
	$(CC65_HOME)/bin/ld65 -t $(CC65_TARGET) $(LDFLAGS) $(CC65_HOME)/lib/$(CC65_TARGET).o --start-group $(CC65_HOME)/lib/$(CC65_TARGET).lib $^ --end-group -o $@
endif
