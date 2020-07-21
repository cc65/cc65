; mul40.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
;
; unsigned int __fastcall__ mul40(unsigned char value);
; 
; REMARKS: Function is defined to return with carry-flag cleared


        .importzp       tmp4
        .export         _mul40

.proc   _mul40                  ; = 33 bytes, 48/53 cycles

        sta     tmp4            ; remember value for later addition...
        ldx     #0              ; clear high-byte
        asl     a               ; * 2
        bcc     mul4            ; high-byte affected?
        ldx     #2              ; this will be the 1st high-bit soon...

mul4:   asl     a               ; * 4
        bcc     mul5            ; high-byte affected?
        inx                     ; => yes, apply to 0 high-bit
        clc                     ; prepare addition

mul5:   adc     tmp4            ; * 5
        bcc     mul10           ; high-byte affected?
        inx                     ; yes, correct...

mul10:  stx     tmp4            ; continue with classic shifting...
        
        asl     a               ; * 10
        rol     tmp4

        asl     a               ; * 20
        rol     tmp4

        asl     a               ; * 40
        rol     tmp4

        ldx     tmp4            ; deliver high-byte in X
        rts

.endproc
