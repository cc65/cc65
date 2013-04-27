/* GEOS resource compiler

   by Maciej 'YTM/Elysium' Witkowiak

   see GEOSLib documentation for license info
*/

/* - make it work, then do it better
   - more or less comments? it was hard to code, should be even harder to
     understand =D
   - add loadable icons feature (binary - 63 bytes)
*/

/* - err, maybe free allocated memory, huh? (who cares, it's just a little prog...)
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <time.h>

/* common stuff */
#include "abend.h"
#include "cmdline.h"
#include "fname.h"
#include "chartype.h"
#include "target.h"
#include "version.h"
#include "xmalloc.h"

/* I hope that no one will be able to create a .grc bigger than this... */
#define BLOODY_BIG_BUFFER 65000



struct menuitem {
    char *name;
    char *type;
    char *target;
    struct menuitem *next;
};

struct menu {
    char *name;
    int top, left;
    int bot, right;
    char *type;
    struct menuitem *item;
};

struct appheader {
    int year, month, day, hour, min;
    int mode;
    int dostype;
    int geostype;
    int structure;
    char *dosname;
    char *classname;
    char *version;
    char *author;
    char *info;
    char *icon;
};

const char *mainToken[] = {"MENU", "HEADER", "ICON", "DIALOG", "MEMORY", ""};

const char *toggle[] = {"off", "no", "0", "on", "yes", "1", ""};

const char *hdrFTypes[] = {"APPLICATION", "AUTO_EXEC", "DESK_ACC", "ASSEMBLY",
                           "DISK_DEVICE", "PRINTER", "SYSTEM", ""};

const char *hdrFields[] = {"author", "info", "date", "dostype", "mode", "structure", "icon", ""};

const char *hdrDOSTp[] = {"seq", "SEQ", "prg", "PRG", "usr", "USR", ""};

const char *hdrStructTp[] = {"seq", "SEQ", "vlir", "VLIR", ""};

const char *hdrModes[] = {"any", "40only", "80only", "c64only", ""};

const char *memFields[] = {"stacksize", "overlaysize", "overlaynums", "backbuffer", ""};

const int BSWTab[] = {0, 0x005, 0x007, 0x00b, 0x011, 0x017, 0x01d, 0x023,
    0x025, 0x029, 0x02d, 0x033, 0x039, 0x03c, 0x041, 0x043, 0x04a, 0x04f,
    0x052, 0x056, 0x05a, 0x05f, 0x063, 0x068, 0x06d, 0x072, 0x077, 0x079,
    0x07c, 0x080, 0x084, 0x088, 0x08e, 0x094, 0x09a, 0x09f, 0x0a4, 0x0a9,
    0x0ad, 0x0b1, 0x0b6, 0x0bc, 0x0be, 0x0c2, 0x0c8, 0x0cc, 0x0d4, 0x0da,
    0x0e0, 0x0e5, 0x0eb, 0x0f0, 0x0f5, 0x0f9, 0x0fe, 0x104, 0x10c, 0x112,
    0x118, 0x11e, 0x121, 0x129, 0x12c, 0x132, 0x13a, 0x13e, 0x143, 0x148,
    0x14d, 0x152, 0x157, 0x15a, 0x15f, 0x164, 0x166, 0x168, 0x16d, 0x16f,
    0x177, 0x17c, 0x182, 0x187, 0x18c, 0x18f, 0x193, 0x196, 0x19b, 0x1a1,
    0x1a9, 0x1af, 0x1b4, 0x1ba, 0x1be, 0x1c0, 0x1c4, 0x1ca, 0x1d2, 0x1dd};

const unsigned char icon1[] = {255, 255, 255, 128,   0,   1, 128,   0,   1,
                               128,   0,   1, 128,   0,   1, 128,   0,   1,
                               128,   0,   1, 128,   0,   1, 128,   0,   1,
                               128,   0,   1, 128,   0,   1, 128,   0,   1,
                               128,   0,   1, 128,   0,   1, 128,   0,   1,
                               128,   0,   1, 128,   0,   1, 128,   0,   1,
                               128,   0,   1, 128,   0,   1, 255, 255, 255};

const char *outputCName = NULL;
const char *outputSName = NULL;
FILE *outputCFile, *outputSFile;
int CFnum = 0, SFnum = 0;
int apple = 0;
char outputCMode[2] = "w";
char outputSMode[2] = "w";


