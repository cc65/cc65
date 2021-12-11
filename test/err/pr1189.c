/* pr #1189 - Fixed compiler CHECK failure when calling functions defined with duplicate param names */

void f(int a, int a)
{

}

int main(void)
{
    f(0, 1); /* Check failed: (Param->Flags & SC_PARAM) != 0 */
    return 0;
}
