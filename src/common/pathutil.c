
#include <stdlib.h>


#if defined(_WIN32)
#   include <windows.h>
#endif

#if defined(_WIN32)

char *FindAbsolutePath (const char *path) {
    return  _fullpath (NULL, path, MAX_PATH);
}

#else

extern char* realpath (const char* path, char* resolved_path);

/* this uses the POSIX1.-2008 version of the function,
   which solves the problem of finding a maximum path length for the file */
char* FindAbsolutePath (const char* path) {
    return realpath (path, NULL);
}

#endif
