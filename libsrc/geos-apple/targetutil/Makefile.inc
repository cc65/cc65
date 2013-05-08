../wrk/$(TARGET)/convert.o: TARGET = apple2enh

../wrk/$(TARGET)/convert.o: $(SRCDIR)/targetutil/convert.c | ../wrk/$(TARGET)
	$(COMPILE_recipe)

../targetutil/convert.system: ../wrk/$(TARGET)/convert.o | ../targetutil
	$(LD) -o $@ -C apple2enh-system.cfg $^ apple2enh.lib

$(TARGET): ../targetutil/convert.system
