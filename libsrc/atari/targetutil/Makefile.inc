DEPS += ../wrk/$(TARGET)/w2cas.d

../wrk/$(TARGET)/w2cas.o: $(SRCDIR)/targetutil/w2cas.c | ../wrk/$(TARGET)
	$(COMPILE_recipe)

../targetutil/w2cas.com: ../wrk/$(TARGET)/w2cas.o ../lib/$(TARGET).lib | ../targetutil
	$(LD65) -o $@ -t $(TARGET) $^

$(TARGET): ../targetutil/w2cas.com
