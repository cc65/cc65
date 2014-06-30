/*
** seek test program
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    char *filename,*x;
    char buf[20];
    FILE *file;
    long pos;
    off_t fsz;
    int fd;

    if (argc <= 1) {
        printf("\nfilename: ");
        x = fgets(buf,19,stdin);
        printf("\n");
        if (!x) {
            return(0);
        }
        filename = x;
    }
    else {
        filename = *(argv+1);
    }

    file = fopen(filename,"rb");
    if (!file) {
        fprintf(stderr,"cannot open %s: %s\n",filename,strerror(errno));
        return(1);
    }

    if (fread(buf, 10, 1, file) != 1) {
        fprintf(stderr,"short read, aborted\n");
        fclose(file);
        return(1);
    }

    pos = ftell(file);
    if (pos == -1) {
        fprintf(stderr,"ftell returned -1: %s\n", strerror(errno));
        fclose(file);
        return(1);
    }

    printf("reading 10 bytes from file\n");
    printf("current file pos: %ld\n", pos);

    printf("get file size (lseek): ");
    fd = *(char *)file;   /* kids, don't do this at home */
    fsz = lseek(fd, 0, SEEK_END);
    if (fsz == -1) {
        fprintf(stderr,"lseek returned -1: %s\n", strerror(errno));
        fclose(file);
        return(1);
    }
    printf("%ld (fd = %d)\n", (long)fsz, fd);

    printf("get file size (fseek): ");
    pos = fseek(file, 0, SEEK_END);
    if (pos != 0) {
        fprintf(stderr,"fseek returned -1: %s\n", strerror(errno));
        fclose(file);
        return(1);
    }

    pos = ftell(file);
    if (pos == -1) {
        fprintf(stderr,"ftell returned -1: %s\n", strerror(errno));
        fclose(file);
        return(1);
    }
    printf("%ld\n",pos);

    printf("positioning at offset 100: ");
    pos = fseek(file, 100, SEEK_SET);
    if (pos != 0) {
        fprintf(stderr,"fseek returned -1: %s\n", strerror(errno));
        fclose(file);
        return(1);
    }
    pos = ftell(file);
    if (pos == -1) {
        fprintf(stderr,"ftell returned -1: %s\n", strerror(errno));
        fclose(file);
        return(1);
    }
    if (pos == 100) {
        printf("Ok\n");
    }
    else {
        printf("failed! cur pos = %ld\n",pos);
        fclose(file);
        return(1);
    }

    printf("seeking back 44 bytes: ");
    pos = fseek(file, -44, SEEK_CUR);
    if (pos != 0) {
        fprintf(stderr,"fseek returned -1: %s\n", strerror(errno));
        fclose(file);
        return(1);
    }
    pos = ftell(file);
    if (pos == -1) {
        fprintf(stderr,"ftell returned -1: %s\n", strerror(errno));
        fclose(file);
        return(1);
    }
    if (pos == 56) {
        printf("Ok\n");
    }
    else {
        printf("failed! cur pos = %ld\n",pos);
        fclose(file);
        return(1);
    }

    printf("seeking forward 111 bytes: ");
    pos = fseek(file, 111, SEEK_CUR);
    if (pos != 0) {
        fprintf(stderr,"fseek returned -1: %s\n", strerror(errno));
        fclose(file);
        return(1);
    }
    pos = ftell(file);
    if (pos == -1) {
        fprintf(stderr,"ftell returned -1: %s\n", strerror(errno));
        fclose(file);
        return(1);
    }
    if (pos == 167) {
        printf("Ok\n");
    }
    else {
        printf("failed! cur pos = %ld\n",pos);
        fclose(file);
        return(1);
    }

    printf("seeking 13 bytes before eof: ");
    pos = fseek(file, -13, SEEK_END);
    if (pos != 0) {
        fprintf(stderr,"fseek returned -1: %s\n", strerror(errno));
        fclose(file);
        return(1);
    }
    pos = ftell(file);
    if (pos == -1) {
        fprintf(stderr,"ftell returned -1: %s\n", strerror(errno));
        fclose(file);
        return(1);
    }
    if (pos == fsz - 13) {
        printf("Ok\n");
    }
    else {
        printf("failed! cur pos = %ld\n",pos);
        fclose(file);
        return(1);
    }

    printf("seeking before sof:\n\t");
    pos = fseek(file, -fsz, SEEK_CUR);
    if (pos != 0) {
        printf("Ok, error %s\n", strerror(errno));
    }
    else {
        printf("NOT OK, no error\n");
    }

    fclose(file);
    return(0);
}
