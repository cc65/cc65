/* Bug #2301 - Function parameter list with an extra trailing comma should not compile */

int bar(a,) int a;  /* Should fail */
{
    return a;
}
