/*
** dbg.c
**
** Ullrich von Bassewitz, 08.08.1998
**
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <6502.h>
#include <dbg.h>



/*****************************************************************************/
/*                             Function forwards                             */
/*****************************************************************************/



/* Forwards for handler functions */
static char AsmHandler (void);
static char RegHandler (void);
static char StackHandler (void);
static char CStackHandler (void);
static char DumpHandler (void);
static char HelpHandler (void);

/* Forwards for other functions */
static void DisplayPrompt (char* s);
static void SingleStep (char StepInto);
static void RedrawStatic (char  Frame);
static void Redraw (char Frame);
static char GetKeyUpdate (void);



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Color definitions */
#if defined(__C16__)
#  define COLOR_BORDER          (BCOLOR_DARKBLUE | CATTR_LUMA6)
#  define COLOR_BACKGROUND      COLOR_WHITE
#  define COLOR_TEXTHIGH        COLOR_BLACK
#  define COLOR_TEXTLOW         COLOR_GRAY1
#  define COLOR_FRAMEHIGH       COLOR_BLACK
#  define COLOR_FRAMELOW        COLOR_GRAY2
#else
#  if defined(COLOR_GRAY3)
#    define COLOR_BORDER        COLOR_BLACK
#    define COLOR_BACKGROUND    COLOR_BLACK
#    define COLOR_TEXTHIGH      COLOR_WHITE
#    define COLOR_TEXTLOW       COLOR_GRAY3
#    define COLOR_FRAMEHIGH     COLOR_WHITE
#    define COLOR_FRAMELOW      COLOR_GRAY3
#  else
#    if defined(__APPLE2__)
#      define COLOR_BORDER      COLOR_BLACK
#      define COLOR_BACKGROUND  COLOR_BLACK
#      define COLOR_TEXTHIGH    COLOR_BLACK
#      define COLOR_TEXTLOW     COLOR_BLACK
#      define COLOR_FRAMEHIGH   COLOR_BLACK
#      define COLOR_FRAMELOW    COLOR_BLACK
#    else
#      define COLOR_BORDER      COLOR_BLACK
#      define COLOR_BACKGROUND  COLOR_BLACK
#      define COLOR_TEXTHIGH    COLOR_WHITE
#      define COLOR_TEXTLOW     COLOR_WHITE
#      define COLOR_FRAMEHIGH   COLOR_WHITE
#      define COLOR_FRAMELOW    COLOR_WHITE
#    endif
#  endif
#endif
#ifndef COLOR_BLACK
#  define COLOR_BLACK   0
#endif
#ifndef COLOR_WHITE
#  define COLOR_WHITE   1
#endif

/* Screen definitions */
#if defined(__CBM610__)
#  define BIGSCREEN
#  define MAX_X         80
#  define MAX_Y         25
#  define DUMP_BYTES    16
#elif defined(__APPLE2__) || defined(__ATARI__)
#  define MAX_X         40
#  define MAX_Y         24
#  define DUMP_BYTES     8
#else
#  define MAX_X         40
#  define MAX_Y         25
#  define DUMP_BYTES     8
#endif

/* Replacement key definitions */
#ifndef CH_DEL
#  define CH_DEL        ('H' - 'A' + 1)         /* Ctrl+H */
#endif

/* Replacement char definitions */
#ifndef CH_ULCORNER
#  define CH_ULCORNER   '+'
#endif
#ifndef CH_URCORNER
#  define CH_URCORNER   '+'
#endif
#ifndef CH_LLCORNER
#  define CH_LLCORNER   '+'
#endif
#ifndef CH_LRCORNER
#  define CH_LRCORNER   '+'
#endif
#ifndef CH_TTEE
#  define CH_TTEE       '+'
#endif
#ifndef CH_LTEE
#  define CH_LTEE       '+'
#endif
#ifndef CH_RTEE
#  define CH_RTEE       '+'
#endif
#ifndef CH_BTEE
#  define CH_BTEE       '+'
#endif
#ifndef CH_CROSS
#  define CH_CROSS      '+'
#endif

/* Defines for opcodes */
#define OPC_BRK         0x00
#define OPC_BPL         0x10
#define OPC_JSR         0x20
#define OPC_BMI         0x30
#define OPC_RTI         0x40
#define OPC_JMP         0x4C
#define OPC_BVC         0x50
#define OPC_RTS         0x60
#define OPC_JMPIND      0x6C
#define OPC_BVS         0x70
#define OPC_BCC         0x90
#define OPC_BCS         0xB0
#define OPC_BNE         0xD0
#define OPC_BEQ         0xF0



