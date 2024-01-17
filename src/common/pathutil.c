
#include <stdlib.h>

#if defined(_WIN32)
#   include <windows.h>
#   include <fileapi.h>
#   include "xmalloc.h"
#endif

#if defined(_WIN32)

char *FindRealPath (const char *Path)
/*
** Returns a malloced buffer containing the canonical path of the given path.
** If the path points to a non-existent file, or if any error occurs, NULL is returned.
** If the path points to a symlink, the resolved symlink path is returned.
** Note: The returned path's separator is system specific.
*/
{

    HANDLE Handle = CreateFileA (Path,
                                 FILE_READ_ATTRIBUTES,
                                 FILE_SHARE_READ |
                                 FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_FLAG_BACKUP_SEMANTICS,
                                 NULL);

    if (Handle == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    size_t BufferSize = MAX_PATH + 10;
    char* Buffer = xmalloc (BufferSize);

    DWORD Status = GetFinalPathNameByHandleA (Handle,
                                              Buffer,
                                              BufferSize,
                                              FILE_NAME_NORMALIZED
                                              | VOLUME_NAME_DOS);

    if (Status == 0) {
        xfree (Buffer);
        CloseHandle (Handle);
        return NULL;
    }

    CloseHandle (Handle);

    return Buffer;
}

#else

extern char* realpath (const char* path, char* resolved_path);

char* FindRealPath (const char* path)
/*
** Returns a malloced buffer containing the canonical path of the given path.
** If the path points to a non-existent file, or if any error occurs, NULL is returned.
** If the path points to a symlink, the resolved symlink path is returned.
** Note: The returned path's separator is system specific.
*/
{
    return realpath (path, NULL);
}

#endif
