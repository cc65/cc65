;
; Ullrich von Bassewitz, 2003-08-12
;
; This module contains a utility function for the machine dependent parts
; of uname (__sysuname): It copies a packed struct utsname (just the strings
; without padding) into a struct utsname. The source of the data is an
; external symbol named "utsdata", the target is passed in a/x.
; The function is written so that it is a direct replacement for __sysuname
; for systems where utsdata is fixed. It may also be called via jump or
; subroutine on systems where utsdata must be changed at runtime.
;

        .export         utscopy

        .import         utsdata
        .importzp       ptr1, tmp1

        .include        "utsname.inc"



;--------------------------------------------------------------------------
; Data.

.rodata

; Table with offsets into struct utsname
fieldoffs:
        .byte   utsname::sysname
        .byte   utsname::nodename
        .byte   utsname::release
        .byte   utsname::version
        .byte   utsname::machine

fieldcount = * - fieldoffs

;--------------------------------------------------------------------------

.code

.proc   utscopy

        sta     ptr1
        stx     ptr1+1          ; Save buf

        ldx     #0
        stx     tmp1            ; Field number

next:   ldy     tmp1
        cpy     #fieldcount
        beq     done
        inc     tmp1            ; Bump field counter
        lda     fieldoffs,y     ; Get next field offset
        tay                     ; Field offset -> y

loop:   lda     utsdata,x
        sta     (ptr1),y
        inx                     ; Next char in utsdata
        cmp     #$00            ; Check for end of string
        beq     next            ; Jump for next field
        iny                     ; Next char in utsname struct
        bne     loop            ; Copy string

done:   lda     #$00            ; Always successful
        rts

.endproc


