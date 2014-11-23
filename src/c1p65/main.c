/* Object file conversion utility for Challenger 1P

   by Stephan Muehlstrasser
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

static void Usage (void)
{
    printf (
        "Usage: %s [options] file\n"
        "Short options:\n"
        "  -V\t\t\tPrint the version number\n"
        "  -h\t\t\tHelp (this text)\n"
		"  -n\t\tNo automatic start after loading program\n"
		"  -o name\t\tName the C1P output file (default: <input>.c1p)\n"
		"  -S addr\t\tLoad address (default 0x300)\n"
        "\n"
        "Long options:\n"
        "  --help\t\tHelp (this text)\n"
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


static void OptVersion (const char* Opt attribute ((unused)),
                        const char* Arg attribute ((unused)))
/* Print the program version */
{
    fprintf (stderr, "grc65 V%s\n", GetVersionAsString ());
}


static unsigned long CvtNumber (const char* Arg, const char* Number)
/* Convert a number from a string. Allow '$' and '0x' prefixes for hex
 * numbers. Duplicated from ld65's main.c.
 */
{
    unsigned long Val;
    int           Converted;

    /* Convert */
    if (*Number == '$') {
        ++Number;
        Converted = sscanf (Number, "%lx", &Val);
    } else {
        Converted = sscanf (Number, "%li", (long*)&Val);
    }

    /* Check if we do really have a number */
    if (Converted != 1) {
        AbEnd ("Invalid number given in argument: %s\n", Arg);
    }

    /* Return the result */
    return Val;
}

/* Commands of C1P PROM monitor */
#define ADDRESS_MODE_CMD	'.'
#define DATA_MODE_CMD		'/'
#define EXECUTE_CMD			'G'
#define DATA_MODE_ADDRESS	0x00FB

/* Transform the cc65 executable binary into a series of
   commands that make the C1P PROM monitor load the bytes
   into memory.
*/
static void Transform (unsigned long StartAddress, FILE *In, FILE *Out,
	unsigned AutoStart)
{
	int c;

	/* Position to the start address */
	fprintf(Out, "%c%04.4X%c", ADDRESS_MODE_CMD,
		StartAddress & 0xFFFF, DATA_MODE_CMD);

	/* Loop over all input bytes and enter them one by one */
	for (c = getc(In); c != EOF; c = getc(In)) {
		fprintf(Out, "%02.2X\n", (unsigned int) c & 0xFF);
	}

	if (AutoStart) {
		/* Execute */
		fprintf (Out, "%c%04.4x%c",
			ADDRESS_MODE_CMD, (unsigned int) StartAddress & 0xFFFF,
			EXECUTE_CMD);
	}
	else {
		/* Store 00 to 0x00FB to enable keyboard input at the end */
		fprintf(Out, "%c%04.4X%c%02.2X\n", ADDRESS_MODE_CMD,
			0x00FB, DATA_MODE_CMD, 0x00);
	}
} 

/* Default suffix for C1P object file */
#define C1P_SUFFIX ".c1p"

int main (int argc, char *argv[])
{
    /* Program long options */
    static const LongOpt OptTab[] = {
        { "--help",    0, OptHelp},
        { "--version", 0, OptVersion},
    };

	/* Initialize input and output file name */
    const char* InputFile = 0;
    const char* OutputFile = 0;
	char *GeneratedOutputFile = 0;

	/* Initialize file pointers */
	FILE *InputFileFp = 0;
	FILE *OutputFileFp = 0;

	/* Initialize with default start address defined in c1p.cfg */
	unsigned long StartAddr = 0x300;

	/* Start program automatically after loading */
	unsigned AutoStart = 1;

    unsigned int I;

    /* Initialize the cmdline module */
    InitCmdLine (&argc, &argv, "c1p65");

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

				case 'n':
					AutoStart = 0;
					break;

				case 'o':
					OutputFile = GetArg(&I, 2);
					break;

				case 'S':
                    StartAddr = CvtNumber (Arg, GetArg (&I, 2));
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
            if (InputFile) {
                fprintf (stderr, "additional file specs ignored\n");
            } else {
                InputFile = Arg;
            }
        }

        /* Next argument */
        ++I;
    }

    if (!InputFile) AbEnd ("No input file");

	if (!OutputFile) {
		const size_t len = strlen(InputFile) + sizeof(C1P_SUFFIX);
		
		GeneratedOutputFile = (char *) xmalloc(len);
		sprintf(GeneratedOutputFile, "%s%s", InputFile, C1P_SUFFIX);
		OutputFile = GeneratedOutputFile;
	}

	/* Open input and output files */
	InputFileFp = fopen(InputFile, "rb");
	if (!InputFileFp) AbEnd ("Unable to open input file");

	OutputFileFp = fopen(OutputFile, "wb");
	if (!OutputFileFp) AbEnd ("Unable to open output file");

	/* Generate object file */
	Transform (StartAddr, InputFileFp, OutputFileFp, AutoStart);

	/* Cleanup */
	if (fclose(InputFileFp) == EOF) AbEnd ("Error closing input file");

	if (fclose(OutputFileFp) == EOF) AbEnd ("Error closing output file");

	if (GeneratedOutputFile) {
		xfree(GeneratedOutputFile);
	}

    return EXIT_SUCCESS;
}
