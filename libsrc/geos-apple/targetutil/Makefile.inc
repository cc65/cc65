DEPS += ../wrk/$(TARGET)/convert.d

../wrk/$(TARGET)/convert.o: TARGET = apple2enh

../wrk/$(TARGET)/convert.o: $(SRCDIR)/targetutil/convert.c | ../wrk/$(TARGET)
	$(COMPILE_recipe)

../lib/apple2enh.lib:
	@$(MAKE) --no-print-directory apple2enh

../targetutil/convert.system: ../wrk/$(TARGET)/convert.o ../lib/apple2enh.lib | ../targetutil
	$(LD65) -o $@ -C apple2enh-system.cfg $^

$(TARGET): ../targetutil/convert.system
