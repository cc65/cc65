ifeq ($(TARGET),atari)

DEPS += ../wrk/$(TARGET)/w2cas.d

../wrk/$(TARGET)/w2cas.o: CC65FLAGS:=-O -W error
../wrk/$(TARGET)/w2cas.o: $(SRCDIR)/targetutil/w2cas.c | ../wrk/$(TARGET)
	$(COMPILE_recipe)

../targetutil/W2CAS.COM: ../wrk/$(TARGET)/w2cas.o ../lib/$(TARGET).lib | ../targetutil
	$(LD65) -o $@ -t $(TARGET) $^

$(TARGET): ../targetutil/W2CAS.COM

endif
