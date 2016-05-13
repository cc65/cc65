DEPS += ../libwrk/$(TARGET)/w2cas.d

../libwrk/$(TARGET)/w2cas.o: $(SRCDIR)/targetutil/w2cas.c | ../libwrk/$(TARGET)
	$(COMPILE_recipe)

../goodies/targetutil/w2cas.com: ../libwrk/$(TARGET)/w2cas.o ../lib/$(TARGET).lib | ../goodies/targetutil
	$(LD65) -o $@ -t $(TARGET) $^

$(TARGET): ../goodies/targetutil/w2cas.com