static void Usage (void)
{
    printf (
        "Usage: %s [options] file\n"
        "Short options:\n"
        "  -V\t\t\tPrint the version number\n"
        "  -h\t\t\tHelp (this text)\n"
        "  -o name\t\tName the C output file\n"
        "  -s name\t\tName the asm output file\n"
        "  -t sys\t\tSet the target system\n"
        "\n"
        "Long options:\n"
        "  --help\t\tHelp (this text)\n"
        "  --target sys\t\tSet the target system\n"
        "  --version\t\tPrint the version number\n",
        ProgName);
}


static void OptHelp (const char* Opt attribute ((unused)),
                     const char* Arg attribute ((unused)))
/* Print usage information and exit */
{
    Usage ();
    exit (EXIT_SUCCESS);
}


static void OptTarget (const char* Opt attribute ((unused)), const char* Arg)
/* Set the target system */
{
    switch (FindTarget (Arg)) {

        case TGT_GEOS_CBM:
            apple = 0;
            break;

        case TGT_GEOS_APPLE:
            apple = 1;
            break;

        case TGT_UNKNOWN:
            AbEnd ("Unknown target system `%s'", Arg);
            break;

        default:
            /* Target is known but unsupported */
            AbEnd ("Unsupported target system `%s'", Arg);
            break;
    }
}


static void OptVersion (const char* Opt attribute ((unused)),
                        const char* Arg attribute ((unused)))
/* Print the program version */
{
    fprintf (stderr, "grc65 V%s\n", GetVersionAsString ());
}



static void printCHeader (void)
{
    fprintf (outputCFile,
        "//\n"
        "//\tThis file was generated by the GEOS Resource Compiler\n"
        "//\n"
        "//\tDO NOT EDIT! Any changes will be lost!\n"
        "//\n"
        "//\tEdit proper resource file instead.\n"
        "//\n\n");
}


static void printSHeader (void)
{
    fprintf (outputSFile,
        ";\n"
        ";\tThis file was generated by the GEOS Resource Compiler\n"
        ";\n"
        ";\tDO NOT EDIT! Any changes will be lost!\n"
        ";\n"
        ";\tEdit proper resource file instead.\n"
        ";\n\n");
}


static void openCFile (void)
{
    if ((outputCFile = fopen (outputCName,outputCMode)) == 0) {
        AbEnd ("Can't open file %s for writing: %s", outputCName, strerror (errno));
    }

    if (CFnum == 0) {
        outputCMode[0] = 'a';
        printCHeader ();
        CFnum++;
    }
}


static void openSFile (void)
{
    if ((outputSFile = fopen (outputSName, outputSMode)) == 0) {
        AbEnd ("Can't open file %s for writing: %s", outputSName, strerror (errno));
    }

    if (SFnum == 0) {
        outputSMode[0] = 'a';
        printSHeader ();
        SFnum++;
    }
}


static int findToken (const char **tokenTbl, const char *token)
{
    /* takes as input table of tokens and token, returns position in table or -1 if not found */
    int a = 0;

    while (strlen (tokenTbl[a]) != 0) {
        if (strcmp (tokenTbl[a], token) == 0) break;
        a++;
    }

    if (strlen (tokenTbl[a]) == 0) a = -1;
    return a;
}


static char *nextPhrase (void)
{
    return strtok (NULL, "\"");
}


static char *nextWord (void)
{
    return strtok (NULL, " ");
}


static void setLen (char *name, unsigned len)
{
    if (strlen (name) > len) {
        name[len] = '\0';
    }
}


static void fillOut (char *name, int len, char *filler)
{
    int a;

    setLen (name, len);
    fprintf (outputSFile, "\t.byte \"%s\"\n", name);

    a = strlen (name);
    if (a < len) {
        fprintf (outputSFile, "\t.res  (%i - %i), %s\n", len, a, filler);
    }
}


static char *bintos (unsigned char a, char out[7])
{
    int i=0;

    for (; i < 8; i++) {
        out[7 - i] = ((a & 1) == 0) ? '0' : '1';
        a = a >> 1;
    }
    out[i] = '\0';

    return out;
}


