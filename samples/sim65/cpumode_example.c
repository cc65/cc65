/*
 * Sim65 cpu-mode switching example.
 *
 * Description
 * -----------
 *
 * We can inspect and manipulate the CPU model that sim65 emulates at runtime.
 *
 * Sim65 always runs in one of three modes:
 *
 * - 6502 mode: the 151 documented opcodes are supported; if the processor encounters
 *              one of the 105 undocumented opcodes, the simulator ends with an
 *              'illegal opcode' message.
 * - 65C02 mode: the 105 undocumented opcodes now have well-defined behavior. Some
 *               do useful things, while all others are now defined as NOPs.
 * - 6502X mode: the 105 undocumented opcodes don't have documented behavior, but
 *               they /do/ have behavior on a real 6502. This behavior has been
 *               figured out, and is deterministic (with minor exceptions).
 *               In this mode, sim65 mimics the behavior of a real 6502 when
 *               it encounters an undocumented opcode, rather than terminating.
 *
 * In the example below, we first switch to 6502X mode and execute a small
 * assembly code fragment, then repeat this in 65C02 mode.
 *
 * The code fragment is designed to distinguish between a 6502 and a 65C02
 * processor based on the behavior of the ADC function in decimal mode.
 *
 * Important Note:
 *
 * When running in a program compiled for the "sim6502" target, it is safe to switch to
 * 65C02 or 6502X mode, since the runtime library will only use plain 6502 opcodes, and
 * those work the same in 65C02 and 6502X mode.
 *
 * However, when running in a program compiled for the "sim65c02" target, it is NOT safe
 * to switch to 6502 or 6502X mode, since many routines in the runtime library use
 * 65C02-specific opcodes, and these will not work as expected when the CPU is switched
 * to 6502 or 6502X mode. When such an instruction is encountered, the program will
 * exhibit undefined behavior.
 *
 * For this reason, this program will only work when compiled for the "sim6502" target.
 *
 * Running the example
 * -------------------
 *
 * cl65 -t sim6502 -O cpumode_example.c -o cpumode_example.prg
 * sim65 cpumode_example.prg
 *
 */

#include <stdio.h>
#include <stdbool.h>
#include <sim65.h>

static bool __fastcall__ is_65c02(void)
{
    /* This assembly routine loads 0 into AX on a 6502 (also on a 6502 on which decimal
     * mode is not implemented), and 1 on a 65C02.
     *
     * Note: this implementation triggers a "control reaches end of non-void function"
     * warning that can be safely ignored. While no return statement is present, the
     * return value is correctly loaded into AX by the assembly code.
     */
    __asm__("sed");
    __asm__("ldx #0");
    __asm__("txa");
    __asm__("sbc #155");
    __asm__("asl");
    __asm__("rol");
    __asm__("and #1");
    __asm__("cld");
}

int main(void)
{
    printf("CPU mode at startup ....... : %u\n", GET_CPU_MODE());
    printf("Is 65C02? ................. : %s\n", is_65c02() ? "YES" : "NO");

    printf("\n");

    printf("Switching to 6502 mode ....\n");
    SET_CPU_MODE(SIM65_CPU_MODE_6502);
    printf("Current CPU mode .......... : %u\n", GET_CPU_MODE());
    printf("Is 65C02? ................. : %s\n", is_65c02() ? "YES" : "NO");

    printf("\n");

    printf("Switching to 65C02 mode ...\n");
    SET_CPU_MODE(SIM65_CPU_MODE_65C02);
    printf("Current CPU mode .......... : %u\n", GET_CPU_MODE());
    printf("Is 65C02? ................. : %s\n", is_65c02() ? "YES" : "NO");

    printf("\n");

    printf("Switching to 6502X mode ...\n");
    SET_CPU_MODE(SIM65_CPU_MODE_6502X);
    printf("Current CPU mode .......... : %u\n", GET_CPU_MODE());
    printf("Is 65C02? ................. : %s\n", is_65c02() ? "YES" : "NO");

    printf("\n");

    printf("Bye!\n");

    return 0;
}
