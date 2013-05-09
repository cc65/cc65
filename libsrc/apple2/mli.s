;
; Oliver Schmidt, 30.12.2004
;
; Apple ProDOS 8 MLI
;

        .import         __dos_type

        .include        "mli.inc"

        .bss

mliparam:.tag   MLI

        .data

callmli:
        ; Store parameters
        sta     call
        stx     mliparam

        ; Check for ProDOS 8
        lda     __dos_type
        beq     oserr

        ; Call MLI and return
        jsr     $BF00           ; MLI call entry point
call:   .byte   $00
        .addr   mliparam
        rts

        ; Load oserror code and return
oserr:  lda     #$01            ; "Bad system call number"
        sec
        rts
