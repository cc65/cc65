/*
** char MessageBox (char mode, const char *format, ...)
**
** Maciej 'YTM/Elysium' Witkowiak, 17.08.2003
**
*/

#include <geos.h>
#include <stdio.h>

void _mbprintout(void);

static dlgBoxStr _mbdlg_EMPTY = {
        DB_DEFPOS(1),
        DB_OPVEC(&RstrFrmDialogue),
        DB_USRROUT(&_mbprintout),
        DB_END,
};

static dlgBoxStr _mbdlg_OK = {
        DB_DEFPOS(1),
        DB_USRROUT(&_mbprintout),
        DB_ICON(OK, DBI_X_1, DBI_Y_2),
        DB_END,
};

static dlgBoxStr _mbdlg_OKCANCEL = {
        DB_DEFPOS(1),
        DB_USRROUT(&_mbprintout),
        DB_ICON(OK, DBI_X_0, DBI_Y_2),
        DB_ICON(CANCEL, DBI_X_2, DBI_Y_2),
        DB_END,
};

static dlgBoxStr _mbdlg_YESNO = {
        DB_DEFPOS(1),
        DB_USRROUT(&_mbprintout),
        DB_ICON(YES, DBI_X_0, DBI_Y_2),
        DB_ICON(NO, DBI_X_2, DBI_Y_2),
        DB_END,
};

static dlgBoxStr *_mbboxes[] = {
        &_mbdlg_EMPTY,
        &_mbdlg_OK,
        &_mbdlg_OKCANCEL,
        &_mbdlg_YESNO
};

static char _mbbuffer[256];

char MessageBox(char mode, const char *format, ...)
{
    register char *buf;
    va_list ap;

    /* first format out things */
    va_start(ap, format);
    vsprintf(_mbbuffer, format, ap);
    va_end(ap);

    /* replace LFs by CRs */
    buf = &_mbbuffer[0];
    while (*buf) {
        if (*buf==LF) *buf=CR;
        ++buf;
    }

    /* validate mode */
    if (mode>=MB_LAST)
        mode = MB_EMPTY;

    return DoDlgBox(_mbboxes[mode]);
}

void _mbprintout(void)
{
    UseSystemFont();
    curWindow.top = DEF_DB_TOP;
    curWindow.left = DEF_DB_LEFT+10;
    curWindow.right = DEF_DB_RIGHT-10;
    curWindow.bot = DEF_DB_BOT;
    PutString(_mbbuffer, DEF_DB_TOP+10+curFontDesc.height, DEF_DB_LEFT+10 );
}
