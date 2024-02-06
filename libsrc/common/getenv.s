;
; Ullrich von Bassewitz, 2005-04-21
;
; char* __fastcall__ getenv (const char* name);
;
; Beware: putenv() knows about zero page usage in this module!
;

        .export _getenv
        .import __environ, __envcount
        .import searchenv
        .import return0
        .import ptr1:zp, ptr3:zp, tmp1:zp

.code

;----------------------------------------------------------------------------
; getenv()

.proc   _getenv

        sta     ptr1
        stx     ptr1+1                  ; Save name

; Search for the string in the environment. searchenv will set the N flag if
; the string is not found, otherwise X contains the index of the entry, ptr3
; contains the entry and Y the offset of the '=' in the string.

        jsr     searchenv
        bpl     found
        jmp     return0                 ; Not found, return NULL

; Found the entry. Calculate the pointer to the right side of the environment
; variable. Because we want to skip the '=', we will set the carry.

found:  ldx     ptr3+1                  ; High byte of result
        tya
        sec
        adc     ptr3
        bcc     @L9
        inx
@L9:    rts

.endproc