/* Register values that are used also in the assembler stuff */
extern unsigned char DbgSP;             /* Stack pointer */
extern unsigned      DbgCS;             /* C stack pointer */
extern unsigned      DbgHI;             /* High 16 bit of primary reg */



/* Descriptor for one text line */
typedef struct {
    unsigned char x;
    unsigned char y;
    char*         text;
} TextDesc;

/* Window descriptor */
typedef struct {
    unsigned char fd_tl;                /* Top left char */
    unsigned char fd_tr;                /* Top right char */
    unsigned char fd_bl;                /* Bottom left char */
    unsigned char fd_br;                /* Bottom right char */
    unsigned char fd_x1, fd_y1;         /* Upper left corner */
    unsigned char fd_x2, fd_y2;         /* Lower right corner */
    unsigned char fd_width, fd_height;  /* Redundant but faster */
    unsigned char fd_visible;           /* Is the window currently visible? */
    char (*fd_func) (void);             /* Handler function */
    unsigned char fd_textcount;         /* Number of text lines to print */
    TextDesc*     fd_text;              /* Static text in the window */
} FrameDesc;



/* Texts for the windows */
static TextDesc RegText [] = {
    { 1,  0, "PC" },
    { 1,  1, "SR" },
    { 1,  2, "A"  },
    { 1,  3, "X"  },
    { 1,  4, "Y"  },
    { 1,  5, "SP" },
    { 1,  6, "CS" },
    { 1,  7, "HI" }
};
static TextDesc HelpText [] = {
    { 1,  0, "F1, ?     Help"                           },
    { 1,  1, "F2, t     Toggle breakpoint"              },
    { 1,  2, "F3, u     Run until subroutine returns"   },
    { 1,  3, "F4, h     Run to cursor"                  },
    { 1,  4, "F7, space Step into"                      },
    { 1,  5, "F8, enter Step over"                      },
    { 1,  6, "1-5       Select active window"           },
    { 1,  7, "+         Page down"                      },
    { 1,  8, "-         Page up"                        },
    { 1,  9, "Cursor    Move up/down"                   },
    { 1, 10, "a/z       Move up/down"                   },
    { 1, 11, "c         Continue"                       },
    { 1, 12, "f         Follow instruction"             },
    { 1, 13, "o         Goto origin"                    },
    { 1, 14, "p         Use as new PC value"            },
    { 1, 15, "q         Quit"                           },
    { 1, 16, "r         Redraw screen"                  },
    { 1, 17, "s         Skip next instruction"          },
};


/* Window data */
static FrameDesc AsmFrame = {
    CH_ULCORNER, CH_TTEE, CH_LTEE, CH_CROSS,
    0, 0, MAX_X - 10, 15,
    MAX_X - 11, 14,
    1,
    AsmHandler,
    0, 0
};
static FrameDesc RegFrame = {
    CH_TTEE, CH_URCORNER, CH_LTEE, CH_RTEE,
    MAX_X - 10, 0, MAX_X - 1, 9,
    8, 8,
    1,
    RegHandler,
    sizeof (RegText) / sizeof (RegText [0]), RegText
};
static FrameDesc StackFrame = {
    CH_LTEE, CH_RTEE, CH_CROSS, CH_RTEE,
    MAX_X - 10, 9, MAX_X - 1, 15,
    8, 5,
    1,
    StackHandler,
    0, 0
};
static FrameDesc CStackFrame = {
    CH_CROSS, CH_RTEE, CH_BTEE, CH_LRCORNER,
    MAX_X - 10, 15, MAX_X - 1, MAX_Y - 1,
    8, MAX_Y - 17,
    1,
    CStackHandler,
    0, 0
};
static FrameDesc DumpFrame = {
    CH_LTEE, CH_CROSS, CH_LLCORNER, CH_BTEE,
    0, 15, MAX_X - 10, MAX_Y-1,
    MAX_X - 11, MAX_Y - 17,
    1,
    DumpHandler,
    0, 0
};
static FrameDesc HelpFrame = {
    CH_ULCORNER, CH_URCORNER, CH_LLCORNER, CH_LRCORNER,
    0, 0, MAX_X - 1, MAX_Y-1,
    MAX_X - 2, MAX_Y - 2,
    0,
    HelpHandler,
    sizeof (HelpText) / sizeof (HelpText [0]), HelpText
};
static FrameDesc* Frames [] = {
    &AsmFrame,
    &RegFrame,
    &StackFrame,
    &CStackFrame,
    &DumpFrame,
    &HelpFrame
};

/* Number of active frame, -1 = none */
static int ActiveFrame = -1;

/* Window names */
#define WIN_ASM         0
#define WIN_REG         1
#define WIN_STACK       2
#define WIN_CSTACK      3
#define WIN_DUMP        4
#define WIN_HELP        5