static int getNameSize (const char *word)
{
    /* count length of a word using BSW 9 font table */
    int a = 0, i = 0;

    while (word[i] != '\0') {
        a += (BSWTab[word[i] - 31] - BSWTab[word[i] - 32]);
        i++;
    }

    return a;
}


static void DoMenu (void)
{
    int a, size, tmpsize, item = 0;
    char *token;
    char namebuff[255] = "";
    struct menu myMenu;
    struct menuitem *curItem, *newItem;

    openCFile ();

    myMenu.name = nextWord ();
    myMenu.left = atoi (nextWord ());
    myMenu.top = atoi (nextWord ());
    myMenu.type = nextWord ();

    if (strcmp (nextWord (), "{") != 0) {
        AbEnd ("Menu '%s' description has no opening bracket!", myMenu.name);
    }
    curItem = xmalloc (sizeof(struct menuitem));
    myMenu.item = curItem;

    for (;;) {
        token = nextWord ();
        if (strcmp (token, "}") == 0) break;
        if (token[strlen (token) - 1] != '"') {
            strcpy (namebuff, token);
            do {
                token = nextWord ();
                strcat (namebuff, " ");
                strcat (namebuff, token);
            } while (token[strlen (token) - 1] != '"');
            token = xmalloc (strlen (namebuff));
            strcpy (token, namebuff);
        }
        curItem->name = token;
        curItem->type = nextWord ();
        curItem->target = nextWord ();
        newItem = xmalloc (sizeof(struct menuitem));
        curItem->next = newItem;
        curItem = newItem;
        item++;
    }

    if (item == 0) AbEnd ("Menu '%s' has 0 items!", myMenu.name);
    if (item > 31) AbEnd ("Menu '%s' has too many items!", myMenu.name);

    curItem->next = NULL;

    /* count menu sizes */
    size = 0;
    curItem = myMenu.item;
    if (strstr (myMenu.type, "HORIZONTAL") != NULL) {
        /* menu is HORIZONTAL, ysize=15, sum xsize of all items +~8?*/
        myMenu.bot = myMenu.top + 15;
        for (a = 0; a != item; a++) {
            size += getNameSize (curItem->name);
            curItem = curItem->next;
        }
    } else {
        /* menu is VERTICAL, ysize=item*15, count largest xsize of all items +~8? */
        myMenu.bot = myMenu.top + (14 * item);
        for (a = 0; a != item; a++) {
            tmpsize = getNameSize (curItem->name);
            size = (size > tmpsize) ? size : tmpsize;
            curItem = curItem->next;
        }
    }
    myMenu.right = myMenu.left + size - 1;

    curItem = myMenu.item;
    for (a = 0; a != item; a++) {
        /* print prototype only if MENU_ACTION or DYN_SUB_MENU are present in type */
        if ((strstr (curItem->type, "MENU_ACTION") != NULL) || (strstr (curItem->type, "DYN_SUB_MENU") != NULL)) {
            fprintf (outputCFile,
                "void %s (void);\n",
                curItem->target);
        }
        curItem=curItem->next;
    }

    fprintf (outputCFile,
        "\n"
        "const void %s = {\n"
        "\t(char)%i, (char)%i,\n"
        "\t(int)%i, (int)%i,\n"
        "\t(char)(%i | %s),\n",
        myMenu.name, myMenu.top, myMenu.bot, myMenu.left, myMenu.right, item, myMenu.type);

    curItem = myMenu.item;
    for (a = 0; a != item; a++) {
        fprintf (outputCFile,
            "\t%s, (char)%s, (int)",
            curItem->name, curItem->type);
        if ((strstr (curItem->type, "SUB_MENU") != NULL) && (strstr (curItem->type, "DYN_SUB_MENU") == NULL)) {
            fprintf (outputCFile,
                "&");
        }
        fprintf (outputCFile,
            "%s,\n",
            curItem->target);
        curItem = curItem->next;
    }

    fprintf (outputCFile,
        "};\n\n");

    if (fclose (outputCFile) != 0) {
        AbEnd ("Error closing %s: %s", outputCName, strerror (errno));
    }
}


