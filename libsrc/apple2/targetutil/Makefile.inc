DEPS += ../libwrk/$(TARGET)/loader.d

../libwrk/$(TARGET)/loader.o: $(SRCDIR)/targetutil/loader.s | ../libwrk/$(TARGET)
	$(ASSEMBLE_recipe)

../targetutil/loader.system: ../libwrk/$(TARGET)/loader.o $(SRCDIR)/targetutil/loader.cfg | ../targetutil
	$(LD65) -o $@ -C $(filter %.cfg,$^) $(filter-out %.cfg,$^)

$(TARGET): ../targetutil/loader.system