/* Other window data */
static unsigned AsmAddr;        /* Start address of output */
static unsigned DumpAddr;       /* Start address of output */
static unsigned CStackAddr;     /* Start address of output */
static unsigned char StackAddr; /* Start address of output */



/* Prompt line data */
static char* ActivePrompt = 0;  /* Last prompt line displayed */
static char PromptColor;        /* Color behind prompt */
static char PromptLength;       /* Length of current prompt string */



/* Values for the bk_use field of struct BreakPoint */
#define BRK_EMPTY       0x00
#define BRK_USER        0x01
#define BRK_TMP         0x80

/* Structure describing a breakpoint */
typedef struct {
    unsigned      bk_addr;      /* Address, 0 if unused */
    unsigned char bk_opc;       /* Opcode */
    unsigned char bk_use;       /* 1 if in use, 0 otherwise */
} BreakPoint;



/* Temporary breakpoints - also accessed from the assembler source */
#define MAX_USERBREAKS  10
unsigned char DbgBreakCount = 0;
BreakPoint DbgBreaks [MAX_USERBREAKS+2];



/*****************************************************************************/
/*              Forwards for functions in the assembler source               */
/*****************************************************************************/



BreakPoint* DbgGetBreakSlot (void);
/* Search for a free breakpoint slot. Return a pointer to the slot or 0 */

BreakPoint* DbgIsBreak (unsigned Addr);
/* Check if there is a user breakpoint at the given address, if so, return
** a pointer to the slot, else return 0.
*/



/*****************************************************************************/
/*                         Frame/window drawing code                         */
/*****************************************************************************/



static void DrawFrame (register FrameDesc* F, char Active)
/* Draw one window frame */
{
    TextDesc* T;
    unsigned char Count;
    unsigned char tl, tr, bl, br;
    unsigned char x1, y1, width;
    unsigned char OldColor;

    /* Determine the characters for the corners, set frame color */
    if (Active) {
        OldColor = textcolor (COLOR_FRAMEHIGH);
        tl = CH_ULCORNER;
        tr = CH_URCORNER;
        bl = CH_LLCORNER;
        br = CH_LRCORNER;
    } else {
        OldColor = textcolor (COLOR_FRAMELOW);
        tl = F->fd_tl;
        tr = F->fd_tr;
        bl = F->fd_bl;
        br = F->fd_br;
    }

    /* Get the coordinates into locals for faster access */
    x1 = F->fd_x1;
    y1 = F->fd_y1;
    width = F->fd_width;

    /* Top line */
    cputcxy (x1, y1, tl);
    chline (width);
    cputc (tr);

    /* Left line */
    cvlinexy (x1, ++y1, F->fd_height);

    /* Bottom line */
    cputc (bl);
    chline (width);
    cputc (br);

    /* Right line */
    cvlinexy (F->fd_x2, y1, F->fd_height);

    /* If the window has static text associated, print the text */
    (void) textcolor (COLOR_TEXTLOW);
    Count = F->fd_textcount;
    T = F->fd_text;
    while (Count--) {
        cputsxy (x1 + T->x, y1 + T->y, T->text);
        ++T;
    }

    /* Set the old color */
    (void) textcolor (OldColor);
}



static void DrawFrames (void)
/* Draw all frames */
{
    unsigned char I;
    FrameDesc* F;

    /* Build the frame layout of the screen */
    for (I = 0; I < sizeof (Frames) / sizeof (Frames [0]); ++I) {
        F = Frames [I];
        if (F->fd_visible) {
            DrawFrame (F, 0);
        }
    }
}



static void ActivateFrame (int Num, unsigned char Clear)
/* Activate a new frame, deactivate the old one */
{
    unsigned char y;
    register FrameDesc* F;

    if (ActiveFrame != Num) {

        /* Deactivate the old one */
        if (ActiveFrame >= 0) {
            DrawFrame (Frames [ActiveFrame], 0);
        }

        /* Activate the new one */
        if ((ActiveFrame = Num) >= 0) {
            F = Frames [ActiveFrame];
            /* Clear the frame if requested */
            if (Clear) {
                for (y = F->fd_y1+1; y < F->fd_y2; ++y) {
                    cclearxy (F->fd_x1+1, y, F->fd_width);
                }
            }
            DrawFrame (F, 1);
        }

        /* Redraw the current prompt line */
        DisplayPrompt (ActivePrompt);

    }
}



/*****************************************************************************/
/*                                Prompt line                                */
/*****************************************************************************/



