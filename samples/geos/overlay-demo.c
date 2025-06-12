/*
** Minimalistic GEOSLib overlay demo program
**
** 2012-01-01, Oliver Schmidt (ol.sc@web.de)
**
*/


#include <stdio.h>
#include <geos.h>
#include "overlay-demores.h"


/* Functions resident in an overlay can call back functions resident in the
** main program at any time without any precautions. The function show() is
** an example for such a function resident in the main program.
*/
void show(char *name)
{
    char line1[40];

    sprintf(line1, CBOLDON "Overlay Demo - Overlay %s" CPLAINTEXT, name);
    DlgBoxOk(line1, "Click OK to return to Main.");
}

/* In a real-world overlay program one would probably not use a #pragma but
** rather place the all the code of certain source files into the overlay by
** compiling them with --code-name OVERLAY1.
*/
#pragma code-name(push, "OVERLAY1")

void foo(void)
{
    /* Functions resident in an overlay can access all program variables and
    ** constants at any time without any precautions because those are never
    ** placed in overlays. The string constant "One" is an example for such
    ** a constant resident in the main program.
    */
    show("One");
}

#pragma code-name(pop)


#pragma code-name(push, "OVERLAY2")

void bar(void)
{
    show("Two");
}

#pragma code-name(pop)


#pragma code-name(push, "OVERLAY3")

void foobar (void)
{
    show("Three");
}

#pragma code-name(pop)


void main(int /*argc*/, char *argv[])
{
    if (OpenRecordFile(argv[0])) {
        _poserror("OpenRecordFile");
        return;
    }

    DlgBoxOk(CBOLDON "Overlay Demo - Main" CPLAINTEXT,
             "Click OK to call Overlay One.");

    if (PointRecord(1)) {
        _poserror("PointRecord.1");
        return;
    }

    /* The macro definitions OVERLAY_ADDR and OVERLAY_SIZE were generated in
    ** overlay-demores.h by grc65. They contain the overlay area address and
    ** size specific to a certain program.
    */
    if (ReadRecord(OVERLAY_ADDR, OVERLAY_SIZE)) {
        _poserror("ReadRecord.1");
        return;
    }

    /* The linker makes sure that the call to foo() ends up at the right mem
    ** addr. However, it's up to user to make sure that the -- right -- overlay
    ** actually is loaded before making the call.
    */
    foo();

    DlgBoxOk(CBOLDON "Overlay Demo - Main" CPLAINTEXT,
             "Click OK to call Overlay Two.");

    if (PointRecord(2)) {
        _poserror("PointRecord.2");
        return;
    }

    /* Replacing one overlay with another one can only happen from the main
    ** program. This implies that an overlay can never load another overlay.
    */
    if (ReadRecord(OVERLAY_ADDR, OVERLAY_SIZE)) {
        _poserror("ReadRecord.2");
        return;
    }

    bar();

    DlgBoxOk(CBOLDON "Overlay Demo - Main" CPLAINTEXT,
             "Click OK to call Overlay Three.");

    if (PointRecord(3)) {
        _poserror("PointRecord.3");
        return;
    }

    if (ReadRecord(OVERLAY_ADDR, OVERLAY_SIZE)) {
        _poserror("ReadRecord.3");
        return;
    }

    foobar();

    if (CloseRecordFile()) {
        _poserror("CloseRecordFile");
        return;
    }
}
