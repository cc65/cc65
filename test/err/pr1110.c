
/* pr #1110 - not only should the current test case for #975 compile and work, 
 * but also the code piece below fail to compile and generate errors like commented: */

static const unsigned char array[3];                /* OK */
static const unsigned char array[] = { 0, 1, 2 };   /* OK - complete definition*/
static const unsigned char array[3];                /* OK */
static const unsigned char array[];                 /* OK */
static const unsigned char array[] = { 1, 2, 3 };   /* Error - redefinition */
static const unsigned char array[4];                /* Error - conflicting size */ 

int main(void)
{
    return 0;
}