static void DisplayPrompt (char* s)
/* Display a prompt */
{
    unsigned char OldColor;

    /* Remember the current color */
    OldColor = textcolor (COLOR_TEXTHIGH);

    /* Clear the old prompt if there is one */
    if (ActivePrompt) {
        (void) textcolor (PromptColor);
        chlinexy ((MAX_X - PromptLength) / 2, MAX_Y-1, PromptLength);
    }

    /* Get the new prompt data */
    ActivePrompt = s;
    PromptColor  = OldColor;
    PromptLength = strlen (ActivePrompt);

    /* Display the new prompt */
    (void) textcolor (COLOR_TEXTHIGH);
    cputsxy ((MAX_X - PromptLength) / 2, MAX_Y-1, ActivePrompt);

    /* Restore the old color */
    (void) textcolor (PromptColor);
}



static void HelpPrompt (void)
/* Display a prompt line mentioning the help key */
{
    DisplayPrompt ("Press F1 for help");
}



static void AnyKeyPrompt (void)
{
    DisplayPrompt ("Press any key to continue");
}



static char IsAbortKey (char C)
/* Return true if C is an abort key */
{
#if defined(CH_ESC)
    if (C == CH_ESC) {
        return 1;
    }
#endif
#if defined(CH_STOP)
    if (C == CH_STOP) {
        return 1;
    }
#endif
#if !defined(CH_ESC) && !defined(CH_STOP)
    /* Avoid compiler warning about unused parameter */
    (void) C;
#endif
    return 0;
}



static char Input (char* Prompt, char* Buf, unsigned char Count)
/* Read input from the user, return 1 on success, 0 if aborted */
{
    int Frame;
    unsigned char OldColor;
    unsigned char OldCursor;
    unsigned char x1;
    unsigned char i;
    unsigned char done;
    char c;

    /* Clear the current prompt line */
    cclearxy (0, MAX_Y-1, MAX_X);

    /* Display the new prompt */
    OldColor = textcolor (COLOR_TEXTHIGH);
    cputsxy (0, MAX_Y-1, Prompt);
    (void) textcolor (COLOR_TEXTLOW);

    /* Remember where we are, enable the cursor */
    x1 = wherex ();
    OldCursor = cursor (1);

    /* Get input and handle it */
    i = done = 0;
    do {
        c = cgetc ();
        if (isalnum (c) && i < Count) {
            Buf [i] = c;
            cputcxy (x1 + i, MAX_Y-1, c);
            ++i;
        } else if (i > 0 && c == CH_DEL) {
            --i;
            cputcxy (x1 + i, MAX_Y-1, ' ');
            gotoxy (x1 + i, MAX_Y-1);
        } else if (c == '\n') {
            Buf [i] = '\0';
            done = 1;
        } else if (IsAbortKey (c)) {
            /* Abort */
            done = 2;
        }
    } while (!done);

    /* Reset settings, display old prompt line */
    cursor (OldCursor);
    (void) textcolor (OldColor);
    DrawFrames ();
    Frame = ActiveFrame;
    ActiveFrame = -1;
    ActivateFrame (Frame, 0);

    return (done == 1);
}



static char InputHex (char* Prompt, unsigned* Val)
/* Prompt for a hexadecimal value. Return 0 on failure. */
{
    char Buf [5];
    char* P;
    char C;
    unsigned V;

    /* Read input from the user (4 digits max), check input */
    if (Input (Prompt, Buf, sizeof (Buf)-1) && isxdigit (Buf [0])) {

        /* Check the characters and convert to hex */
        P = Buf;
        V = 0;
        while ((C = *P) && isxdigit (C)) {
            V <<= 4;
            if (isdigit (C)) {
                C -= '0';
            } else {
                C = toupper (C) - ('A' - 10);
            }
            V += C;
            ++P;
        }

        /* Assign the value */
        *Val = V;

        /* Success */
        return 1;

    } else {

        /* Failure */
        return 0;

    }
}



static void ErrorPrompt (char* Msg)
/* Display an error message and wait for a key */
{
    /* Save the current prompt */
    char* OldPrompt = ActivePrompt;

    /* Display the new one */
    DisplayPrompt (Msg);

    /* Wait for a key and discard it */
    cgetc ();

    /* Restore the old prompt */
    DisplayPrompt (OldPrompt);
}



static char InputGoto (unsigned* Addr)
/* Prompt "Goto" and read an address. Print an error and return 0 on failure. */
{
    char Ok;
    Ok = InputHex ("Goto: ", Addr);
    if (!Ok) {
        ErrorPrompt ("Invalid input - press a key");
    }
    return Ok;
}



static void BreakInRomError (void)
/* Print an error message if we cannot set a breakpoint */
{
    ErrorPrompt ("Cannot set breakpoint - press a key");
}



/*****************************************************************************/
/*                            Breakpoint handling                            */
/*****************************************************************************/



