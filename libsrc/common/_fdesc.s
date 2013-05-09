;
; Ullrich von Bassewitz, 17.06.1998
;
; int _fdesc (void);
; /* Find a free descriptor slot */


        .export         __fdesc
        .import         return0

        .include        "stdio.inc"
        .include        "_file.inc"

.proc   __fdesc

        ldy     #0
        lda     #_FOPEN
Loop:   and     __filetab + _FILE::f_flags,y    ; load flags
        beq     Found                           ; jump if closed
.repeat .sizeof(_FILE)
        iny
.endrepeat
        cpy     #(FOPEN_MAX * .sizeof(_FILE))   ; Done?
        bne     Loop

; File table is full

        jmp     return0

; Free slot found, get address

Found:  tya                     ; Offset
        clc
        adc     #<__filetab
        ldx     #>__filetab     ; High byte
        bcc     @L1             ; Jump if no overflow
        inx                     ; Bump high byte
@L1:    rts

.endproc



