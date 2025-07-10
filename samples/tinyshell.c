/*
** Simple ("tiny") shell to test filename and directory functions.
** Copyright (c) 2013,2016 Christian Groessler, chris@groessler.org
*/

#define VERSION_ASC "0.91"

#ifdef __ATARI__
#define UPPERCASE      /* define (e.g. for Atari) to convert filenames etc. to upper case */
#define HAVE_SUBDIRS
#endif

#ifdef __APPLE2__
#define HAVE_SUBDIRS
#endif

#ifdef __CC65__
#define CHECK_SP
#endif

#define KEYB_BUFSZ 127
#define PROMPT ">>> "

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#ifndef __CC65__
#include <sys/stat.h>
#include <sys/param.h>
#define HAVE_SUBDIRS
#else
#define MAXPATHLEN 64
#endif
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>

#ifdef CHECK_SP
extern unsigned int getsp(void);  /* comes from getsp.s */
#endif

#define CMD_NOTHING 0
#define CMD_INVALID 1
#define CMD_HELP    2
#define CMD_QUIT    3
#define CMD_LS      4
#define CMD_MKDIR   5
#define CMD_RMDIR   6
#define CMD_CHDIR   7
#define CMD_RM      8
#define CMD_RENAME  9
#define CMD_COPY    10
#define CMD_PWD     11
#define CMD_CLS     12
#define CMD_VERBOSE 13
#define CMD_EXEC    14

static unsigned char verbose;
static unsigned char terminate;
static unsigned char cmd;
static char *cmd_asc, *arg1, *arg2, *arg3, *args;  /* 'args': everything after command */
static char keyb_buf[KEYB_BUFSZ + 1];
static char keyb_buf2[KEYB_BUFSZ + 1];
static size_t cpbuf_sz = 4096;

struct cmd_table {
    char *name;
    unsigned char code;
} cmd_table[] = {
    { "help",  CMD_HELP },
    { "quit",  CMD_QUIT },
    { "q",     CMD_QUIT },
    { "exit",  CMD_QUIT },
    { "ls",    CMD_LS },
    { "dir",   CMD_LS },
    { "md",    CMD_MKDIR },
#ifdef HAVE_SUBDIRS
    { "mkdir", CMD_MKDIR },
    { "rd",    CMD_RMDIR },
    { "rmdir", CMD_RMDIR },
    { "cd",    CMD_CHDIR },
    { "chdir", CMD_CHDIR },
#endif
    { "rm",    CMD_RM },
    { "del",   CMD_RM },
    { "cp",    CMD_COPY },
    { "copy",  CMD_COPY },
    { "mv",    CMD_RENAME },
    { "ren",   CMD_RENAME },
    { "pwd",   CMD_PWD },
    { "exec",  CMD_EXEC },
#ifdef __ATARI__
    { "cls",   CMD_CLS },
#endif
    { "verbose", CMD_VERBOSE },
    { NULL, 0 }
};

static void banner(void)
{
    puts("\"tiny\" command line shell, v" VERSION_ASC);
    puts("written by chris@groessler.org");
    puts("type 'help' for help\n");
}

static void get_command(void)
{
    unsigned char i = 0;

#ifdef CHECK_SP
    static char firstcall = 1;
    static unsigned int good_sp;
    unsigned int c_sp;
    if (firstcall)
        c_sp = good_sp = getsp();
    else
        c_sp = getsp();

    if (c_sp != good_sp) {
        printf("SP: 0x%04X  ***MISMATCH*** 0x%04X\n", c_sp, good_sp);
    }
    else if (verbose)
        printf("SP: 0x%04X\n", c_sp);
#endif

    arg1 = arg2 = arg3 = NULL;

    /* issue prompt */
    printf(PROMPT);

    /* get input from the user */
    if (! fgets(keyb_buf, KEYB_BUFSZ, stdin)) {
        puts("");
        cmd = CMD_QUIT;
        return;
    }

    /* put everything after first string into 'args' */

    strcpy(keyb_buf2, keyb_buf);  /* use a backup copy for 'args' */

    /* skip over the first non-whitespace item */
    cmd_asc = strtok(keyb_buf2, " \t\n");
    if (cmd_asc)
        args = strtok(NULL, "");  /* get everything */
    else
        *args = 0;  /* no arguments */

    /* split input into cmd, arg1, arg2, arg3 */

    /* get and parse command */
    cmd_asc = strtok(keyb_buf, " \t\n");
    if (! cmd_asc) {
        cmd = CMD_NOTHING;
        return;
    }
    cmd = CMD_INVALID;
    while (cmd_table[i].name) {
        if (! strcmp(cmd_table[i].name, cmd_asc)) {
            cmd = cmd_table[i].code;
            break;
        }
        i++;
    }

    /* get arguments */
    arg1 = strtok(NULL, " \t\n");
    if (! arg1)
        return;
    arg2 = strtok(NULL, " \t\n");
    if (! arg2)
        return;
    arg3 = strtok(NULL, " \t\n");
}