static void DbgSetTmpBreak (unsigned Addr)
/* Set a breakpoint */
{
    BreakPoint* B = DbgGetBreakSlot ();
    B->bk_addr    = Addr;
    B->bk_use     = BRK_TMP;
}



static void DbgToggleUserBreak (unsigned Addr)
/* Set a breakpoint */
{
    register BreakPoint* B = DbgIsBreak (Addr);

    if (B) {
        /* We have a breakpoint, remove it */
        B->bk_use = BRK_EMPTY;
        --DbgBreakCount;
    } else {
        /* We don't have a breakpoint, set one */
        if (DbgBreakCount >= MAX_USERBREAKS) {
            ErrorPrompt ("Too many breakpoints - press a key");
        } else {
            /* Test if we can set a breakpoint at that address */
            if (!DbgIsRAM (Addr)) {
                BreakInRomError ();
            } else {
                /* Set the breakpoint */
                B = DbgGetBreakSlot ();
                B->bk_addr = Addr;
                B->bk_use  = BRK_USER;
                ++DbgBreakCount;
            }
        }
    }
}



static void DbgResetTmpBreaks (void)
/* Reset all temporary breakpoints */
{
    unsigned char i;
    BreakPoint* B = DbgBreaks;

    for (i = 0; i < MAX_USERBREAKS; ++i) {
        if (B->bk_use == BRK_TMP) {
            B->bk_use = BRK_EMPTY;
        }
        ++B;
    }
}



static unsigned char DbgTmpBreaksOk (void)
/* Check if the temporary breakpoints can be set, if so, return 1, if not,
** reset them all and return 0.
*/
{
    unsigned char i;
    BreakPoint* B = DbgBreaks;
    for (i = 0; i < MAX_USERBREAKS; ++i) {
        if (B->bk_use == BRK_TMP && !DbgIsRAM (B->bk_addr)) {
            BreakInRomError ();
            DbgResetTmpBreaks ();
            return 0;
        }
        ++B;
    }
    return 1;
}



/*****************************************************************************/
/*                          Assembler window stuff                           */
/*****************************************************************************/



static unsigned AsmBack (unsigned mem, unsigned char lines)
/* Go back in the assembler window the given number of lines (calculate
** new start address).
*/
{
    unsigned cur;
    unsigned adr [32];
    unsigned char in;

    unsigned offs = 6;
    while (1) {
        in = 0;
        cur = mem - (lines * 3) - offs;
        while (1) {
            cur += DbgDisAsmLen (cur);
            adr [in] = cur;
            in = (in + 1) & 0x1F;
            if (cur >= mem) {
                if (cur == mem || offs == 12) {
                    /* Found */
                    return adr [(in - lines - 1) & 0x1F];
                } else {
                    /* The requested address is inside an instruction, go back
                    ** one more byte and try again.
                    */
                    ++offs;
                    break;
                }
            }
        }
    }
}



static unsigned UpdateAsm (void)
/* Update the assembler window starting at the given address */
{
    char buf [MAX_X];
    unsigned char len;
    unsigned char y;
    unsigned char width = AsmFrame.fd_width;
    unsigned char x = AsmFrame.fd_x1 + 1;
    unsigned      m = AsmBack (AsmAddr, 2);

    for (y = AsmFrame.fd_y1+1; y < AsmFrame.fd_y2; ++y) {
        len = DbgDisAsm (m, buf, width);
        if (m == brk_pc) {
            buf [4] = '-';
            buf [5] = '>';
        }
        if (DbgIsBreak (m)) {
            buf [5] = '*';
        }
        if (m == AsmAddr) {
            revers (1);
            cputsxy (1, y, buf);
            revers (0);
        } else {
            cputsxy (1, y, buf);
        }
        m += len;
    }
    return m;
}



static unsigned AsmArg16 (void)
/* Return a 16 bit argument */
{
    return *(unsigned*)(AsmAddr+1);
}



static void AsmFollow (void)
/* Follow the current instruction */
{
    switch (*(unsigned char*) AsmAddr) {

        case OPC_JMP:
        case OPC_JSR:
            AsmAddr = AsmArg16 ();
            break;

        case OPC_JMPIND:
            AsmAddr = *(unsigned*)AsmArg16 ();
            break;

        case OPC_BPL:
        case OPC_BMI:
        case OPC_BVC:
        case OPC_BVS:
        case OPC_BCC:
        case OPC_BCS:
        case OPC_BNE:
        case OPC_BEQ:
            AsmAddr = AsmAddr + 2 + *(signed char*)(AsmAddr+1);
            break;

        case OPC_RTS:
            AsmAddr = (*(unsigned*) (DbgSP + 0x101) + 1);
            break;

        case OPC_RTI:
            AsmAddr = *(unsigned*) (DbgSP + 0x102);
            break;

    }
}



