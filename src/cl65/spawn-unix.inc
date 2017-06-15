/*****************************************************************************/
/*                                                                           */
/*                              spawn-unix.inc                               */
/*                                                                           */
/*                Execute other external programs (Unix version)             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1999-2002 Ullrich von Bassewitz                                       */
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



#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Mode argument for spawn. This value is ignored by the function and only
** provided for DOS/Windows compatibility.
*/
#ifndef P_WAIT
#define P_WAIT  0
#endif



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int spawnvp (int Mode attribute ((unused)), const char* File, char* const argv [])
/* Execute the given program searching and wait until it terminates. The Mode
** argument is ignored (compatibility only). The result of the function is
** the return code of the program. The function will terminate the program
** on errors.
*/
{
    int Status = 0;

    /* Fork */
    int pid = fork ();
    if (pid < 0) {

        /* Error forking */
        Error ("Cannot fork: %s", strerror (errno));

    } else if (pid == 0) {

        /* The son - exec the program */
        if (execvp (File, argv) < 0) {
            Error ("Cannot exec `%s': %s", File, strerror (errno));
        }

    } else {

        /* The father: Wait for the subprocess to terminate */
        if (waitpid (pid, &Status, 0) < 0) {
            Error ("Failure waiting for subprocess: %s", strerror (errno));
        }

        /* Examine the child status */
        if (!WIFEXITED (Status)) {
            Error ("Subprocess `%s' aborted by signal %d", File, WTERMSIG (Status));
        }
    }

    /* Only the father goes here, we place a return here regardless of that
    ** to avoid compiler warnings.
    */
    return WEXITSTATUS (Status);
}