static void cmd_help(void)
{
    puts("quit, exit -  exit shell");
    puts("ls, dir    -  display current directory");
    puts("              and drive contents");
    puts("rm, del    -  delete file");
    puts("cp, copy   -  copy file");
    puts("mv, ren    -  rename file");
    puts("cd, chdir  -  change directory or drive");
    puts("md, mkdir  -  make directory or drive");
    puts("rd, rmdir  -  remove directory or drive");
    puts("exec       -  run program");
#ifdef __ATARI__
    puts("cls        -  clear screen");
#endif
    puts("verbose    -  set verbosity level");
}

static void cmd_ls(void)
{
    DIR *dir;
    char *arg;
    struct dirent *dirent;
#ifdef __ATARI__
    char need_free = 0;
#endif

    if (arg2) {
        puts("usage: ls [dir]");
        return;
    }

    /* print directory listing */
    if (arg1) {
#ifdef UPPERCASE
        strupr(arg1);
#endif
#ifdef __ATARI__
        /* not sure if this shouldn't be done by the runtime lib */
        if (*(arg1 + strlen(arg1) - 1) == ':' || *(arg1 + strlen(arg1) - 1) == '>') {
            arg = malloc(strlen(arg1) + 4);
            if (! arg) {
                printf("malloc failed: %s", strerror(errno));
                return;
            }
            need_free = 1;
            memcpy(arg, arg1, strlen(arg1) + 1);
            strcat(arg, "*.*");
        }
        else
#endif
            arg = arg1;
    }
    else
        arg = ".";

    if (verbose)
        printf("Buffer addr: %p\n", arg);
    dir = opendir(arg);
#ifdef __ATARI__
    if (need_free) free(arg);
#endif
    if (! dir) {
        puts("opendir failed");
        return;
    }

    while (dirent = readdir(dir))
        puts(dirent->d_name);

    closedir(dir);
}

static void cmd_rm(void)
{
    if (!arg1 || arg2) {
        puts("usage: rm <file>");
        return;
    }

#ifdef UPPERCASE
    strupr(arg1);
#endif

    if (unlink(arg1))
        printf("remove failed: %s\n", strerror(errno));
}

#ifdef HAVE_SUBDIRS

static void cmd_mkdir(void)
{
    if (!arg1 || arg2) {
        puts("usage: mkdir <dir>");
        return;
    }

#ifdef UPPERCASE
    strupr(arg1);
#endif

    if (mkdir(arg1, 0777))
        printf("mkdir failed: %s\n", strerror(errno));
}

static void cmd_rmdir(void)
{
    if (!arg1 || arg2) {
        puts("usage: rmdir <dir>");
        return;
    }

#ifdef UPPERCASE
    strupr(arg1);
#endif

    if (rmdir(arg1))
        printf("rmdir failed: %s\n", strerror(errno));
}

static void cmd_chdir(void)
{
    if (!arg1 || arg2) {
        puts("usage: cddir <dir>");
        return;
    }

#ifdef UPPERCASE
    strupr(arg1);
#endif

    if (chdir(arg1))
        printf("chdir failed: %s\n", strerror(errno));
}

static void cmd_pwd(void)
{
    char *buf;

    if (arg1) {
        puts("usage: pwd");
        return;
    }

    buf = malloc(MAXPATHLEN);
    if (! buf) {
        printf("malloc %u bytes failed: %s\n", MAXPATHLEN, strerror(errno));
        return;
    }
    if (verbose)
        printf("Buffer addr: %p\n", buf);
    if (!getcwd(buf, MAXPATHLEN)) {
        printf("getcwd failed: %s\n", strerror(errno));
        free(buf);
        return;
    }

    puts(buf);
    free(buf);
}

