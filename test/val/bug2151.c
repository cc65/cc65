/* Bug #2151 - #pragma causes errors when used within functions */

#pragma bss-name("BSS1")
int
#pragma code-name("CODE_WUT")
main _Pragma("message(\"_Pragma note\")")
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
    return x + y;
#pragma bss-name("BSS")
}

int y;
