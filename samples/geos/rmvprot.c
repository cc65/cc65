/*
    GEOSLib example

    This small application removes GEOS disk write protection tag.
    e.g. boot disk is always protected after boot-up

    Maciej 'YTM/Elysium' Witkowiak
    <ytm@elysium.pl>

    21.03.2000
*/


#include <geos.h>

char diskName[17] = "";

static const graphicStr clearScreen = {
        MOVEPENTO(0, 0),
        NEWPATTERN(2),
        RECTANGLETO(SC_PIX_WIDTH-1, SC_PIX_HEIGHT-1),
        GSTR_END
};

static const dlgBoxStr mainDialog = {
        DB_DEFPOS(1),
        DB_TXTSTR(TXT_LN_X, TXT_LN_2_Y,
                  CBOLDON "Remove protection on:" CPLAINTEXT),
        DB_TXTSTR(TXT_LN_X, TXT_LN_3_Y, diskName),
        DB_ICON(OK, DBI_X_0, DBI_Y_2),
        DB_ICON(DISK, DBI_X_1, DBI_Y_2),
        DB_ICON(CANCEL, DBI_X_2, DBI_Y_2),
        DB_END
};

static const dlgBoxStr changeDiskDlg = {
        DB_DEFPOS(1),
        DB_TXTSTR(TXT_LN_X, TXT_LN_2_Y, CBOLDON "Insert new disk"),
        DB_TXTSTR(TXT_LN_X, TXT_LN_3_Y, "into drive." CPLAINTEXT),
        DB_ICON(OK, DBI_X_0, DBI_Y_2),
        DB_ICON(CANCEL, DBI_X_2, DBI_Y_2),
        DB_END
};

static const dlgBoxStr errorDialog = {
        DB_DEFPOS(1),
        DB_TXTSTR(TXT_LN_X, TXT_LN_2_Y, CBOLDON "Error happened..."),
        DB_TXTSTR(TXT_LN_X, TXT_LN_3_Y, "exiting..." CPLAINTEXT),
        DB_ICON(OK, DBI_X_0, DBI_Y_2),
        DB_END
};

void Error(void)
{
    DoDlgBox(&errorDialog);
    EnterDeskTop();
}

void main(void)
{
    // Here we clear the screen. Not really needed anyway...
    GraphicsString(&clearScreen);

    // Get the name of current disk to show it in dialog box
    GetPtrCurDkNm(diskName);

    while (1) {
        switch (DoDlgBox(&mainDialog)) {

        // What's the result of dialog box? which icon was pressed?
        case OK:
            if (GetDirHead())
                Error();
            curDirHead[OFF_GS_DTYPE] = 0;
            if (PutDirHead())
                Error();
            break;
        case DISK:
            DoDlgBox(&changeDiskDlg);
            GetPtrCurDkNm(diskName);
            break;
        default:    // CANCEL is the third option
            return;
            break;
        }
    }
}
