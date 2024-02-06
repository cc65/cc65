/* Note:
** This is just a sample piece of code that shows how to use some structs -
** it may not even run.
*/


#include <geos.h>

void sysopvfunc (void);
void opvecfunc (void);
void usrfunc (void);

static const dlgBoxStr myDialog = {
        DB_SETPOS (1, 0, 150, 0, 319),
        DB_TXTSTR (10, 20, "test"),
        DB_VARSTR (10, 20, &r0L),
        DB_GETSTR (10, 20, &r0L, 9),
        DB_SYSOPV (sysopvfunc),
        DB_GRPHSTR (&r0L),
        DB_GETFILES (10, 10),
        DB_OPVEC (opvecfunc),
        DB_USRICON (0, 0, &r0L),
        DB_USRROUT (usrfunc),
        DB_ICON (OK, DBI_X_0, DBI_Y_0 ),
        DB_ICON (CANCEL, DBI_X_1, DBI_Y_1),
        DB_END
        };

void main (void)
{
    DoDlgBox (&myDialog);
}
