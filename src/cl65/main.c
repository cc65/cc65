/*****************************************************************************/
/*									     */
/*				    main.c				     */
/*									     */
/*	       Main module for the cl65 compile and link utility	     */
/*									     */
/*									     */
/*									     */
/* (C) 1999-2004 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
/*									     */
/*									     */
/* This software is provided 'as-is', without any expressed or implied	     */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.				     */
/*									     */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:			     */
/*									     */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.					     */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.			     */
/* 3. This notice may not be removed or altered from any source		     */
/*    distribution.							     */
/*									     */
/*****************************************************************************/



/* Check out if we have a spawn() function on the system, or if we must use
 * our own.
 */
#if defined(__WATCOMC__) || defined(_MSC_VER) || defined(__MINGW32__) || defined(__DJGPP__)
#  define HAVE_SPAWN    1
#else
#  define NEED_SPAWN   1
#endif



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#ifdef HAVE_SPAWN
#  include <process.h>
#endif

/* common */
#include "attrib.h"
#include "cmdline.h"
#include "filetype.h"
#include "fname.h"
#include "mmodel.h"
#include "strbuf.h"
#include "target.h"
#include "version.h"
#include "xmalloc.h"

/* cl65 */
#include "global.h"
#include "error.h"



/*****************************************************************************/
/*	   			     Data				     */
/*****************************************************************************/



/* Struct that describes a command */
typedef struct CmdDesc CmdDesc;
struct CmdDesc {
    char*	Name;		/* The command name */

    unsigned	ArgCount;	/* Count of arguments */
    unsigned	ArgMax;		/* Maximum count of arguments */
    char**	Args;		/* The arguments */

    unsigned	FileCount;	/* Count of files to translate */
    unsigned	FileMax;	/* Maximum count of files */
    char**	Files;		/* The files */
};

/* Command descriptors for the different programs */
static CmdDesc CC65 = { 0, 0, 0, 0, 0, 0, 0 };
static CmdDesc CA65 = { 0, 0, 0, 0, 0, 0, 0 };
static CmdDesc CO65 = { 0, 0, 0, 0, 0, 0, 0 };
static CmdDesc LD65 = { 0, 0, 0, 0, 0, 0, 0 };
static CmdDesc GRC  = { 0, 0, 0, 0, 0, 0, 0 };

/* Variables controlling the steps we're doing */
static int DontLink	= 0;
static int DontAssemble = 0;

/* The name of the output file, NULL if none given */
static const char* OutputName = 0;

/* The name of the linker configuration file if given */
static const char* LinkerConfig = 0;

/* The name of the first input file. This will be used to construct the
 * executable file name if no explicit name is given.
 */
static const char* FirstInput = 0;

/* Remember if we should link a module */
static int Module = 0;

/* Extension used for a module */
#define MODULE_EXT      ".o65"

/* Name of the crt0 object file and the runtime library */
static char* TargetCRT0 = 0;
static char* TargetLib	= 0;



/*****************************************************************************/
/*                Include the system specific spawn function                 */
/*****************************************************************************/



#if defined(NEED_SPAWN)
#  if defined(SPAWN_UNIX)
#    include "spawn-unix.inc"
#  elif defined(SPAWN_AMIGA)
#    include "spawn-amiga.inc"
#  else
#    error "Don't know which spawn module to include!"
#  endif
#endif



/*****************************************************************************/
/*			  Command structure handling			     */
/*****************************************************************************/



static void CmdAddArg (CmdDesc* Cmd, const char* Arg)
/* Add a new argument to the command */
{
    /* Expand the argument vector if needed */
    if (Cmd->ArgCount == Cmd->ArgMax) {
	unsigned NewMax  = Cmd->ArgMax + 10;
	char**	 NewArgs = xmalloc (NewMax * sizeof (char*));
	memcpy (NewArgs, Cmd->Args, Cmd->ArgMax * sizeof (char*));
	xfree (Cmd->Args);
	Cmd->Args   = NewArgs;
	Cmd->ArgMax = NewMax;
    }

    /* Add a copy of the new argument, allow a NULL pointer */
    if (Arg) {
	Cmd->Args [Cmd->ArgCount++] = xstrdup (Arg);
    } else {
	Cmd->Args [Cmd->ArgCount++] = 0;
    }
}



static void CmdAddArg2 (CmdDesc* Cmd, const char* Arg1, const char* Arg2)
/* Add a new argument pair to the command */
{
    CmdAddArg (Cmd, Arg1);
    CmdAddArg (Cmd, Arg2);
}



static void CmdDelArgs (CmdDesc* Cmd, unsigned LastValid)
/* Remove all arguments with an index greater than LastValid */
{
    while (Cmd->ArgCount > LastValid) {
	Cmd->ArgCount--;
	xfree (Cmd->Args [Cmd->ArgCount]);
	Cmd->Args [Cmd->ArgCount] = 0;
    }
}



