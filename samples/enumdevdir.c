/*
** Enumerate devices, directories and files.
**
** 2012-10-15, Oliver Schmidt (ol.sc@web.de)
**
*/



#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <device.h>
#include <dirent.h>


void printdir (char *newdir)
{
    char olddir[FILENAME_MAX];
    char curdir[FILENAME_MAX];
    DIR *dir;
    struct dirent *ent;
    char *subdirs = NULL;
    unsigned dirnum = 0;
    unsigned num;

    getcwd (olddir, sizeof (olddir));
    if (chdir (newdir)) {

        /* If chdir() fails we just print the
        ** directory name - as done for files.
        */
        printf ("  Dir  %s\n", newdir);
        return;
    }

    /* We call getcwd() in order to print the
    ** absolute pathname for a subdirectory.
    */
    getcwd (curdir, sizeof (curdir));
    printf (" Dir %s:\n", curdir);

    /* Calling opendir() always with "." avoids
    ** fiddling around with pathname separators.
    */
    dir = opendir (".");
    while (ent = readdir (dir)) {

        if (_DE_ISREG (ent->d_type)) {
            printf ("  File %s\n", ent->d_name);
            continue;
        }

        /* We defer handling of subdirectories until we're done with the
        ** current one as several targets don't support other disk i/o
        ** while reading a directory (see cc65 readdir() doc for more).
        */
        if (_DE_ISDIR (ent->d_type)) {
            subdirs = realloc (subdirs, FILENAME_MAX * (dirnum + 1));
            strcpy (subdirs + FILENAME_MAX * dirnum++, ent->d_name);
        }
    }
    closedir (dir);

    for (num = 0; num < dirnum; ++num) {
        printdir (subdirs + FILENAME_MAX * num);
    }
    free (subdirs);

    chdir (olddir);
}


void main (void)
{
    unsigned char device;
    char devicedir[FILENAME_MAX];

    /* Calling getfirstdevice()/getnextdevice() does _not_ turn on the motor
    ** of a drive-type device and does _not_ check for a disk in the drive.
    */
    device = getfirstdevice ();
    while (device != INVALID_DEVICE) {
        printf ("Device %d:\n", device);

        /* Calling getdevicedir() _does_ check for a (formatted) disk in a
        ** floppy-disk-type device and returns NULL if that check fails.
        */
        if (getdevicedir (device, devicedir, sizeof (devicedir))) {
            printdir (devicedir);
        } else {
            printf (" N/A\n");
        }

        device = getnextdevice (device);
    }

    cgetc ();
}
