// cl65 -t apple2enh --start-addr 0x4000 dhgrshow.c

#include <tgi.h>
#include <conio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <peekpoke.h>

void main (void)
{
    unsigned old;
    DIR *dir;
    struct dirent *ent;

    old = videomode (VIDEOMODE_80x24);
    tgi_install (a2e_hi_tgi);
    tgi_init ();
    POKE (0xC05E, 0);

    dir = opendir (".");
    while (ent = readdir (dir)) {
        char *ext;
        int hgr;

        ext = strrchr (ent->d_name, '.');
        if (!ext || strcasecmp (ext, ".dhgr"))
            continue;

        hgr = open (ent->d_name, O_RDONLY);
        POKE (0xC055, 0);
        read (hgr, (void*)0x2000, 0x2000);
        POKE (0xC054, 0);
        read (hgr, (void*)0x2000, 0x2000);
        close (hgr);

        if (cgetc () == '\r')
            break;
    }
    closedir (dir);

    POKE (0xC05F, 0);
    tgi_uninstall ();
    videomode (old);
}
