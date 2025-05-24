/* Note: This tests just if the #pragmas are understood. It doesn't test if
** they do really work. This would require much more work.
*/

void func(void);
#pragma align(push, 1024)
#pragma allow-eager-inline(push, on)
#pragma allow_eager_inline(pop)
#pragma bss-name(push, "BSS")
#pragma bss_name(pop)
#pragma charmap(1, 1)
#pragma check-stack(on)
#pragma check_stack(off)
#pragma code-name(push, "CODE")
#pragma code_name("CODE")
#pragma codesize(200)
#pragma data-name("DATA")
#pragma data_name("DATA")
#pragma inline-stdfuncs(off)
#pragma inline_stdfuncs(on)
#pragma local-strings(off)
#pragma local_strings(off)
#pragma message("in a bottle")
#pragma optimize(off)
#pragma register-vars(off)
#pragma register_vars(on)
#pragma regvaraddr(on)
#pragma rodata-name("RODATA")
#pragma rodata_name("RODATA")
#pragma signed-chars(off)
#pragma signed_chars(on)
#pragma static-locals(off)
#pragma static_locals(on)
#pragma warn(unused-param, on)
#pragma wrapped-call(push, func, 0)     // push is required for this #pragma
#pragma wrapped_call(push, func, 1)
#pragma writable-strings(on)
#pragma writable_strings(off)
#pragma zpsym("func")

int main ()
{
    return 0;
}
