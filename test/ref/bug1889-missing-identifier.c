/* bug 1889 - endless errors due to failure in recovery from missing identifier */

int enum { a } x;
inline enum { b };

int main(void)
{
    return 0;
}