static void AsmHome (void)
/* Set the cursor to home position */
{
    AsmAddr = brk_pc;
}



static void InitAsm (void)
/* Initialize the asm window */
{
    AsmHome ();
    UpdateAsm ();
}



static char AsmHandler (void)
/* Get characters and handle them */
{
    char c;
    unsigned Last;

    while (1) {

        /* Update the window contents */
        Last = UpdateAsm ();

        /* Read and handle input */
        switch (c = GetKeyUpdate ()) {

            case  '+':
                AsmAddr = Last;
                break;

            case '-':
                AsmAddr = AsmBack (AsmAddr, AsmFrame.fd_height);
                break;

            case 't':
#ifdef CH_F2
            case CH_F2:
#endif
                DbgToggleUserBreak (AsmAddr);
                break;

            case 'f':
                AsmFollow ();
                break;

            case 'g':
                InputGoto (&AsmAddr);
                break;

            case 'o':
                AsmHome ();
                break;

            case 'p':
                brk_pc = AsmAddr;
                break;

            case 'a':
#ifdef CH_CURS_UP
            case CH_CURS_UP:
#endif
                AsmAddr = AsmBack (AsmAddr, 1);
                break;

            case 'z':
#ifdef CH_CURS_DOWN
            case CH_CURS_DOWN:
#endif
                AsmAddr += DbgDisAsmLen (AsmAddr);
                break;

            default:
                return c;

        }
    }
}



/*****************************************************************************/
/*                           Register window stuff                           */
/*****************************************************************************/



static unsigned UpdateReg (void)
/* Update the register window */
{
    unsigned char x1 = RegFrame.fd_x1 + 5;
    unsigned char x2 = x1 + 2;
    unsigned char y = RegFrame.fd_y1;

    /* Print the register contents */
    gotoxy (x1, ++y);   cputhex16 (brk_pc);
    gotoxy (x2, ++y);   cputhex8  (brk_sr);
    gotoxy (x2, ++y);   cputhex8  (brk_a);
    gotoxy (x2, ++y);   cputhex8  (brk_x);
    gotoxy (x2, ++y);   cputhex8  (brk_y);
    gotoxy (x2, ++y);   cputhex8  (DbgSP);
    gotoxy (x1, ++y);   cputhex16 (DbgCS);
    gotoxy (x1, ++y);   cputhex16 (DbgHI);

    /* Not needed */
    return 0;
}



static void InitReg (void)
/* Initialize the register window */
{
    UpdateReg ();
}



static char RegHandler (void)
/* Get characters and handle them */
{
    return GetKeyUpdate ();
}



/*****************************************************************************/
/*                             Stack window stuff                            */
/*****************************************************************************/



static unsigned UpdateStack (void)
/* Update the stack window */
{
    unsigned char mem = StackAddr;
    unsigned char x1 = StackFrame.fd_x1 + 1;
    unsigned char x2 = x1 + 6;
    unsigned char y;

    for (y = StackFrame.fd_y2-1; y > StackFrame.fd_y1; --y) {
        gotoxy (x1, y);
        cputhex8 (mem);
        gotoxy (x2, y);
        cputhex8 (* (unsigned char*) (mem + 0x100));
        ++mem;
    }
    return mem;
}



static void StackHome (void)
/* Set the cursor to home position */
{
    StackAddr = DbgSP + 1;
}



static void InitStack (void)
/* Initialize the stack window */
{
    StackHome ();
    UpdateStack ();
}



static char StackHandler (void)
/* Get characters and handle them */
{
    char c;
    unsigned char BytesPerPage = StackFrame.fd_height;

    while (1) {

        /* Read and handle input */
        switch (c = GetKeyUpdate ()) {

            case  '+':
                StackAddr += BytesPerPage;
                break;

            case '-':
                StackAddr -= BytesPerPage;
                break;

            case 'o':
                StackHome ();
                break;

            case 'a':
#ifdef CH_CURS_UP:
            case CH_CURS_UP:
#endif
                --StackAddr;
                break;

            case 'z':
#ifdef CH_CURS_DOWN
            case CH_CURS_DOWN:
#endif
                ++StackAddr;
                break;

            default:
                return c;

        }

        /* Update the window contents */
        UpdateStack ();
    }
}



/*****************************************************************************/
/*                           C Stack window stuff                            */
/*****************************************************************************/



