DEPS += ../libwrk/$(TARGET)/convert.d

../libwrk/$(TARGET)/convert.o: TARGET = apple2enh

../libwrk/$(TARGET)/convert.o: $(SRCDIR)/targetutil/convert.c | ../libwrk/$(TARGET)
	$(COMPILE_recipe)

../lib/apple2enh.lib:
	@$(MAKE) --no-print-directory apple2enh

../targetutil/convert.system: ../libwrk/$(TARGET)/convert.o ../lib/apple2enh.lib | ../targetutil
	$(LD65) -o $@ -C apple2enh-system.cfg $^

$(TARGET): ../targetutil/convert.system