static void DoHeader (void)
{
    time_t t;
    struct tm *my_tm;

    struct appheader myHead;
    char *token;
    char i1[9], i2[9], i3[9];
    int i;

    openSFile ();

    token = nextWord ();

    i = findToken (hdrFTypes, token);

    if (apple == 1) {
        switch (i) {
            case 0:
                myHead.geostype = 0x82;
                break;
            default:
                AbEnd ("Filetype '%s' is not supported yet", token);
        }
    } else {
        switch (i) {
            case 0:
                myHead.geostype = 6;
                break;
            case 1:
                myHead.geostype = 14;
                break;
            default:
                AbEnd ("Filetype '%s' is not supported yet", token);
        }
    }

    myHead.dosname = nextPhrase ();
    nextPhrase ();
    myHead.classname = nextPhrase ();
    nextPhrase ();
    myHead.version = nextPhrase ();

    /* put default values into myHead here */
    myHead.author = "cc65";
    myHead.info = "Program compiled with cc65 and GEOSLib.";
    myHead.dostype = 128;
    if (apple == 0) myHead.dostype += 3;
    myHead.structure = 0;
    myHead.mode = 0;
    myHead.icon = NULL;

    t = time (NULL);
    my_tm = localtime (&t);

    myHead.year = my_tm->tm_year % 100;
    myHead.month = my_tm->tm_mon + 1;
    myHead.day = my_tm->tm_mday;
    myHead.hour = my_tm->tm_hour;
    myHead.min = my_tm->tm_min;

    if (strcmp (nextWord (), "{") != 0) {
        AbEnd ("Header '%s' has no opening bracket!", myHead.dosname);
    }

    for (;;) {
        token = nextWord ();
        if (strcmp (token, "}") == 0) break;
        switch (findToken (hdrFields, token)) {
            case -1:
                AbEnd ("Unknown field '%s' in header '%s'", token, myHead.dosname);
                break;

            case 0: /* author */
                myHead.author = nextPhrase ();
                break;

            case 1: /* info */
                myHead.info = nextPhrase ();
                break;

            case 2: /* date */
                myHead.year = atoi (nextWord ());
                myHead.month = atoi (nextWord ());
                myHead.day = atoi (nextWord ());
                myHead.hour = atoi (nextWord ());
                myHead.min = atoi (nextWord ());
                break;

            case 3: /* dostype */
                switch (i = findToken (hdrDOSTp, nextWord ())) {
                    case -1:
                        AbEnd ("Unknown dostype in header '%s'", myHead.dosname);
                        break;
                    default:
                        if (apple == 0) myHead.dostype = i / 2 + 128 + 1;
                        break;
                }
                break;

            case 4: /* mode */
                switch (findToken (hdrModes, nextWord ())) {
                    case -1:
                        AbEnd ("Unknown mode in header '%s'", myHead.dosname);
                    case 0:
                        if (apple == 0) myHead.mode = 0x40;
                        break;
                    case 1:
                        if (apple == 0) myHead.mode = 0x00;
                        break;
                    case 2:
                        if (apple == 0) myHead.mode = 0xc0;
                        break;
                    case 3:
                        if (apple == 0) myHead.mode = 0x80;
                        break;
                }
                break;

            case 5: /* structure */
                switch (findToken (hdrStructTp, nextWord ())) {
                    case -1:
                        AbEnd ("unknown structure type in header '%s'", myHead.dosname);
                    case 0:
                    case 1:
                        myHead.structure = 0;
                        break;
                    case 2:
                    case 3:
                        myHead.structure = 1;
                        break;
                }
                break;

            case 6: /* icon */
                myHead.icon = nextPhrase ();
                break;
        }
    }

    /* OK, all information is gathered, do flushout */

    fprintf (outputSFile,
        "\t\t.segment \"DIRENTRY\"\n\n");

    if (apple == 1) {

        if (myHead.structure == 0) {
            fprintf (outputSFile,
                "\t.import __VLIR0_START__, __VLIR0_LAST__, __BSS_SIZE__\n\n");
        }
        fprintf (outputSFile,
            "\t.byte %i << 4 | %u\n",
            myHead.structure + 2, (unsigned)strlen (myHead.dosname));

        fillOut (myHead.dosname, 15, "0");

        fprintf (outputSFile,
            "\t.byte $%02x\n"
            "\t.word 0\n"
            "\t.word 0\n"
            "\t.word %s\n"
            "\t.byte 0\n"
            "\t.word %i << 9 | %i << 5 | %i, %i << 8 | %i\n"
            "\t.byte 0\n"
            "\t.byte 0\n"
            "\t.byte 0\n"
            "\t.word 0\n"
            "\t.word %i << 9 | %i << 5 | %i, %i << 8 | %i\n"
            "\t.word 0\n\n",
            myHead.geostype,
            myHead.structure == 0 ?
                "__VLIR0_LAST__ - __VLIR0_START__ - __BSS_SIZE__" : "0",
            myHead.year, myHead.month, myHead.day, myHead.hour, myHead.min,
            myHead.year, myHead.month, myHead.day, myHead.hour, myHead.min);

    } else {

        fprintf (outputSFile,
            "\t.byte %i\n"
            "\t.word 0\n",
            myHead.dostype);

        fillOut (myHead.dosname, 16, "$a0");

        fprintf (outputSFile,
            "\t.word 0\n"
            "\t.byte %i\n"
            "\t.byte %i\n"
            "\t.byte %i, %i, %i, %i, %i\n\n"
            "\t.word 0\n"
            "\t.byte \"PRG formatted GEOS file V1.0\"\n\n",
            myHead.structure, myHead.geostype,
            myHead.year, myHead.month, myHead.day, myHead.hour, myHead.min);
    }

    fprintf (outputSFile,
        "\t\t.segment \"FILEINFO\"\n\n"
        "\t.import __VLIR0_START__, __STARTUP_RUN__\n\n"
        "\t.byte 3, 21, 63 | $80\n");

    if (myHead.icon != NULL) {
        fprintf (outputSFile,
            "\t.incbin \"%s\", 0, 63\n",
            myHead.icon);
    } else {
        for (i = 0; i != 63; i = i + 3) {
            fprintf (outputSFile,
                "\t.byte %%%s, %%%s, %%%s\n",
                bintos (icon1[i], i1), bintos (icon1[i+1], i2), bintos (icon1[i+2], i3));
        }
    }

    fprintf (outputSFile,
        "\t.byte %i, %i, %i\n"
        "\t.word __VLIR0_START__, __VLIR0_START__ - 1, __STARTUP_RUN__\n\n",
        myHead.dostype, myHead.geostype, myHead.structure);

    fillOut (myHead.classname, 12, "$20");

    fillOut (myHead.version, 4, "0");

    fprintf (outputSFile,
        "\t.byte 0, 0, 0\n"
        "\t.byte %i\n\n",
        myHead.mode);

    setLen (myHead.author, 62);
    fprintf (outputSFile,
        "\t.byte \"%s\"\n"
        "\t.byte 0\n"
        "\t.res  (63 - %i)\n\n",
        myHead.author, (int)(strlen (myHead.author) + 1));

    setLen (myHead.info, 95);
    fprintf (outputSFile,
        "\t.byte \"%s\"\n"
        "\t.byte 0\n\n",
        myHead.info);

    if (fclose (outputSFile) != 0) {
        AbEnd ("Error closing %s: %s", outputSName, strerror (errno));
    }
}