static void CmdAddFile (CmdDesc* Cmd, const char* File)
/* Add a new file to the command */
{
    /* Expand the file vector if needed */
    if (Cmd->FileCount == Cmd->FileMax) {
	unsigned NewMax   = Cmd->FileMax + 10;
	char**	 NewFiles = xmalloc (NewMax * sizeof (char*));
	memcpy (NewFiles, Cmd->Files, Cmd->FileMax * sizeof (char*));
	xfree (Cmd->Files);
	Cmd->Files   = NewFiles;
	Cmd->FileMax = NewMax;
    }

    /* If the file name is not NULL (which is legal and is used to terminate
     * the file list), check if the file name does already exist in the file
     * list and print a warning if so. Regardless of the search result, add
     * the file.
     */
    if (File) {
	unsigned I;
	for (I = 0; I < Cmd->FileCount; ++I) {
	    if (strcmp (Cmd->Files[I], File) == 0) {
	     	/* Duplicate file */
		Warning ("Duplicate file in argument list: `%s'", File);
		/* No need to search further */
	     	break;
	    }
	}

	/* Add the file */
	Cmd->Files [Cmd->FileCount++] = xstrdup (File);
    } else {
	/* Add a NULL pointer */
	Cmd->Files [Cmd->FileCount++] = 0;
    }
}



static void CmdInit (CmdDesc* Cmd, const char* Path)
/* Initialize the command using the given path to the executable */
{
    /* Remember the command */
    Cmd->Name = xstrdup (Path);

    /* Use the command name as first argument */
    CmdAddArg (Cmd, Path);
}



static void CmdSetOutput (CmdDesc* Cmd, const char* File)
/* Set the output file in a command desc */
{
    CmdAddArg2 (Cmd, "-o", File);
}



static void CmdSetTarget (CmdDesc* Cmd, target_t Target)
/* Set the output file in a command desc */
{
    CmdAddArg2 (Cmd, "-t", TargetNames[Target]);
}



static void CmdPrint (CmdDesc* Cmd, FILE* F)
/* Output the command line encoded in the command desc */
{
    unsigned I;
    for (I = 0; I < Cmd->ArgCount && Cmd->Args[I] != 0; ++I) {
	fprintf (F, "%s ", Cmd->Args[I]);
    }
}



/*****************************************************************************/
/*   	   		     	Target handling				     */
/*****************************************************************************/



static void SetTargetFiles (void)
/* Set the target system files */
{
    /* Determine the names of the default startup and library file */
    if (Target != TGT_NONE) {

 	/* Get a pointer to the system name and its length */
 	const char* TargetName = TargetNames [Target];
 	unsigned    TargetNameLen = strlen (TargetName);

 	/* Set the startup file */
 	TargetCRT0 = xmalloc (TargetNameLen + 2 + 1);
       	memcpy (TargetCRT0, TargetName, TargetNameLen);
       	strcpy (TargetCRT0 + TargetNameLen, ".o");

 	/* Set the library file */
 	TargetLib = xmalloc (TargetNameLen + 4 + 1);
 	memcpy (TargetLib, TargetName, TargetNameLen);
 	strcpy (TargetLib + TargetNameLen, ".lib");

    }
}



/*****************************************************************************/
/*	    		   	 Subprocesses				     */
/*****************************************************************************/



static void ExecProgram (CmdDesc* Cmd)
/* Execute a subprocess with the given name/parameters. Exit on errors. */
{
    int Status;

    /* If in debug mode, output the command line we will execute */
    if (Debug) {
	printf ("Executing: ");
       	CmdPrint (Cmd, stdout);
	printf ("\n");
    }

    /* Call the program */
    Status = spawnvp (P_WAIT, Cmd->Name, Cmd->Args);

    /* Check the result code */
    if (Status < 0) {
	/* Error executing the program */
	Error ("Cannot execute `%s': %s", Cmd->Name, strerror (errno));
    } else if (Status != 0) {
	/* Called program had an error */
	exit (Status);
    }
}



