DEPS += ../libwrk/$(TARGET)/w2cas.d

../libwrk/$(TARGET)/w2cas.o: $(SRCDIR)/targetutil/w2cas.c | ../libwrk/$(TARGET)
	$(COMPILE_recipe)

../target/$(TARGET)/util/w2cas.com: ../libwrk/$(TARGET)/w2cas.o ../lib/$(TARGET).lib | ../target/$(TARGET)/util
	$(LD65) -o $@ -t $(TARGET) $^

$(TARGET): ../target/$(TARGET)/util/w2cas.com
