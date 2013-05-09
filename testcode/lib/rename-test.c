/* rename-test.c
**
**
** A simple test of the rename function.
**
** 2008-10-06, Greg King
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>


static FILE* file;
static int r;
static char name1[16], name2[16];

int main(void)
{
    /* Generate two temporary file-names that have a random, unused spelling. */
    _randomize();
    for (;;) {
        r = rand();
        sprintf(name1, "r%04.4u.1", (unsigned)r);
        sprintf(name2, "r%04.4u.2", (unsigned)r);

        /* Ensure that neither file-name exists. */
        errno = 0;
        file = fopen(name1, "r");
        if (file != NULL) {
            fclose(file);
            continue;                   /* try a different spelling */
        }

        /* Make sure that fopen() failed for the right reason. */
        if (errno != ENOENT) {
            perror("Disk error with first name");
            return EXIT_FAILURE;
        }

        errno = 0;
        file = fopen(name2, "r");
        if (file != NULL) {
            fclose(file);
            continue;
        }
        if (errno != ENOENT) {
            perror("Disk error with second name");
            return EXIT_FAILURE;
        }

        break;                          /* neither one exists; do next step */
    }

    /* Create the first file.
    ** Close it without writing anything because only its name is important.
    */
    printf("Creating file: %s\n", name1);
    file = fopen(name1, "w");
    if (file == NULL) {
        _poserror("Disk error making first file");
        return EXIT_FAILURE;
    }
    fclose(file);

    /* Verify that the file-name exists now. */
    file = fopen(name1, "r");
    if (file == NULL) {
        _poserror("Cannot find first name");
        return EXIT_FAILURE;
    }
    fclose(file);

    /* Whew!  Finally, we get to the reason why this program exists:
    ** Confirm that the first file-name can be changed into the second
    ** file-name.
    */
    printf("Renaming %s to %s\n", name1, name2);
    r = rename(name1, name2);
    if (r < 0) {
        _poserror("rename() failed");
        return EXIT_FAILURE;
    }

    /* Verify that the first file-name no longer exists. */
    file = fopen(name1, "r");
    if (file != NULL) {
        fclose(file);
        _poserror("First name still exists");
        return EXIT_FAILURE;
    }

    /* Verify that the second file-name exists now. */
    file = fopen(name2, "r");
    if (file == NULL) {
        _poserror("Cannot find second name");
        return EXIT_FAILURE;
    }
    fclose(file);

    printf("Success!\n");

    /* Delete the second (temporary) name. */
    printf("Removing %s\n", name2);
    r = remove(name2);
    if (r < 0) {
        _poserror("remove() failed");
        return EXIT_FAILURE;
    }

    printf("rename() passed the test.\n");
    return EXIT_SUCCESS;
}


