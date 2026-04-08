int main(void)
{
    int BaseTest = 0;
    for (BaseTest = 0; BaseTest < 6502; BaseTest++)
    {
    }

    if (BaseTest != 6502) {
        return 1;
    }

    int DeclAfterStart = 0;
    for (DeclAfterStart = 0; DeclAfterStart < 6502; DeclAfterStart++)
    {
    }

    if (DeclAfterStart != 6502)
    {
        return 1;
    }

    int DeclInFor = 0;
    for (int i = 0; i < 6502; i++)
    {
        DeclInFor++;
    }

    if (DeclInFor != 6502)
    {
        return 1;
    }

    /* Should compile fine */
    for (int i = 0; i < 6502; i++)
    {
    }

    return 0;
}