static unsigned UpdateCStack (void)
/* Update the C stack window */
{
    unsigned mem     = CStackAddr;
    unsigned char x  = CStackFrame.fd_x1 + 5;
    unsigned char y;

    for (y = CStackFrame.fd_y2-1; y > CStackFrame.fd_y1; --y) {
        gotoxy (x, y);
        cputhex16 (* (unsigned*)mem);
        mem += 2;
    }
    cputsxy (CStackFrame.fd_x1+1, CStackFrame.fd_y2-1, "->");
    return mem;
}



static void CStackHome (void)
/* Set the cursor to home position */
{
    CStackAddr = DbgCS;
}



static void InitCStack (void)
/* Initialize the C stack window */
{
    CStackHome ();
    UpdateCStack ();
}



static char CStackHandler (void)
/* Get characters and handle them */
{
    char c;
    unsigned char BytesPerPage = CStackFrame.fd_height * 2;

    while (1) {

        /* Read and handle input */
        switch (c = GetKeyUpdate ()) {

            case  '+':
                CStackAddr += BytesPerPage;
                break;

            case '-':
                CStackAddr -= BytesPerPage;
                break;

            case 'o':
                CStackHome ();
                break;

            case 'a':
#ifdef CH_CURS_UP
            case CH_CURS_UP:
#endif
                CStackAddr -= 2;
                break;

            case 'z':
#ifdef CH_CURS_DOWN
            case CH_CURS_DOWN:
#endif
                CStackAddr += 2;
                break;

            default:
                return c;

        }

        /* Update the window contents */
        UpdateCStack ();
    }
}



/*****************************************************************************/
/*                             Dump window stuff                             */
/*****************************************************************************/



static unsigned UpdateDump (void)
/* Update the dump window */
{
    char Buf [MAX_X];
    unsigned char y;
    unsigned mem = DumpAddr;
    unsigned char x = DumpFrame.fd_x1 + 1;
    unsigned char* p = (unsigned char*) mem;

    for (y = DumpFrame.fd_y1+1; y < DumpFrame.fd_y2; ++y) {
        cputsxy (x, y, DbgMemDump (mem, Buf, DUMP_BYTES));
        mem += DUMP_BYTES;
    }
    return mem;
}



static void DumpHome (void)
/* Set the cursor to home position */
{
    DumpAddr = 0;
}



static char DumpHandler (void)
/* Get characters and handle them */
{
    char c;
    unsigned BytesPerPage = DumpFrame.fd_height * 8;

    while (1) {

        /* Read and handle input */
        switch (c = GetKeyUpdate ()) {

            case  '+':
                DumpAddr += BytesPerPage;
                break;

            case '-':
                DumpAddr -= BytesPerPage;
                break;

            case 'g':
                InputGoto (&DumpAddr);
                break;

            case 'o':
                DumpHome ();
                break;

            case 'a':
#ifdef CH_CURS_UP
            case CH_CURS_UP:
#endif
                DumpAddr -= 8;
                break;

            case 'z':
#ifdef CH_CURS_DOWN
            case CH_CURS_DOWN:
#endif
                DumpAddr += 8;
                break;

            default:
                return c;

        }

        /* Update the window contents */
        UpdateDump ();
    }
}



/*****************************************************************************/
/*                             Help window stuff                             */
/*****************************************************************************/



static char HelpHandler (void)
/* Get characters and handle them */
{
    /* Activate the frame */
    int OldActive = ActiveFrame;
    ActivateFrame (WIN_HELP, 1);

    /* Say that we're waiting for a key */
    AnyKeyPrompt ();

    /* Get a character and discard it */
    cgetc ();

    /* Redraw the old stuff */
    Redraw (OldActive);

    /* Done, return no char */
    return 0;
}



/*****************************************************************************/
/*                                Singlestep                                 */
/*****************************************************************************/



static unsigned GetArg16 (void)
/* Read an argument */
{
    return *(unsigned*)(brk_pc+1);
}



static unsigned GetStack16 (unsigned char Offs)
/* Fetch a 16 bit value from stack top */
{
    return *(unsigned*)(DbgSP+Offs+0x101);
}



static void SetRTSBreak (void)
/* Set a breakpoint at the return target */
{
    DbgSetTmpBreak (GetStack16 (0) + 1);
}