static void Link (void)
/* Link the resulting executable */
{
    unsigned I;

    /* If we have a linker config file given, add it to the command line.
     * Otherwise pass the target to the linker if we have one.
     */
    if (LinkerConfig) {
        if (Module) {
            Error ("Cannot use -C and --module together");
        }
       	CmdAddArg2 (&LD65, "-C", LinkerConfig);
    } else if (Module) {
        CmdSetTarget (&LD65, TGT_MODULE);
    } else {
	CmdSetTarget (&LD65, Target);
    }

    /* Determine which target libraries are needed */
    SetTargetFiles ();

    /* Since linking is always the final step, if we have an output file name
     * given, set it here. If we don't have an explicit output name given,
     * try to build one from the name of the first input file.
     */
    if (OutputName) {

    	CmdSetOutput (&LD65, OutputName);

    } else if (FirstInput && FindExt (FirstInput)) {  /* Only if ext present! */

        const char* Extension = Module? MODULE_EXT : "";
    	char* Output = MakeFilename (FirstInput, Extension);
    	CmdSetOutput (&LD65, Output);
    	xfree (Output);

    }

    /* If we have a startup file and if we are not linking a module, add its
     * name as a parameter
     */
    if (TargetCRT0 && !Module) {
    	CmdAddArg (&LD65, TargetCRT0);
    }

    /* Add all object files as parameters */
    for (I = 0; I < LD65.FileCount; ++I) {
    	CmdAddArg (&LD65, LD65.Files [I]);
    }

    /* Add the system runtime library */
    if (TargetLib) {
	CmdAddArg (&LD65, TargetLib);
    }

    /* Terminate the argument list with a NULL pointer */
    CmdAddArg (&LD65, 0);

    /* Call the linker */
    ExecProgram (&LD65);
}



static void Assemble (const char* File)
/* Assemble the given file */
{
    /* Remember the current assembler argument count */
    unsigned ArgCount = CA65.ArgCount;

    /* Set the target system */
    CmdSetTarget (&CA65, Target);

    /* If we won't link, this is the final step. In this case, set the
     * output name.
     */
    if (DontLink && OutputName) {
	CmdSetOutput (&CA65, OutputName);
    } else {
	/* The object file name will be the name of the source file
	 * with .s replaced by ".o". Add this file to the list of
	 * linker files.
	 */
	char* ObjName = MakeFilename (File, ".o");
	CmdAddFile (&LD65, ObjName);
	xfree (ObjName);
    }

    /* Add the file as argument for the assembler */
    CmdAddArg (&CA65, File);

    /* Add a NULL pointer to terminate the argument list */
    CmdAddArg (&CA65, 0);

    /* Run the assembler */
    ExecProgram (&CA65);

    /* Remove the excess arguments */
    CmdDelArgs (&CA65, ArgCount);
}



static void Compile (const char* File)
/* Compile the given file */
{
    char* AsmName = 0;

    /* Remember the current compiler argument count */
    unsigned ArgCount = CC65.ArgCount;

    /* Set the target system */
    CmdSetTarget (&CC65, Target);

    /* If we won't link, this is the final step. In this case, set the
     * output name.
     */
    if (DontAssemble && OutputName) {
	CmdSetOutput (&CC65, OutputName);
    } else {
	/* The assembler file name will be the name of the source file
	 * with .c replaced by ".s".
	 */
	AsmName = MakeFilename (File, ".s");
    }

    /* Add the file as argument for the compiler */
    CmdAddArg (&CC65, File);

    /* Add a NULL pointer to terminate the argument list */
    CmdAddArg (&CC65, 0);

    /* Run the compiler */
    ExecProgram (&CC65);

    /* Remove the excess arguments */
    CmdDelArgs (&CC65, ArgCount);

    /* If this is not the final step, assemble the generated file, then
     * remove it
     */
    if (!DontAssemble) {
	Assemble (AsmName);
	if (remove (AsmName) < 0) {
	    Warning ("Cannot remove temporary file `%s': %s",
		     AsmName, strerror (errno));
	}
	xfree (AsmName);
    }
}



static void CompileRes (const char* File)
/* Compile the given geos resource file */
{
    char* AsmName = 0;

    /* Remember the current assembler argument count */
    unsigned ArgCount = GRC.ArgCount;

    /* The assembler file name will be the name of the source file
     * with .grc replaced by ".s".
     */
    AsmName = MakeFilename (File, ".s");

    /* Add the file as argument for the resource compiler */
    CmdAddArg (&GRC, File);

    /* Add a NULL pointer to terminate the argument list */
    CmdAddArg (&GRC, 0);

    /* Run the compiler */
    ExecProgram (&GRC);

    /* Remove the excess arguments */
    CmdDelArgs (&GRC, ArgCount);

    /* If this is not the final step, assemble the generated file, then
     * remove it
     */
    if (!DontAssemble) {
	Assemble (AsmName);
	if (remove (AsmName) < 0) {
	    Warning ("Cannot remove temporary file `%s': %s",
		     AsmName, strerror (errno));
	}
    }

    /* Free the assembler file name which was allocated from the heap */
    xfree (AsmName);
}



