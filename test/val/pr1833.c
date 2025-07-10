/* Test for PR #1833 fixes */

#define char 1

#if char && !int && L'A' - L'B' == 'A' - 'B' && L'A' == 'A'
#else
#error
#endif

int main(void)
{
    return 0;
}
