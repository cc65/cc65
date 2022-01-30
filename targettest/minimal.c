 
/* this is a minimal / empty c program, any supported target that has some
 * sort of C support should be able to link this. Failure indicates a problem
 * with the crt0 or the linker config of the respective target */

char bss_variable;
char data_variable = 42;
const char rodata_variable = 23;

void main(void)
{
    /* nothing here */
}