static void DoMemory (void)
{
    char *token;
    int stacksize, overlaysize;
    int overlaytable[127];
    int number, lastnumber;
    int backbuffer;

    openSFile ();

    stacksize = -1;
    overlaysize = -1;
    memset (overlaytable, 0, sizeof (overlaytable));
    lastnumber = -1;
    backbuffer = -1;

    if (strcmp (nextWord (), "{") != 0) {
        AbEnd ("MEMORY description has no opening bracket!");
    }

    token = NULL;
    for (;;) {
        if (token == NULL) token = nextWord ();
        if (strcmp (token, "}") == 0) break;
        switch (findToken (memFields, token)) {
            case -1:
                AbEnd ("Unknown field '%s' in MEMORY description", token);
                break;

            case 0: /* stacksize */
                stacksize = strtol (nextWord (), NULL, 0);
                token = NULL;
                break;

            case 1: /* overlaysize */
                overlaysize = strtol (nextWord (), NULL, 0);
                token = NULL;
                break;

            case 2: /* overlaynums */
                do {
                    token = nextWord ();
                    if (IsDigit (token[0]) == 0) break;

                    number = atoi (token);
                    if (number < 0 || number > 126) {
                        AbEnd ("Overlay number %i is out of range 0-126", number);
                    }
                    if (overlaytable[number] == 1) {
                        AbEnd ("Overlay number %i is defined twice", number);
                    }

                    overlaytable[number] = 1;
                    if (number > lastnumber) lastnumber = number;

                } while (IsDigit (token[0]) != 0);

                if (lastnumber == -1) {
                    AbEnd ("There must be at least one overlay number");
                }

                /* always include number 0 */
                overlaytable[0] = 1;
                break;

            case 3: /* backbuffer */
                switch (findToken (toggle, nextWord ())) {
                    case -1:
                        AbEnd ("Unknown value for 'backbuffer'");
                        break;
                    case 0:
                    case 1:
                    case 2:
                        backbuffer = 0;
                        break;
                    default:
                        backbuffer = 1;
                        break;
                }
                token = NULL;
                break;
        }
    }

    /* OK, all information is gathered, do flushout */

    if (lastnumber != -1) {
        fprintf (outputSFile,
            "\t\t.segment \"RECORDS\"\n\n");

        if (apple == 1) {

            for (number = 0; number <= lastnumber; number++) {
                fprintf (outputSFile,
                    "\t.byte %s\n",
                    overlaytable[number] == 1 ? "$00" : "$FF");
            }
            fprintf (outputSFile,
                "\n");

            for (number = 0; number <= lastnumber; number++) {
                if (overlaytable[number] == 1) {
                    fprintf (outputSFile,
                        "\t\t.segment \"VLIRIDX%i\"\n\n"
                        "\t.import __VLIR%i_START__, __VLIR%i_LAST__%s\n\n"
                        "\t.res  255\n"
                        "\t.byte .lobyte (__VLIR%i_LAST__ - __VLIR%i_START__%s)\n"
                        "\t.res  255\n"
                        "\t.byte .hibyte (__VLIR%i_LAST__ - __VLIR%i_START__%s)\n\n",
                        number, number, number,
                        number == 0 ? ", __BSS_SIZE__" : "",
                        number, number,
                        number == 0 ? " - __BSS_SIZE__" : "",
                        number, number,
                        number == 0 ? " - __BSS_SIZE__" : "");
                }
            }

        } else {

            for (number = 0; number <= lastnumber; number++) {
                if (overlaytable[number] == 1) {
                    fprintf (outputSFile,
                        "\t.import __VLIR%i_START__, __VLIR%i_LAST__%s\n",
                        number, number, number == 0 ? ", __BSS_SIZE__" : "");
                }
            }
            fprintf (outputSFile,
                "\n");

            for (number = 0; number <= lastnumber; number++) {
                if (overlaytable[number] == 1) {
                    fprintf (outputSFile,
                        "\t.byte .lobyte ((__VLIR%i_LAST__ - __VLIR%i_START__%s - 1) /    254) + 1\n"
                        "\t.byte .lobyte ((__VLIR%i_LAST__ - __VLIR%i_START__%s - 1) .MOD 254) + 2\n",
                        number, number, number == 0 ? " - __BSS_SIZE__" : "",
                        number, number, number == 0 ? " - __BSS_SIZE__" : "");
                } else {
                    fprintf (outputSFile,
                        "\t.byte $00\n"
                        "\t.byte $FF\n");
                }
            }
            fprintf (outputSFile,
                "\n");
        }

        openCFile ();

        fprintf (outputCFile,
            "extern void _OVERLAYADDR__[];\n"
            "extern void _OVERLAYSIZE__[];\n\n"
            "#define OVERLAY_ADDR (char*)   _OVERLAYADDR__\n"
            "#define OVERLAY_SIZE (unsigned)_OVERLAYSIZE__\n\n");

        if (fclose (outputCFile) != 0) {
            AbEnd ("Error closing %s: %s", outputCName, strerror (errno));
        }
    }

    if (stacksize != -1) {
        fprintf (outputSFile,
            "\t.export __STACKSIZE__ : absolute = $%04x\n\n",
            stacksize);
    }

    if (overlaysize != -1) {
        fprintf (outputSFile,
            "\t.export __OVERLAYSIZE__ : absolute = $%04x\n\n",
            overlaysize);
    }

    if (backbuffer != -1) {
        fprintf (outputSFile,
            "\t.export __BACKBUFSIZE__ : absolute = $%04x\n\n",
            backbuffer ? 0x2000 : 0x0000);
    }

    if (fclose (outputSFile) != 0) {
        AbEnd ("Error closing %s: %s", outputSName, strerror (errno));
    }
}


