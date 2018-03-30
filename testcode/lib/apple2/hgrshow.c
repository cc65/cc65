// cl65 -t apple2 --start-addr 0x4000 hgrshow.c

#include <tgi.h>
#include <conio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

void main (void)
{
    DIR *dir;
    struct dirent *ent;

    tgi_install (a2_hi_tgi);
    tgi_init ();

    dir = opendir (".");
    while (ent = readdir (dir)) {
        char *ext;
        int hgr;

        ext = strrchr (ent->d_name, '.');
        if (!ext || strcasecmp (ext, ".hgr"))
            continue;

        hgr = open (ent->d_name, O_RDONLY);
        read (hgr, (void*)0x2000, 0x2000);
        close (hgr);

        if (cgetc () == '\r')
            break;
    }
    closedir (dir);

    tgi_uninstall ();
}
