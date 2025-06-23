;
; Colin Leroy-Mira, 4 Sept. 2024
;
; char* stpcpy (char* dest, const char* src);
;

        .export         _stpcpy
        .import         _strcpy

        .importzp       tmp1, ptr2

_stpcpy:
        jsr     _strcpy

        ldx     ptr2+1        ; Load dest pointer's last high byte
        tya                   ; Get the last offset strcpy wrote to

        clc
        adc     ptr2          ; Add to low byte value
        bcc     :+
        inx
:       rts                   ; Return pointer to dest's terminator
