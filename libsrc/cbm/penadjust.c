/*
** Main lightpen driver calibration functions.
**
** 2013-07-25, Greg King
*/


#include <stddef.h>
#include <fcntl.h>
#include <unistd.h>
#include <pen.h>


static const char *name;


/* Get a lightpen calibration value from a file if it exists.  Otherwise, call
** pen_calibrate() to create a value; then, write it into a file, so that it
** will be available at the next time that the lightpen is used.
** Might change the screen.
*/
static void __fastcall__ adjuster (unsigned char *XOffset)
{
    int fd = open (name, O_RDONLY);

    if (fd < 0) {
        pen_calibrate (XOffset);
        fd = open (name, O_WRONLY | O_CREAT | O_EXCL);
        if (fd >= 0) {
            (void) write (fd, XOffset, 1);
            close (fd);
        }
    } else {
        (void) read (fd, XOffset, 1);
        close (fd);
    }
}


/* pen_adjust() is optional; if you want to use its feature,
** then it must be called before a driver is installed.
** Note:  This function merely saves the file-name pointer, and sets
** the pen_adjuster pointer.  The file will be read only when a driver
** is installed, and only if that driver wants to be calibrated.
*/
void __fastcall__ pen_adjust (const char *filename)
{
    if (filename != NULL && filename[0] != '\0') {
        name = filename;
        pen_adjuster = adjuster;
    } else {
        pen_adjuster = pen_calibrate;
    }
}
