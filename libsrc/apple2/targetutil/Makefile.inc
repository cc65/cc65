DEPS += ../wrk/$(TARGET)/loader.d

../wrk/$(TARGET)/loader.o: $(SRCDIR)/targetutil/loader.s | ../wrk/$(TARGET)
	$(ASSEMBLE_recipe)

../targetutil/loader.system: ../wrk/$(TARGET)/loader.o $(SRCDIR)/targetutil/loader.cfg | ../targetutil
	$(LD65) -o $@ -C $(filter %.cfg,$^) $(filter-out %.cfg,$^)

$(TARGET): ../targetutil/loader.system
