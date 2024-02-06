/* Note:
** This is just a sample piece of code that shows how to use some structs -
** it may not even run.
*/


#include <geos.h>


/* prototypes are necessary */

void smenu1 (void);
void smenu2 (void);
void smenu3 (void);

typedef void menuString;

/* you can declare a menu using cc65 non-ANSI extensions */

static const menuString subMenu1 = {
        (char)0, (char)(3*15),
        (unsigned)0, (unsigned)50,
        (char)(3 | VERTICAL),
        "subitem1", (char)MENU_ACTION, (unsigned)smenu1,
        "subitem2", (char)MENU_ACTION, (unsigned)smenu2,
        "subitem3", (char)MENU_ACTION, (unsigned)smenu3
 };

/* or by using initialized structures */

static struct menu subMenu2 = {
        { 0, 3*15, 0, 50 },
        3 | VERTICAL,
          {
            { "subitem1", MENU_ACTION, smenu1 },
            { "subitem2", MENU_ACTION, smenu2 },
            { "subitem3", MENU_ACTION, smenu3 },
          }
 };

void main (void)
{
    DoMenu(&subMenu1);
    DoMenu(&subMenu2);
}
