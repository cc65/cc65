#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>



FILE* Fopen (const char* Name, const char* Mode)
{
    FILE* F;
    printf ("Opening %s(%s): ", Name, Mode);
    F = fopen (Name, Mode);
    if (F) {
        printf ("Ok (%d)\n", fileno (F));
    } else {
        printf (strerror (errno));
    }
    return F;
}



void Fwrite (FILE* F, const void* Buf, unsigned Size)
{
    size_t Res;
    Res = fwrite (Buf, 1, Size, F);
    printf ("Writing %u bytes to %d: %u\n", Size, fileno (F), Res);
}



int Fread (FILE* F, void* Buf, unsigned Size)
{
    size_t Res;
    Res = fread (Buf, 1, Size, F);
    printf ("Reading %u bytes from %d: %u\n", Size, fileno (F), Res);
    return Res > 0? Res : 0;
}



void Fclose (FILE* F)
{
    printf ("Closing %d:", fileno (F));
    if (fclose (F) == 0) {
        printf ("Ok\n");
    } else {
        printf (strerror (errno));
    }
}



int main (void)
{
    FILE* F1;
    FILE* F2;
    int Res;
    static const char text1[] = "This goes into file #1\n";
    static const char text2[] = "This goes into file #2\n";
    static const char text3[] = "This goes into file #3\n";
    static const char text4[] = "This goes into file #4\n";
    static char Buf[200];


    F1 = Fopen ("foobar1", "w");
    F2 = Fopen ("foobar2", "w");

    Fwrite (F1, text1, sizeof (text1) - 1);
    Fwrite (F2, text2, sizeof (text2) - 1);
    Fwrite (F1, text1, sizeof (text1) - 1);
    Fwrite (F2, text2, sizeof (text2) - 1);

    Fclose (F1);
    Fclose (F2);

    F1 = Fopen ("foobar3", "w");
    F2 = Fopen ("foobar4", "w");

    Fwrite (F1, text3, sizeof (text3) - 1);
    Fwrite (F2, text4, sizeof (text4) - 1);
    Fwrite (F1, text3, sizeof (text3) - 1);
    Fwrite (F2, text4, sizeof (text4) - 1);

    Fclose (F1);
    Fclose (F2);

    F1 = Fopen ("foobar1", "r");
    Res = Fread (F1, Buf, sizeof (Buf));
    printf ("%.*s", Res, Buf);
    Res = Fread (F1, Buf, sizeof (Buf));
    Fclose (F1);

    return 0;
}


