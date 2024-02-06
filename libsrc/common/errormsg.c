/*
** errormsg.c
**
** Ullrich von Bassewitz, 17.05.2000
*/



const char* const _sys_errlist[] = {
    "Unknown error",                /* 0 */
    "No such file or directory",    /* ENOENT */
    "Out of memory",                /* ENOMEM */
    "Permission denied",            /* EACCES */
    "No such device",               /* ENODEV */
    "Too many open files",          /* EMFILE */
    "Device or resource busy",      /* EBUSY */
    "Invalid argument",             /* EINVAL */
    "No space left on device",      /* ENOSPC */
    "File exists",                  /* EEXIST */
    "Try again",                    /* EAGAIN */
    "I/O error",                    /* EIO */
    "Interrupted system call",      /* EINTR */
    "Function not implemented",     /* ENOSYS */
    "Illegal seek",                 /* ESPIPE */
    "Range error",                  /* ERANGE */
    "Bad file number",              /* EBADF */
    "Exec format error",            /* ENOEXEC */
    "Unknown OS error code",        /* EUNKNOWN */
};


