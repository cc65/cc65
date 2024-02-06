/* Bug #2151 - #pragma causes errors when used within functions */

#include <stdio.h>
#include <string.h>

#pragma charmap(0x61, 0x61)
_Static_assert('A'==
#pragma charmap(0x61, 0x41)
'a'
#pragma charmap(0x61, 0x42)
,
#pragma charmap(0x61, 0x61)
"charmap failed");

char str[] =
"a"
#pragma charmap(0x61, 0x42)
"a"
#pragma charmap(0x61, 0x43)
"a"
#pragma charmap(0x61, 0x61)
;

unsigned failures;

#pragma bss-name("BSS1")
int
#pragma code-name("CODE_WUT")
main _Pragma
#pragma charmap(0x61, 0x32)
(
"message(\"_Pragma string"
/* Concatenated string literals in _Pragma is a cc65 extension */
" unaffected by charmap\")"
)
#pragma charmap(0x61, 0x61)
(
void
_Pragma _Pragma (
#pragma message("nested message 1")
"message(\"nested message 2\")"
)
(
"message(\"_Pragma in function parentheses\")")
#pragma code-name("CODE")
)
#pragma bss-name("BSS")
{
    extern int y;
#pragma bss-name("BSS2")
    static
#pragma zpsym ("y")
    int x; // TODO: currently in "BSS", but supposed to be in "BSS2"?
    x = 0;

    if (memcmp(str, "aBC", 3))
    {
        ++failures;
        printf("%3s\n", str);
    }

    if (x + y != 0)
    {
        ++failures;
        printf("%d\n", x + y);
    }

    if (failures != 0)
    {
        printf("faiures: %d\n", failures);
    }

    return failures;
#pragma bss-name("BSS")
}

int y;
