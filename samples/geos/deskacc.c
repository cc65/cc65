/*
** Desk accessory example, replicating the behaviour of the sample desk
** accessory shipped with GeoProgrammer
**
** 2026, Asbjørn Djupdal (asbjoern@djupdal.org)
*/

#include <stdbool.h>
#include <geos.h>

#include "deskaccres.h"

/*****************************************************************************/
/* defines */

/* screen region this accessory uses, in cards */
#define DA_TOP      8
#define DA_LEFT     10
#define DA_HEIGHT   8
#define DA_WIDTH    20

#define NUM_CARDS   (DA_HEIGHT * DA_WIDTH)

#define COLOR_MATRIX_BASE ((char *) 0x8c00)

#define CARD_ADDR(base, cardRow, cardCol) \
    ((base) + (unsigned) (cardRow) * (40 * 8) + (unsigned) (cardCol) * 8)

/*****************************************************************************/
/* variables */

static char recoverFlag;
static char colorBuf[NUM_CARDS];
static char foreBuf[NUM_CARDS * 8];
static char backBuf[NUM_CARDS * 8];

/*****************************************************************************/
/* icon */

static const unsigned char iconPicture[] = {
    0x06, 0xFF, 0x81, 0x80, 0x04, 0x00, 0x82, 0x01,
    0x80, 0x04, 0x00, 0x82, 0x01, 0x80, 0x04, 0x00,
    0x82, 0x01, 0x80, 0x04, 0x00, 0x82, 0x01, 0x80,
    0x04, 0x00, 0xBE, 0x01, 0x80, 0x0E, 0x00, 0x00,
    0x00, 0x01, 0x80, 0x04, 0x00, 0x00, 0x00, 0x01,
    0x80, 0x04, 0x00, 0x00, 0x00, 0x01, 0x80, 0x04,
    0x38, 0x63, 0x60, 0x01, 0x80, 0x04, 0x4C, 0x91,
    0x90, 0x01, 0x80, 0x04, 0x41, 0x09, 0x10, 0x01,
    0x80, 0x04, 0x41, 0x09, 0x10, 0x01, 0x80, 0x04,
    0x41, 0x09, 0x10, 0x01, 0x80, 0x04, 0x44, 0x91,
    0x10, 0x01, 0x80, 0x0E, 0x38, 0x63, 0xB8, 0x01,
    0x80, 0x04, 0x00, 0x82, 0x01, 0x80, 0x04, 0x00,
    0x82, 0x01, 0x80, 0x04, 0x00, 0x82, 0x01, 0x80,
    0x04, 0x00, 0x82, 0x01, 0x80, 0x04, 0x00, 0x82,
    0x01, 0x80, 0x04, 0x00, 0x81, 0x01, 0x06, 0xFF
};

void DoIcon1 (void);

static const struct icontab icon = {
    1,
    { DA_LEFT * 8, DA_TOP * 8 },
    {
        { (char *) iconPicture, DA_LEFT + 3, DA_TOP * 8 + 24, 6, 23,
          (unsigned) DoIcon1 }
    }
};

/*****************************************************************************/
/* framed box */

static const graphicStr drawBox = {
    NEWPATTERN (0),                       /* white */
    MOVEPENTO (DA_LEFT * 8, DA_TOP * 8),
    RECTANGLETO ((DA_LEFT + DA_WIDTH) * 8 - 1, (DA_TOP + DA_HEIGHT) * 8 - 1),
    NEWPATTERN (1),                       /* black */
    FRAME_RECTO (DA_LEFT * 8, DA_TOP * 8),
    GSTR_END
};

/*****************************************************************************/
/* functions */

/* copy this accessory's screen area, one card at a time, between the given
** screen bitmap and the given RAM buffer.
*/
static void CopyBitmap (char *screen, char *buf, bool toBuf)
{
    char row, col, i;
    char *card;

    for (row = 0; row < DA_HEIGHT; ++row) {
        for (col = 0; col < DA_WIDTH; ++col) {
            card = CARD_ADDR (screen, DA_TOP + row, DA_LEFT + col);
            for (i = 0; i < 8; ++i) {
                if (toBuf) {
                    *buf++ = card[i];
                } else {
                    card[i] = *buf++;
                }
            }
        }
    }
}

/* save screen buffers */
static void SaveScreen (void)
{
    CopyBitmap (BACK_SCR_BASE, backBuf, true);

    if (recoverFlag & 0x80) {
        CopyBitmap (SCREEN_BASE, foreBuf, true);
    }
}

/* restore screen buffers */
static void RestoreScreen (void)
{
    CopyBitmap (BACK_SCR_BASE, backBuf, false);

    if (recoverFlag & 0x80) {
        CopyBitmap (SCREEN_BASE, foreBuf, false);
    }
}

/* save color values */
static void SaveColors (void)
{
    char row, col, fillColor;

    fillColor = COLOR_MATRIX_BASE[39];

    for (row = 0; row < DA_HEIGHT; ++row) {
        for (col = 0; col < DA_WIDTH; ++col) {
            if (recoverFlag & 0x40) {
                colorBuf[row * DA_WIDTH + col] =
                    COLOR_MATRIX_BASE[(DA_TOP + row) * 40 + DA_LEFT + col];
            }
            COLOR_MATRIX_BASE[(DA_TOP + row) * 40 + DA_LEFT + col] = fillColor;
        }
    }
}

/* restore color values */
static void RestoreColors (void)
{
    char row, col;

    for (row = 0; row < DA_HEIGHT; ++row) {
        for (col = 0; col < DA_WIDTH; ++col) {
            COLOR_MATRIX_BASE[(DA_TOP + row) * 40 + DA_LEFT + col] =
                colorBuf[row * DA_WIDTH + col];
        }
    }
}

/*****************************************************************************/
/* handlers */

void DoClose (void)
{
    GotoFirstMenu ();
}

void DoCut (void)
{
    GotoFirstMenu ();
}

void DoCopy (void)
{
    GotoFirstMenu ();
}

void DoPaste (void)
{
    GotoFirstMenu ();
}

void DoIcon1 (void)
{
}

void DoQuit (void)
{
    GotoFirstMenu ();
    if (recoverFlag & 0x40) {
        RestoreColors ();
    }
    RestoreScreen ();
    RstrAppl ();
}

/*****************************************************************************/

void main (void)
{
    recoverFlag = r10L;

    SaveScreen ();
    SaveColors ();

    dispBufferOn = ST_WR_FORE | ST_WR_BACK;

    GraphicsString (&drawBox);
    DoMenu ((struct menu *) &mainMenu);
    DoIcons ((struct icontab *) &icon);

    MainLoop ();
}
