
#include <dbg.h>

int main(void)
{
    /* DbgInit has to be called once, to install the BRK handler */
    DbgInit(0);

    /* now to break into the debugger, use the BREAK macro */
    BREAK();
    return 0;
}