static void ConvertO65 (const char* File)
/* Convert an o65 object file into an assembler file */
{
    char* AsmName = 0;

    /* Remember the current converter argument count */
    unsigned ArgCount = CO65.ArgCount;

    /* If we won't link, this is the final step. In this case, set the
     * output name.
     */
    if (DontAssemble && OutputName) {
	CmdSetOutput (&CO65, OutputName);
    } else {
	/* The assembler file name will be the name of the source file
	 * with .c replaced by ".s".
	 */
	AsmName = MakeFilename (File, ".s");
    }

    /* Add the file as argument for the object file converter */
    CmdAddArg (&CO65, File);

    /* Add a NULL pointer to terminate the argument list */
    CmdAddArg (&CO65, 0);

    /* Run the converter */
    ExecProgram (&CO65);

    /* Remove the excess arguments */
    CmdDelArgs (&CO65, ArgCount);

    /* If this is not the final step, assemble the generated file, then
     * remove it
     */
    if (!DontAssemble) {
	Assemble (AsmName);
	if (remove (AsmName) < 0) {
	    Warning ("Cannot remove temporary file `%s': %s",
		     AsmName, strerror (errno));
	}
    }

    /* Free the assembler file name which was allocated from the heap */
    xfree (AsmName);
}



/*****************************************************************************/
/*		    	       	     Code				     */
/*****************************************************************************/



static void Usage (void)
/* Print usage information and exit */
{
    fprintf (stderr,
	     "Usage: %s [options] file [...]\n"
       	     "Short options:\n"
       	     "  -c\t\t\tCompile and assemble but don't link\n"
       	     "  -d\t\t\tDebug mode\n"
       	     "  -g\t\t\tAdd debug info\n"
       	     "  -h\t\t\tHelp (this text)\n"
       	     "  -l\t\t\tCreate an assembler listing\n"
       	     "  -m name\t\tCreate a map file\n"
             "  -mm model\t\tSet the memory model\n"
       	     "  -o name\t\tName the output file\n"
             "  -r\t\t\tEnable register variables\n"
       	     "  -t sys\t\tSet the target system\n"
       	     "  -v\t\t\tVerbose mode\n"
       	     "  -vm\t\t\tVerbose map file\n"
       	     "  -A\t\t\tStrict ANSI mode\n"
       	     "  -C name\t\tUse linker config file\n"
       	     "  -Cl\t\t\tMake local variables static\n"
       	     "  -D sym[=defn]\t\tDefine a preprocessor symbol\n"
       	     "  -I dir\t\tSet a compiler include directory path\n"
             "  -L path\t\tSpecify a library search path\n"
       	     "  -Ln name\t\tCreate a VICE label file\n"
       	     "  -O\t\t\tOptimize code\n"
       	     "  -Oi\t\t\tOptimize code, inline functions\n"
       	     "  -Or\t\t\tOptimize code, honour the register keyword\n"
       	     "  -Os\t\t\tOptimize code, inline known C funtions\n"
       	     "  -S\t\t\tCompile but don't assemble and link\n"
       	     "  -T\t\t\tInclude source as comment\n"
       	     "  -V\t\t\tPrint the version number\n"
       	     "  -W\t\t\tSuppress warnings\n"
	     "\n"
	     "Long options:\n"
       	     "  --add-source\t\tInclude source as comment\n"
       	     "  --ansi\t\tStrict ANSI mode\n"
       	     "  --asm-define sym[=v]\tDefine an assembler symbol\n"
	     "  --asm-include-dir dir\tSet an assembler include directory\n"
             "  --bss-label name\tDefine and export a BSS segment label\n"
	     "  --bss-name seg\tSet the name of the BSS segment\n"
             "  --cfg-path path\tSpecify a config file search path\n"
       	     "  --check-stack\t\tGenerate stack overflow checks\n"
             "  --code-label name\tDefine and export a CODE segment label\n"
       	     "  --code-name seg\tSet the name of the CODE segment\n"
	     "  --codesize x\t\tAccept larger code by factor x\n"
       	     "  --config name\t\tUse linker config file\n"
       	     "  --cpu type\t\tSet cpu type\n"
	     "  --create-dep\t\tCreate a make dependency file\n"
             "  --data-label name\tDefine and export a DATA segment label\n"
       	     "  --data-name seg\tSet the name of the DATA segment\n"
       	     "  --debug\t\tDebug mode\n"
       	     "  --debug-info\t\tAdd debug info\n"
	     "  --feature name\tSet an emulation feature\n"
       	     "  --forget-inc-paths\tForget include search paths (compiler)\n"
       	     "  --help\t\tHelp (this text)\n"
       	     "  --include-dir dir\tSet a compiler include directory path\n"
             "  --lib file\t\tLink this library\n"
             "  --lib-path path\tSpecify a library search path\n"
	     "  --list-targets\tList all available targets\n"
       	     "  --listing\t\tCreate an assembler listing\n"
	     "  --mapfile name\tCreate a map file\n"
             "  --memory-model model\tSet the memory model\n"
             "  --module\t\tLink as a module\n"
             "  --module-id id\tSpecify a module id for the linker\n"
             "  --o65-model model\tOverride the o65 model\n"
             "  --obj file\t\tLink this object file\n"
             "  --obj-path path\tSpecify an object file search path\n"
             "  --register-space b\tSet space available for register variables\n"
             "  --register-vars\tEnable register variables\n"
       	     "  --rodata-name seg\tSet the name of the RODATA segment\n"
       	     "  --signed-chars\tDefault characters are signed\n"
       	     "  --start-addr addr\tSet the default start address\n"
       	     "  --static-locals\tMake local variables static\n"
       	     "  --target sys\t\tSet the target system\n"
       	     "  --version\t\tPrint the version number\n"
       	     "  --verbose\t\tVerbose mode\n"
             "  --zeropage-label name\tDefine and export a ZEROPAGE segment label\n"
       	     "  --zeropage-name seg\tSet the name of the ZEROPAGE segment\n",
    	     ProgName);
}



