/*
 * Simple ("tiny") shell to test filename and directory functions.
 * Copyright (c) 2013, Christian Groessler, chris@groessler.org
 */

#define VERSION_ASC "0.90"

#define KEYB_BUFSZ 80
#define PROMPT ">>> "
#ifdef __ATARI__
#define UPPERCASE      /* define (e.g. for Atari) to convert filenames etc. to upper case */
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#ifndef __CC65__
#include <sys/stat.h>
#include <sys/param.h>
#else
#define MAXPATHLEN 64
#endif
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>

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

static unsigned char terminate;
static unsigned char cmd;
static unsigned char *cmd_asc, *arg1, *arg2, *arg3;
static unsigned char keyb_buf[KEYB_BUFSZ];
static size_t cpbuf_sz = 4096;

struct cmd_table {
    unsigned char *name;
    unsigned char code;
} cmd_table[] = {
    { "help",  CMD_HELP },
    { "quit",  CMD_QUIT },
    { "q",     CMD_QUIT },
    { "exit",  CMD_QUIT },
    { "ls",    CMD_LS },
    { "dir",   CMD_LS },
    { "md",    CMD_MKDIR },
    { "mkdir", CMD_MKDIR },
    { "rd",    CMD_RMDIR },
    { "rmdir", CMD_RMDIR },
    { "cd",    CMD_CHDIR },
    { "chdir", CMD_CHDIR },
    { "rm",    CMD_RM },
    { "del",   CMD_RM },
    { "cp",    CMD_COPY },
    { "copy",  CMD_COPY },
    { "mv",    CMD_RENAME },
    { "ren",   CMD_RENAME },
    { "pwd",   CMD_PWD },
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

    arg1 = arg2 = arg3 = NULL;

    /* issue prompt */
    printf(PROMPT);

    /* get input from the user */
    if (! fgets(keyb_buf, KEYB_BUFSZ, stdin)) {
        puts("");
        cmd = CMD_QUIT;
        return;
    }

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
    puts("sorry, you cannot start programs here");
}

static void cmd_ls(void)
{
    DIR *dir;
    unsigned char *arg;
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
    if (!getcwd(buf, MAXPATHLEN)) {
        printf("getcwd failed: %s\n", strerror(errno));
        free(buf);
        return;
    }

    puts(buf);
    free(buf);
}

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

static void cmd_copy(void)
{
    int srcfd = -1, dstfd = -1;
    unsigned char *buf;
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
        case CMD_CHDIR: cmd_chdir(); return;
        case CMD_MKDIR: cmd_mkdir(); return;
        case CMD_RMDIR: cmd_rmdir(); return;
        case CMD_PWD: cmd_pwd(); return;
        case CMD_RENAME: cmd_rename(); return;
        case CMD_COPY: cmd_copy(); return;
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
