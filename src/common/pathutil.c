
#include <stdlib.h>

#if defined(_WIN32)
#   include <windows.h>
#   include <fileapi.h>
#   include "xmalloc.h"
#endif

#if defined(_WIN32)

char *FindRealPath (const char *Path)
/*
** Determines the real path the given relative path of an existing file.
** If the path points to a symlink, resolves such symlink.
** The real path for the file is stored in a malloced buffer.
** Returns NULL if the file doesn't exist.
** The returned path's separator is system specific.
*/
{

    HANDLE Handle = CreateFileA (Path,
                                 FILE_READ_ATTRIBUTES,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_FLAG_BACKUP_SEMANTICS,
                                 NULL);

    if (Handle == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    size_t BufferSize = MAX_PATH + 10;
    char* Buffer = xmalloc(BufferSize);

    DWORD Status = GetFinalPathNameByHandleA (Handle,
                                              Buffer,
                                              BufferSize,
                                              FILE_NAME_NORMALIZED | VOLUME_NAME_DOS);

    if (Status == 0) {
        free(Buffer);
        CloseHandle(Handle);
        return NULL;
    }

    CloseHandle(Handle);

    return Buffer;
}

#else

extern char* realpath (const char* path, char* resolved_path);

char* FindAbsolutePath (const char* path)
/*
** Determines the absolute path of the given relative path.
** If the path points to a symlink, resolves such symlink.
** The absolute path for the file is stored in a malloced buffer.
** Returns NULL if some error occured.
** The returned path's separator is system specific.
*/
{
    return realpath (path, NULL);
}

#endif