#endif /* #ifdef HAVE_SUBDIRS */

static void cmd_rename(void)
{
    if (!arg2 || arg3) {
        puts("usage: mv <oldname> <newname>");
        return;
    }

#ifdef UPPERCASE
    strupr(arg1);
    strupr(arg2);
#endif

    if (rename(arg1, arg2))
        printf("rename failed: %s\n", strerror(errno));
}

static void cmd_exec(void)
{
    char *progname, *arguments;

    progname = strtok(args, " \t\n");
    if (! progname) {
        puts("usage: exec <progname> [arguments]");
        return;
    }
    arguments = strtok(NULL, "");

    /*printf("exec: %s %s\n", progname, arguments ? arguments : "");*/
    (void)exec(progname, arguments);
    printf("exec error: %s\n", strerror(errno));
}

static void cmd_copy(void)
{
    int srcfd = -1, dstfd = -1;
    char *buf;
    int readsz, writesz;

    if (!arg2 || arg3) {
        puts("usage: cp <src> <dest>");
        return;
    }

#ifdef UPPERCASE
    strupr(arg1);
    strupr(arg2);
#endif

    buf = malloc(cpbuf_sz);
    if (! buf) {
        printf("malloc %u bytes failed: %s\n", cpbuf_sz, strerror(errno));
        return;
    }
    if (verbose)
        printf("Buffer addr: %p\n", buf);

    while (1) {
        if (srcfd == -1) {
            srcfd = open(arg1, O_RDONLY);
            if (srcfd < 0) {
                printf("open(%s) failed: %s\n", arg1, strerror(errno));
                break;
            }
        }

        readsz = read(srcfd, buf, cpbuf_sz);
        if (readsz < 0) {
            printf("read error: %s\n", strerror(errno));
            break;
        }
        if (! readsz)
            break;

        if (dstfd == -1) {
            dstfd = open(arg2, O_WRONLY | O_CREAT | O_TRUNC, 0777);
            if (dstfd < 0) {
                printf("open(%s) failed: %s\n", arg2, strerror(errno));
                break;
            }
        }

        writesz = write(dstfd, buf, readsz);
        if (writesz < 0 || writesz != readsz) {
            printf("write error: %s\n", strerror(errno));
            break;
        }
        if (readsz != cpbuf_sz)
            break;
    }

    free(buf);
    if (srcfd >= 0) close(srcfd);
    if (dstfd >= 0) close(dstfd);
}

#ifdef __ATARI__
static void cmd_cls(void)
{
    printf("\f");
}
#endif

static void cmd_verbose(void)
{
    unsigned long verb;
    char *endptr;

    if (!arg1 || arg2) {
        puts("usage: verbose <level>");
        return;
    }

    verb = strtoul(arg1, &endptr, 10);
    if (verb > 255 || *endptr) {
        puts("invalid verbosity level");
        return;
    }

    verbose = verb;
    printf("verbosity level set to %d\n", verbose);
}

static void run_command(void)
{
    switch (cmd) {
        default: puts("internal error"); return;
        case CMD_NOTHING: return;
        case CMD_INVALID: puts("invalid command"); return;
        case CMD_HELP: cmd_help(); return;
        case CMD_QUIT: terminate = 1; return;
        case CMD_LS: cmd_ls(); return;
        case CMD_RM: cmd_rm(); return;
#ifdef HAVE_SUBDIRS
        case CMD_CHDIR: cmd_chdir(); return;
        case CMD_MKDIR: cmd_mkdir(); return;
        case CMD_RMDIR: cmd_rmdir(); return;
        case CMD_PWD: cmd_pwd(); return;
#endif
        case CMD_EXEC: cmd_exec(); return;
        case CMD_RENAME: cmd_rename(); return;
        case CMD_COPY: cmd_copy(); return;
#ifdef __ATARI__
        case CMD_CLS: cmd_cls(); return;
#endif
        case CMD_VERBOSE: cmd_verbose(); return;
    }
}

int main(void)
{
    banner();

    while (! terminate) {
        get_command();
        run_command();
    }
    return 0;
}

/* Local Variables: */
/* c-file-style: "cpg" */
/* c-basic-offset: 4 */
/* End: */
