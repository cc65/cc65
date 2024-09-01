#define str(arg) #arg
#include str(bug2458.h)         /* Ok, macro replacement */

#define string foo
#include <string.h>             /* Ok, no macro replacement */

int main()
{
    return 0;
}
