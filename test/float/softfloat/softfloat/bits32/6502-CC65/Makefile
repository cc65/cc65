
PROCESSOR_H = ../../../processors/6502-CC65.h
SOFTFLOAT_MACROS = ../softfloat-macros

TARGET=-t sim6502

OBJ = .o
EXE =
INCLUDES = -I. -I..
COMPILE_C = ../../../../../../bin/cl65 $(TARGET) -c -o $@ $(INCLUDES) -I- -O
COMPILE_ONLY = ../../../../../../bin/cl65 $(TARGET) --add-source -S -o $@.s $(INCLUDES) -I- -O
LINK = ../../../../../../bin/cl65 $(TARGET) -o $@

#------------------------------------------------------------------------------
# Probably okay below here.
#------------------------------------------------------------------------------

ALL: softfloat$(OBJ) timesoftfloat$(EXE)

milieu.h: $(PROCESSOR_H)
	touch milieu.h

softfloat$(OBJ): milieu.h softfloat.h softfloat-specialize $(SOFTFLOAT_MACROS) ../softfloat.c
	$(COMPILE_C) ../softfloat.c

timesoftfloat$(OBJ): milieu.h softfloat.h ../timesoftfloat.c
	$(COMPILE_ONLY) ../timesoftfloat.c
	$(COMPILE_C) ../timesoftfloat.c

timesoftfloat$(EXE): softfloat$(OBJ) timesoftfloat$(OBJ)
	$(LINK) softfloat$(OBJ) timesoftfloat$(OBJ)

clean:
	$(RM) softfloat$(OBJ) timesoftfloat$(EXE) timesoftfloat$(OBJ) timesoftfloat$(OBJ).s
