
/* bug #1933 - wrong printf specifier breaks data lines */

unsigned char info_signature[3] = {3, 21, 63 | 0x80};

int main(void)
{
    return 0;
}
