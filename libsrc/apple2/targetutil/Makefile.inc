DEPS += ../libwrk/$(TARGET)/convert.d \
        ../libwrk/$(TARGET)/loader.d

../libwrk/$(TARGET)/convert.o: $(SRCDIR)/targetutil/convert.c | ../libwrk/$(TARGET)
	$(COMPILE_recipe)

../libwrk/$(TARGET)/loader.o: $(SRCDIR)/targetutil/loader.s | ../libwrk/$(TARGET)
	$(ASSEMBLE_recipe)

../target/$(TARGET)/util/convert.system: ../libwrk/$(TARGET)/convert.o ../lib/$(TARGET).lib | ../target/$(TARGET)/util
	$(LD65) -o $@ -C $(TARGET)-system.cfg $^

../target/$(TARGET)/util/loader.system: ../libwrk/$(TARGET)/loader.o $(SRCDIR)/targetutil/loader.cfg | ../target/$(TARGET)/util
	$(LD65) -o $@ -C $(filter %.cfg,$^) $(filter-out %.cfg,$^)

$(TARGET): ../target/$(TARGET)/util/convert.system \
           ../target/$(TARGET)/util/loader.system
