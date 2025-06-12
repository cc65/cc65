/*****************************************************************************/
/*                                                                           */
/*                                strftime.c                                 */
/*                                                                           */
/*      Convert broken down time to a string in a user specified format      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
/*                                                                           */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/



#include <string.h>
#include <stdio.h>
#include <time.h>



/* Use static local variables for speed */
#pragma static-locals (on)



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



size_t __fastcall__ strftime (char* buf, size_t bufsize, const char* format,
                              const struct tm* tm)
{
    static const char* const days[7] = {
        "Sunday",   "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday"
    };
    static const char* const months[12] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    unsigned    count;
    unsigned    len;
    char        c;
    char        arg[40];
    const char* argptr;

    /* Copy until we reach the end of the format string or a format specifier */
    count = 0;
    while (1) {
        if (count >= bufsize) {
            /* Not enough buffer space available */
            return 0;
        }
        if ((c = *format++) == '\0') {
            /* End of format string reached */
            *buf = '\0';
            return count;
        }
        if (c == '%') {
            /* Format specifier */
            argptr = arg;
            switch (*format++) {

                case '%':
                    arg[0] = '%';
                    arg[1] = '\0';
                    break;

                case 'A':
                    argptr = days[tm->tm_wday];
                    break;

                case 'B':
                    argptr = months[tm->tm_mon];
                    break;

                case 'D':
                    sprintf (arg, "%02d/%02d/%02d", tm->tm_mon + 1,
                             tm->tm_mday, tm->tm_year % 100);
                    break;

                case 'F':
                    /* C99 */
                    sprintf (arg, "%04d-%02d-%02d", tm->tm_year + 1900,
                             tm->tm_mon + 1, tm->tm_mday);
                    break;

                case 'H':
                    sprintf (arg, "%02d", tm->tm_hour);
                    break;

                case 'I':
                    sprintf (arg, "%02d", (tm->tm_hour + 11) % 12 + 1);
                    break;

                case 'M':
                    sprintf (arg, "%02d", tm->tm_min);
                    break;

                case 'P':
                    /* GNU extension */
                    argptr = (tm->tm_hour >= 12)? "pm" : "am";
                    break;

                case 'S':
                    sprintf (arg, "%02d", tm->tm_sec);
                    break;

                case 'U':
                    sprintf (arg, "%02d", (tm->tm_yday + 7 - tm->tm_wday) / 7);
                    break;

                case 'W':
                    sprintf (arg, "%02d",
                             (tm->tm_yday + 7 - (tm->tm_wday? tm->tm_wday - 1 : 6)) / 7);
                    break;

                case 'X':
                    sprintf (arg, "%02d:%02d:%02d", tm->tm_hour,
                             tm->tm_min, tm->tm_sec);
                    break;

                case 'Y':
                    sprintf (arg, "%4d", tm->tm_year + 1900);
                    break;

                case 'Z':
                    argptr = tm->tm_isdst? _tz.dstname : _tz.tzname;
                    break;

                case 'a':
                    sprintf (arg, "%.3s", days[tm->tm_wday]);
                    break;

                case 'b':
                    sprintf (arg, "%.3s", months[tm->tm_mon]);
                    break;

                case 'c':
                    sprintf (arg, "%.3s %.3s%3d %02d:%02d:%02d %d",
                             days[tm->tm_wday], months[tm->tm_mon],
                             tm->tm_mday, tm->tm_hour, tm->tm_min,
                             tm->tm_sec, tm->tm_year + 1900);
                    break;

                case 'd':
                    sprintf (arg, "%02d", tm->tm_mday);
                    break;

                case 'j':
                    sprintf (arg, "%03d", tm->tm_yday + 1);
                    break;

                case 'm':
                    sprintf (arg, "%02d", tm->tm_mon + 1);
                    break;

                case 'p':
                    argptr = (tm->tm_hour >= 12)? "PM" : "AM";
                    break;

                case 'w':
                    sprintf (arg, "%d", tm->tm_wday);
                    break;

                case 'x':
                    sprintf (arg, "%04d-%02d-%02d", tm->tm_year + 1900,
                             tm->tm_mon + 1, tm->tm_mday);
                    break;

                case 'y':
                    sprintf (arg, "%02d", tm->tm_year % 100);
                    break;

                default:
                    /* Unknown format specifier, convert to empty string */
                    arg[0] = '\0';
                    break;
            }

            /* Check if we have enough space to copy the argument string */
            len = strlen (argptr);
            count += len;
            if (count < bufsize) {
                memcpy (buf, argptr, len);
                buf += len;
            }

        } else {

            /* No format character, just copy */
            *buf++ = c;
            ++count;

        }
    }
}