static char *filterInput (FILE *F, char *tbl)
{
    /* loads file into buffer filtering it out */
    int a, prevchar = -1, i = 0, bracket = 0, quote = 1;

    for (;;) {
        a = getc(F);
        if ((a == '\n') || (a == '\015')) a = ' ';
        if (a == ',' && quote) a = ' ';
        if (a == '\042') quote =! quote;
        if (quote) {
            if ((a == '{') || (a == '(')) bracket++;
            if ((a == '}') || (a == ')')) bracket--;
        }
        if (a == EOF) {
            tbl[i] = '\0';
            xrealloc (tbl, i + 1);
            break;
        }
        if (IsSpace (a)) {
            if ((prevchar != ' ') && (prevchar != -1)) {
                tbl[i++] = ' ';
                prevchar = ' ';
            }
        } else {
            if (a == ';' && quote) {
                do {
                    a = getc (F);
                } while (a != '\n');
                fseek (F, -1, SEEK_CUR);
            } else {
                tbl[i++] = a;
                prevchar = a;
            }
        }
    }

    if (bracket != 0) AbEnd ("There are unclosed brackets!");

    return tbl;
}


static void processFile (const char *filename)
{
    FILE *F;

    char *str;
    char *token;

    int head = 0;   /* number of processed HEADER sections */
    int memory = 0; /* number of processed MEMORY sections */

    if ((F = fopen (filename, "r")) == 0) {
        AbEnd ("Can't open file %s for reading: %s", filename, strerror (errno));
    }

    str = filterInput (F, xmalloc (BLOODY_BIG_BUFFER));

    token = strtok (str, " ");

    do {
        if (str != NULL) {
            switch (findToken (mainToken, token)) {
                case 0:
                    DoMenu ();
                    break;
                case 1:
                    if (++head != 1) {
                        AbEnd ("More than one HEADER section, aborting");
                    } else {
                        DoHeader ();
                    }
                    break;
                case 2: break; /* icon not implemented yet */
                case 3: break; /* dialog not implemented yet */
                case 4:
                    if (++memory != 1) {
                        AbEnd ("More than one MEMORY section, aborting");
                    } else {
                        DoMemory ();
                    }
                    break;
                default:
                    AbEnd ("Unknown section '%s'", token);
                    break;
            }
        }
        token = nextWord ();
    } while (token != NULL);
}


