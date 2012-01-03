/*
 * Minimalistic GEOSLib overlay demo program
 *
 * 2012-01-01, Oliver Schmidt (ol.sc@web.de)
 *
 */


#include <stdio.h>
#include <geos.h>
#include "overlay-demores.h"


/* Functions resident in an overlay can call back functions resident in the
 * main program at any time without any precautions. The function show() is
 * an example for such a function resident in the main program.
 */
void show(char *name)
{
    char line1[40];

    sprintf(line1, CBOLDON "Overlay Demo - Overlay %s" CPLAINTEXT, name);
    DlgBoxOk(line1,
             "Click OK to return to Main.");
}

/* In a real-world overlay program one would probably not use a #pragma but
 * rather place the all the code of certain souce files into the overlay by
 * compiling them with --code-name OVERLAY1.
 */
#pragma code-name(push, "OVERLAY1");

void foo(void)
{
    /* Functions resident in an overlay can access all program variables and
     * constants at any time without any precautions because those are never
     * placed in overlays. The string constant "One" is an example for such 
     * a constant resident in the main program.
     */
    show("One");
}

#pragma code-name(pop);


#pragma code-name(push, "OVERLAY2");

void bar(void)
{
    show("Two");
}

#pragma code-name(pop);


#pragma code-name(push, "OVERLAY3");

void foobar (void)
{
    show("Three");
}

#pragma code-name(pop);


void main(int /*argc*/, char *argv[])
{
    OpenRecordFile(argv[0]);

    DlgBoxOk(CBOLDON "Overlay Demo - Main" CPLAINTEXT,
             "Click OK to call Overlay One.");

    PointRecord(1);

    /* The macro definitions OVERLAY_ADDR and OVERLAY_SIZE were generated in
     * overlay-demores.h by grc65. They contain the overlay area address and
     * size specific to a certain program.
     */
    ReadRecord(OVERLAY_ADDR, OVERLAY_SIZE);

    /* The linker makes sure that the call to foo() ends up at the right mem
     * addr. However it's up to user to make sure that the - right - overlay
     * is actually loaded before making the the call.
     */
    foo();

    DlgBoxOk(CBOLDON "Overlay Demo - Main" CPLAINTEXT,
             "Click OK to call Overlay Two.");

    PointRecord(2);

    /* Replacing one overlay with another one can only happen from the main
     * program. This implies that an overlay can never load another overlay.
     */
    ReadRecord(OVERLAY_ADDR, OVERLAY_SIZE);

    bar();

    DlgBoxOk(CBOLDON "Overlay Demo - Main" CPLAINTEXT,
             "Click OK to call Overlay Three.");

    PointRecord(3);

    ReadRecord(OVERLAY_ADDR, OVERLAY_SIZE);

    foobar();

    CloseRecordFile();
}
