/*
** Enumerate devices, directories and files.
**
** 2012-10-15, Oliver Schmidt (ol.sc@web.de)
**
*/



#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <device.h>
#include <dirent.h>
#include <cc65.h>


/* returns true for error, false for OK */
bool printdir (char *newdir)
{
    char *olddir;
    char *curdir;
    DIR *dir;
    struct dirent *ent;
    char *subdirs = NULL;
    unsigned dirnum = 0;
    unsigned num;

    olddir = malloc (FILENAME_MAX);
    if (olddir == NULL) {
      perror ("cannot allocate memory");
      return true;
    }

    getcwd (olddir, FILENAME_MAX);
    if (chdir (newdir)) {

        /* If chdir() fails we just print the
        ** directory name - as done for files.
        */
        printf ("  Dir  %s\n", newdir);
        free (olddir);
        return false;
    }

    curdir = malloc (FILENAME_MAX);
    if (curdir == NULL) {
      perror ("cannot allocate memory");
      return true;
    }

    /* We call getcwd() in order to print the
    ** absolute pathname for a subdirectory.
    */
    getcwd (curdir, FILENAME_MAX);
    printf (" Dir %s:\n", curdir);
    free (curdir);

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
        if (printdir (subdirs + FILENAME_MAX * num))
            break;
    }
    free (subdirs);

    chdir (olddir);
    free (olddir);
    return false;
}


void main (void)
{
    unsigned char device;
    char *devicedir;

    devicedir = malloc (FILENAME_MAX);
    if (devicedir == NULL) {
      perror ("cannot allocate memory");
      return;
    }

    /* Calling getfirstdevice()/getnextdevice() does _not_ turn on the motor
    ** of a drive-type device and does _not_ check for a disk in the drive.
    */
    device = getfirstdevice ();
    while (device != INVALID_DEVICE) {
        printf ("Device %d:\n", device);

        /* Calling getdevicedir() _does_ check for a (formatted) disk in a
        ** floppy-disk-type device and returns NULL if that check fails.
        */
        if (getdevicedir (device, devicedir, FILENAME_MAX)) {
            printdir (devicedir);
        } else {
            printf (" N/A\n");
        }

        device = getnextdevice (device);
    }

    if (doesclrscrafterexit ()) {
        getchar ();
    }

    free (devicedir);
}
