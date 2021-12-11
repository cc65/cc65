#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>


int Open (const char* Name, int Flags)
{
    int fd;
    printf ("Opening %s: ", Name);
    fd = open (Name, Flags);
    printf ("%d\n", fd);
    return fd;
}



void Write (int fd, const void* Buf, unsigned Size)
{
    int Res;
    Res = write (fd, Buf, Size);
    printf ("Writing %u bytes to %d: %d\n", Size, fd, Res);
}



int Read (int fd, void* Buf, unsigned Size)
{
    int Res;
    Res = read (fd, Buf, Size);
    printf ("Reading %u bytes from %d: %d\n", Size, fd, Res);
    return Res > 0? Res : 0;
}



void Close (int fd)
{
    printf ("Closing %d: %d\n", fd, close (fd));
}



int main (void)
{
    int fd1, fd2;
    int Res;
    static const char text1[] = "This goes into file #1\n";
    static const char text2[] = "This goes into file #2\n";
    static const char text3[] = "This goes into file #3\n";
    static const char text4[] = "This goes into file #4\n";
    static char Buf[200];


    fd1 = Open ("foobar1", O_WRONLY|O_CREAT|O_TRUNC);
    fd2 = Open ("foobar2", O_WRONLY|O_CREAT|O_TRUNC);

    Write (fd1, text1, sizeof (text1) - 1);
    Write (fd2, text2, sizeof (text2) - 1);
    Write (fd1, text1, sizeof (text1) - 1);
    Write (fd2, text2, sizeof (text2) - 1);

    Close (fd1);
    Close (fd2);

    fd1 = Open ("foobar3", O_WRONLY|O_CREAT|O_TRUNC);
    fd2 = Open ("foobar4", O_WRONLY|O_CREAT|O_TRUNC);

    Write (fd1, text3, sizeof (text3) - 1);
    Write (fd2, text4, sizeof (text4) - 1);
    Write (fd1, text3, sizeof (text3) - 1);
    Write (fd2, text4, sizeof (text4) - 1);

    Close (fd1);
    Close (fd2);

    fd1 = Open ("foobar1", O_RDONLY);
    Res = Read (fd1, Buf, sizeof (Buf));
    printf ("%.*s", Res, Buf);
    Res = Read (fd1, Buf, sizeof (Buf));
    Close (fd1);

    return 0;
}