static void OptAddSource (const char* Opt attribute ((unused)),
			  const char* Arg attribute ((unused)))
/* Strict source code as comments to the generated asm code */
{
    CmdAddArg (&CC65, "-T");
}



static void OptAnsi (const char* Opt attribute ((unused)),
		     const char* Arg attribute ((unused)))
/* Strict ANSI mode (compiler) */
{
    CmdAddArg (&CC65, "-A");
}



static void OptAsmDefine (const char* Opt attribute ((unused)), const char* Arg)
/* Define an assembler symbol (assembler) */
{
    CmdAddArg2 (&CA65, "-D", Arg);
}



static void OptAsmIncludeDir (const char* Opt attribute ((unused)), const char* Arg)
/* Include directory (assembler) */
{
    CmdAddArg2 (&CA65, "-I", Arg);
}



static void OptBssLabel (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --bss-label option */
{
    CmdAddArg2 (&CO65, "--bss-label", Arg);
}



static void OptBssName (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --bss-name option */
{
    CmdAddArg2 (&CC65, "--bss-name", Arg);
    CmdAddArg2 (&CO65, "--bss-name", Arg);
}



static void OptCfgPath (const char* Opt attribute ((unused)), const char* Arg)
/* Config file search path (linker) */
{
    CmdAddArg2 (&LD65, "--cfg-path", Arg);
}



static void OptCheckStack (const char* Opt attribute ((unused)),
		  	   const char* Arg attribute ((unused)))
/* Handle the --check-stack option */
{
    CmdAddArg (&CC65, "--check-stack");
}



static void OptCodeLabel (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --code-label option */
{
    CmdAddArg2 (&CO65, "--code-label", Arg);
}



static void OptCodeName (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --code-name option */
{
    CmdAddArg2 (&CC65, "--code-name", Arg);
    CmdAddArg2 (&CO65, "--code-name", Arg);
}



static void OptCodeSize (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --codesize option */
{
    CmdAddArg2 (&CC65, "--codesize", Arg);
}



static void OptConfig (const char* Opt attribute ((unused)), const char* Arg)
/* Config file (linker) */
{
    if (LinkerConfig) {
        Error ("Cannot specify -C/--config twice");
    }
    LinkerConfig = Arg;
}



static void OptCPU (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --cpu option */
{
    /* Add the cpu type to the assembler and compiler */
    CmdAddArg2 (&CA65, "--cpu", Arg);
    CmdAddArg2 (&CC65, "--cpu", Arg);
}



static void OptCreateDep (const char* Opt attribute ((unused)),
			  const char* Arg attribute ((unused)))
/* Handle the --create-dep option */
{
    CmdAddArg (&CC65, "--create-dep");
}



static void OptDataLabel (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --data-label option */
{
    CmdAddArg2 (&CO65, "--data-label", Arg);
}



static void OptDataName (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --data-name option */
{
    CmdAddArg2 (&CC65, "--data-name", Arg);
    CmdAddArg2 (&CO65, "--data-name", Arg);
}



static void OptDebug (const char* Opt attribute ((unused)),
	    	      const char* Arg attribute ((unused)))
/* Debug mode (compiler and cl65 utility) */
{
    CmdAddArg (&CC65, "-d");
    CmdAddArg (&CO65, "-d");
    Debug = 1;
}



static void OptDebugInfo (const char* Opt attribute ((unused)),
			  const char* Arg attribute ((unused)))
/* Debug Info - add to compiler and assembler */
{
    CmdAddArg (&CC65, "-g");
    CmdAddArg (&CA65, "-g");
    CmdAddArg (&CO65, "-g");
}



static void OptFeature (const char* Opt attribute ((unused)), const char* Arg)
/* Emulation features for the assembler */
{
    CmdAddArg2 (&CA65, "--feature", Arg);
}



static void OptForgetIncPaths (const char* Opt attribute ((unused)), const char* Arg)
/* Forget all currently defined include paths */
{
    CmdAddArg2 (&CC65, "--forget-inc-paths", Arg);
}



static void OptHelp (const char* Opt attribute ((unused)),
		     const char* Arg attribute ((unused)))
/* Print help - cl65 */
{
    Usage ();
    exit (EXIT_SUCCESS);
}



static void OptIncludeDir (const char* Opt attribute ((unused)), const char* Arg)
/* Include directory (compiler) */
{
    CmdAddArg2 (&CC65, "-I", Arg);
}



static void OptLib (const char* Opt attribute ((unused)), const char* Arg)
/* Library file follows (linker) */
{
    CmdAddArg2 (&LD65, "--lib", Arg);
}



static void OptLibPath (const char* Opt attribute ((unused)), const char* Arg)
/* Library search path (linker) */
{
    CmdAddArg2 (&LD65, "--lib-path", Arg);
}



static void OptListing (const char* Opt attribute ((unused)),
			const char* Arg attribute ((unused)))
/* Create an assembler listing */
{
    CmdAddArg (&CA65, "-l");
}



static void OptListTargets (const char* Opt attribute ((unused)),
			    const char* Arg attribute ((unused)))
/* List all targets */
{
    unsigned I;

    /* List the targets */
    for (I = TGT_NONE; I < TGT_COUNT; ++I) {
	printf ("%s\n", TargetNames[I]);
    }

    /* Terminate */
    exit (EXIT_SUCCESS);
}



static void OptMapFile (const char* Opt attribute ((unused)), const char* Arg)
/* Create a map file */
{
    /* Create a map file (linker) */
    CmdAddArg2 (&LD65, "-m", Arg);
}



static void OptMemoryModel (const char* Opt attribute ((unused)), const char* Arg)
/* Set the memory model */
{
    mmodel_t MemoryModel = FindMemoryModel (Arg);
    if (MemoryModel == MMODEL_UNKNOWN) {
        Error ("Unknown memory model: %s", Arg);
    } else if (MemoryModel == MMODEL_HUGE) {
        Error ("Unsupported memory model: %s", Arg);
    } else {
        CmdAddArg2 (&CA65, "-mm", Arg);
        CmdAddArg2 (&CC65, "-mm", Arg);
    }
}



static void OptModule (const char* Opt attribute ((unused)),
                       const char* Arg attribute ((unused)))
/* Link as a module */
{
    Module = 1;
}



static void OptModuleId (const char* Opt attribute ((unused)), const char* Arg)
/* Specify a module if for the linker */
{
    /* Pass it straight to the linker */
    CmdAddArg2 (&LD65, "--module-id", Arg);
}



static void OptO65Model (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --o65-model option */
{
    CmdAddArg2 (&CO65, "-m", Arg);
}



static void OptObj (const char* Opt attribute ((unused)), const char* Arg)
/* Object file follows (linker) */
{
    CmdAddArg2 (&LD65, "--obj", Arg);
}



static void OptObjPath (const char* Opt attribute ((unused)), const char* Arg)
/* Object file search path (linker) */
{
    CmdAddArg2 (&LD65, "--obj-path", Arg);
}



static void OptRegisterSpace (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --register-space option */
{
    CmdAddArg2 (&CC65, "--register-space", Arg);
}



static void OptRegisterVars (const char* Opt attribute ((unused)),
                             const char* Arg attribute ((unused)))
/* Handle the --register-vars option */
{
    CmdAddArg (&CC65, "-r");
}



static void OptRodataName (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --rodata-name option */
{
    CmdAddArg2 (&CC65, "--rodata-name", Arg);
}



static void OptSignedChars (const char* Opt attribute ((unused)),
			    const char* Arg attribute ((unused)))
/* Make default characters signed */
{
    CmdAddArg (&CC65, "-j");
}



static void OptStartAddr (const char* Opt attribute ((unused)), const char* Arg)
/* Set the default start address */
{
    CmdAddArg2 (&LD65, "-S", Arg);
}



static void OptStaticLocals (const char* Opt attribute ((unused)),
	      		     const char* Arg attribute ((unused)))
/* Place local variables in static storage */
{
    CmdAddArg (&CC65, "-Cl");
}



static void OptTarget (const char* Opt attribute ((unused)), const char* Arg)
/* Set the target system */
{
    Target = FindTarget (Arg);
    if (Target == TGT_UNKNOWN) {
        Error ("No such target system: `%s'", Arg);
    } else if (Target == TGT_MODULE) {
        Error ("Cannot use `module' as target, use --module instead");
    }
}



static void OptVerbose (const char* Opt attribute ((unused)),
			const char* Arg attribute ((unused)))
/* Verbose mode (compiler, assembler, linker) */
{
    CmdAddArg (&CC65, "-v");
    CmdAddArg (&CA65, "-v");
    CmdAddArg (&CO65, "-v");
    CmdAddArg (&LD65, "-v");
}



static void OptVersion (const char* Opt attribute ((unused)),
		    	const char* Arg attribute ((unused)))
/* Print version number */
{
    fprintf (stderr,
 	     "cl65 V%u.%u.%u - (C) Copyright 1998-2003 Ullrich von Bassewitz\n",
 	     VER_MAJOR, VER_MINOR, VER_PATCH);
}



static void OptZeropageLabel (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --zeropage-label option */
{
    CmdAddArg2 (&CO65, "--zeropage-label", Arg);
}



static void OptZeropageName (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --zeropage-name option */
{
    CmdAddArg2 (&CO65, "--zeropage-name", Arg);
}



int main (int argc, char* argv [])
/* Utility main program */
{
    /* Program long options */
    static const LongOpt OptTab[] = {
	{ "--add-source",	0,    	OptAddSource 		},
	{ "--ansi",		0,	OptAnsi			},
       	{ "--asm-define",      	1,     	OptAsmDefine            },
	{ "--asm-include-dir",	1,	OptAsmIncludeDir	},
       	{ "--bss-label",       	1,     	OptBssLabel             },
	{ "--bss-name",	 	1, 	OptBssName   		},
       	{ "--cfg-path",	       	1,     	OptCfgPath              },
       	{ "--check-stack",	0,     	OptCheckStack		},
       	{ "--code-label",      	1,     	OptCodeLabel            },
	{ "--code-name", 	1, 	OptCodeName  		},
	{ "--codesize",	 	1,	OptCodeSize		},
        { "--config",           1,      OptConfig               },
        { "--cpu",     	       	1, 	OptCPU 			},
	{ "--create-dep",    	0,	OptCreateDep 		},
       	{ "--data-label",      	1,     	OptDataLabel            },
	{ "--data-name",     	1, 	OptDataName  		},
	{ "--debug",	     	0,	OptDebug		},
	{ "--debug-info",    	0,	OptDebugInfo		},
	{ "--feature",	     	1,	OptFeature		},
       	{ "--forget-inc-paths",	0,     	OptForgetIncPaths       },
	{ "--help",	     	0,	OptHelp			},
	{ "--include-dir",   	1,	OptIncludeDir		},
       	{ "--lib",     	       	1,     	OptLib                  },
       	{ "--lib-path",	       	1,     	OptLibPath              },
	{ "--list-targets",	0,	OptListTargets		},
	{ "--listing",	      	0,	OptListing		},
	{ "--mapfile",	      	1,	OptMapFile		},
        { "--memory-model",     1,      OptMemoryModel          },
        { "--module",           0,      OptModule               },
        { "--module-id",        1,      OptModuleId             },
        { "--o65-model",        1,      OptO65Model             },
       	{ "--obj",              1,     	OptObj                  },
       	{ "--obj-path",	       	1,     	OptObjPath              },
        { "--register-space",   1,      OptRegisterSpace        },
        { "--register-vars",    0,      OptRegisterVars         },
	{ "--rodata-name",    	1, 	OptRodataName		},
	{ "--signed-chars",   	0, 	OptSignedChars	       	},
	{ "--start-addr",     	1,	OptStartAddr		},
       	{ "--static-locals",   	0, 	OptStaticLocals	       	},
	{ "--target",	      	1,	OptTarget		},
	{ "--verbose",	      	0,	OptVerbose		},
	{ "--version",	      	0,	OptVersion		},
       	{ "--zeropage-label",   1,     	OptZeropageLabel        },
	{ "--zeropage-name",    1, 	OptZeropageName         },
    };

    unsigned I;

    /* Initialize the cmdline module */
    InitCmdLine (&argc, &argv, "cl65");

    /* Initialize the command descriptors */
    CmdInit (&CC65, "cc65");
    CmdInit (&CA65, "ca65");
    CmdInit (&CO65, "co65");
    CmdInit (&LD65, "ld65");
    CmdInit (&GRC,  "grc");

    /* Our default target is the C64 instead of "none" */
    Target = TGT_C64;

    /* Check the parameters */
    I = 1;
    while (I < ArgCount) {

	/* Get the argument */
	const char* Arg = ArgVec[I];

	/* Check for an option */
	if (Arg [0] == '-') {

	    switch (Arg [1]) {

		case '-':
		    LongOption (&I, OptTab, sizeof(OptTab)/sizeof(OptTab[0]));
		    break;

		case 'A':
		    /* Strict ANSI mode (compiler) */
		    OptAnsi (Arg, 0);
		    break;

		case 'C':
	   	    if (Arg[2] == 'l' && Arg[3] == '\0') {
			/* Make local variables static */
			OptStaticLocals (Arg, 0);
		    } else {
		     	/* Specify linker config file */
		     	OptConfig (Arg, GetArg (&I, 2));
		    }
	     	    break;

		case 'D':
		    /* Define a preprocessor symbol (compiler) */
		    CmdAddArg2 (&CC65, "-D", GetArg (&I, 2));
		    break;

		case 'I':
		    /* Include directory (compiler) */
		    OptIncludeDir (Arg, GetArg (&I, 2));
		    break;

	   	case 'L':
		    if (Arg[2] == 'n' && Arg[3] == '\0') {
		      	/* VICE label file (linker) */
		      	CmdAddArg2 (&LD65, "-Ln", GetArg (&I, 3));
		    } else {
                        /* Library search path (linker) */
                        OptLibPath (Arg, GetArg (&I, 2));
		    }
	    	    break;

	    	case 'O':
	    	    /* Optimize code (compiler, also covers -Oi and others) */
	    	    CmdAddArg (&CC65, Arg);
	    	    break;

	    	case 'S':
	    	    /* Dont assemble and link the created files */
		    DontLink = DontAssemble = 1;
		    break;

	   	case 'T':
		    /* Include source as comment (compiler) */
		    OptAddSource (Arg, 0);
		    break;

		case 'V':
		    /* Print version number */
	       	    OptVersion (Arg, 0);
	   	    break;

	   	case 'W':
	   	    /* Suppress warnings - compiler and assembler */
	   	    CmdAddArg (&CC65, "-W");
	     	    CmdAddArg2 (&CA65, "-W", "0");
	   	    break;

	   	case 'c':
	   	    /* Don't link the resulting files */
	   	    DontLink = 1;
	   	    break;

		case 'd':
		    /* Debug mode (compiler) */
		    OptDebug (Arg, 0);
		    break;

		case 'g':
		    /* Debugging - add to compiler and assembler */
		    OptDebugInfo (Arg, 0);
		    break;

		case 'h':
		case '?':
		    /* Print help - cl65 */
	     	    OptHelp (Arg, 0);
		    break;

	   	case 'j':
		    /* Default characters are signed */
		    OptSignedChars (Arg, 0);
		    break;

	   	case 'l':
		    /* Create an assembler listing */
		    OptListing (Arg, 0);
		    break;

	   	case 'm':
		    /* Create a map file (linker) */
		    OptMapFile (Arg, GetArg (&I, 2));
		    break;

		case 'o':
		    /* Name the output file */
		    OutputName = GetArg (&I, 2);
		    break;

		case 'r':
		    /* Enable register variables */
		    OptRegisterVars (Arg, 0);
		    break;

		case 't':
		    /* Set target system - compiler, assembler and linker */
		    OptTarget (Arg, GetArg (&I, 2));
		    break;

		case 'v':
		    if (Arg [2] == 'm') {
	     	     	/* Verbose map file (linker) */
		     	CmdAddArg (&LD65, "-vm");
	   	    } else {
	   	     	/* Verbose mode (compiler, assembler, linker) */
			OptVerbose (Arg, 0);
	   	    }
	   	    break;

	     	default:
	     	    UnknownOption (Arg);
	    }
	} else {

	    /* Remember the first file name */
	    if (FirstInput == 0) {
	     	FirstInput = Arg;
	    }

	    /* Determine the file type by the extension */
	    switch (GetFileType (Arg)) {

	     	case FILETYPE_C:
	     	    /* Compile the file */
	     	    Compile (Arg);
	     	    break;

	     	case FILETYPE_ASM:
	     	    /* Assemble the file */
	     	    if (!DontAssemble) {
	     		Assemble (Arg);
	     	    }
	     	    break;

	     	case FILETYPE_OBJ:
	     	case FILETYPE_LIB:
	     	    /* Add to the linker files */
	     	    CmdAddFile (&LD65, Arg);
	     	    break;

		case FILETYPE_GR:
		    /* Add to the resource compiler files */
		    CompileRes (Arg);
		    break;

                case FILETYPE_O65:
                    /* Add the the object file converter files */
                    ConvertO65 (Arg);
                    break;

	     	default:
	     	    Error ("Don't know what to do with `%s'", Arg);

	    }

	}

	/* Next argument */
	++I;
    }

    /* Check if we had any input files */
    if (FirstInput == 0) {
	Warning ("No input files");
    }

    /* Link the given files if requested and if we have any */
    if (DontLink == 0 && LD65.FileCount > 0) {
	Link ();
    }

    /* Return an apropriate exit code */
    return EXIT_SUCCESS;
}