int main (int argc, char *argv[])
{
    /* Program long options */
    static const LongOpt OptTab[] = {
        { "--help",    0, OptHelp},
        { "--target",  1, OptTarget},
        { "--version", 0, OptVersion},
    };

    unsigned ffile = 0;
    unsigned I;

    /* Initialize the cmdline module */
    InitCmdLine (&argc, &argv, "grc65");

    /* Check the parameters */
    I = 1;
    while (I < ArgCount) {

        /* Get the argument */
        const char* Arg = ArgVec [I];

        /* Check for an option */
        if (Arg[0] == '-') {
            switch (Arg[1]) {

                case '-':
                    LongOption (&I, OptTab, sizeof(OptTab)/sizeof(OptTab[0]));
                    break;

                case 'o':
                    outputCName = GetArg (&I, 2);
                    break;

                case 's':
                    outputSName = GetArg (&I, 2);
                    break;

                case 't':
                    OptTarget (Arg, GetArg (&I, 2));
                    break;

                case 'h':
                case '?':
                    OptHelp (Arg, 0);
                    break;

                case 'V':
                    OptVersion (Arg, 0);
                    break;

                default:
                    UnknownOption (Arg);
            }

        } else {
            ffile++;

            if (outputCName == NULL) outputCName = MakeFilename (Arg, ".h");
            if (outputSName == NULL) outputSName = MakeFilename (Arg, ".s");

            processFile (Arg);
        }

        /* Next argument */
        ++I;
    }

    if (ffile == 0) AbEnd ("No input file");

    return EXIT_SUCCESS;
}