static void SingleStep (char StepInto)
{
    signed char Offs;

    switch (*(unsigned char*) brk_pc) {

        case OPC_JMP:
            /* Set breakpoint at target */
            DbgSetTmpBreak (GetArg16 ());
            return;

        case OPC_JMPIND:
            /* Indirect jump, ignore CPU error when crossing page */
            DbgSetTmpBreak (*(unsigned*)GetArg16 ());
            return;

        case OPC_BPL:
        case OPC_BMI:
        case OPC_BVC:
        case OPC_BVS:
        case OPC_BCC:
        case OPC_BCS:
        case OPC_BNE:
        case OPC_BEQ:
            /* Be sure not to set the breakpoint twice if this is a jump to
            ** the following instruction.
            */
            Offs = ((signed char*)brk_pc)[1];
            if (Offs) {
                DbgSetTmpBreak (brk_pc + Offs + 2);
            }
            break;

        case OPC_RTS:
            /* Set a breakpoint at the return target */
            SetRTSBreak ();
            return;

        case OPC_RTI:
            /* Set a breakpoint at the return target */
            DbgSetTmpBreak (GetStack16 (1));
            return;

        case OPC_JSR:
            if (StepInto) {
                /* Set breakpoint at target */
                DbgSetTmpBreak (GetArg16 ());
                return;
            }
            break;
    }

    /* Place a breakpoint behind the instruction */
    DbgSetTmpBreak (brk_pc + DbgDisAsmLen (brk_pc));
}



/*****************************************************************************/
/*                        High level window handling                         */
/*****************************************************************************/



static void RedrawStatic (char Frame)
/* Redraw static display stuff */
{
    /* Reset the active frame */
    ActiveFrame = -1;

    /* Clear the screen hide the cursor */
    (void) bordercolor (COLOR_BORDER);
    (void) bgcolor (COLOR_BACKGROUND);
    clrscr ();
    cursor (0);

    /* Build the frame layout of the screen */
    (void) textcolor (COLOR_FRAMELOW);
    DrawFrames ();

    /* Draw the prompt line */
    HelpPrompt ();

    /* Activate the active frame */
    ActivateFrame (Frame, 0);
}



static void Redraw (char Frame)
/* Redraw the display in case it's garbled */
{
    /* Redraw the static stuff */
    RedrawStatic (Frame);

    /* Init the window contents */
    UpdateAsm ();
    UpdateReg ();
    UpdateStack ();
    UpdateCStack ();
    UpdateDump ();
}



static char GetKeyUpdate (void)
/* Wait for a key updating the windows in the background */
{
    static unsigned char Win;

    /* While there are no keys... */
    while (!kbhit ()) {

        switch (Win) {

            case 0:
                UpdateAsm ();
                break;

            case 1:
                UpdateStack ();
                break;

            case 2:
                UpdateCStack ();
                break;

            case 3:
                UpdateDump ();
                break;
        }

        Win = (Win + 1) & 0x03;

    }

    /* We have a key - return it */
    return cgetc ();
}



/*****************************************************************************/
/*                       Externally visible functions                        */
/*****************************************************************************/



void DbgEntry (void)
/* Start up the debugger */
{
    static unsigned char FirstTime = 1;
    char c;
    char done;

    /* If this is the first call, setup the display */
    if (FirstTime) {
        FirstTime = 0;

        /* Draw the window, default active frame is ASM frame */
        RedrawStatic (WIN_ASM);
        InitAsm ();
        InitReg ();
        InitStack ();
        InitCStack ();
        UpdateDump ();
    }

    /* Only initialize variables here, don't do a display update. The actual
    ** display update will be done while waiting for user input.
    */
    AsmHome ();
    UpdateReg ();               /* Must update this (static later) */
    StackHome ();
    CStackHome ();

    /* Wait for user input */
    done = 0;
    while (!done) {
        c = Frames [ActiveFrame]->fd_func ();
        switch (c) {

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
                ActivateFrame (c - '1', 0);
                break;

            case '?':
#ifdef CH_F1
            case CH_F1:
#endif
                HelpHandler ();
                break;

            case 'u':
#ifdef CH_F3
            case CH_F3:
#endif
                /* Go until return */
                SetRTSBreak ();
                done = 1;
                break;

            case 'h':
#ifdef CH_F4
            case CH_F4:
#endif
                /* Go to cursor, only possible if cursor not at current PC */
                if (AsmAddr != brk_pc) {
                    DbgSetTmpBreak (AsmAddr);
                    done = 1;
                }
                break;

            case ' ':
#ifdef CH_F7
            case CH_F7:
#endif
                SingleStep (1);
                if (DbgTmpBreaksOk ()) {
                    /* Could set breakpoints */
                    done = 1;
                }
                break;

            case '\n':
#ifdef CH_F8
            case CH_F8:
#endif
                SingleStep (0);
                if (DbgTmpBreaksOk ()) {
                    /* Could set breakpoints */
                    done = 1;
                }
                break;

            case 'c':
            case 0:
                done = 1;
                break;

            case 's':
                /* Skip instruction */
                brk_pc += DbgDisAsmLen (brk_pc);
                InitAsm ();
                break;

            case 'r':
                /* Redraw screen */
                Redraw (ActiveFrame);
                break;

            case 'q':
                /* Quit program */
                clrscr ();
                exit (1);

        }
    }
}



