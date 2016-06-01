DEPS += ../libwrk/$(TARGET)/loader.d

../libwrk/$(TARGET)/loader.o: $(SRCDIR)/targetutil/loader.s | ../libwrk/$(TARGET)
	$(ASSEMBLE_recipe)

../target/$(TARGET)/util/loader.system: ../libwrk/$(TARGET)/loader.o $(SRCDIR)/targetutil/loader.cfg | ../target/$(TARGET)/util
	$(LD65) -o $@ -C $(filter %.cfg,$^) $(filter-out %.cfg,$^)

$(TARGET): ../target/$(TARGET)/util/loader.system
