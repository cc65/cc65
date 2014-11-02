/* Note:
** This is just a sample piece of code that shows how to use some structs -
** it may not even run.
*/


#include <geos.h>

static const graphicStr myString = {
        MOVEPENTO (0, 0),
        LINETO(100, 100),
        RECTANGLETO(50, 50),
        NEWPATTERN(3),
        FRAME_RECTO(50, 50),
        PEN_X_DELTA(10),
        PEN_Y_DELTA(10),
        PEN_XY_DELTA(10, 10),
        GSTR_END
        };

int main (void)
{
    GraphicsString(&myString);
}

