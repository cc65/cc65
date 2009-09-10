;
; Oliver Schmidt, 30.12.2004
;
; Apple ProDOS 8 MLI
;

        .import         __dos_type
        .import         iobuf_init, iobuf_done
        .export         iobuf_nop

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
        jsr     iobuf_init
        jsr     ENTRY
call:   .byte   $00
        .addr   mliparam
        jmp     iobuf_done

        ; Load oserror code and return
oserr:  lda     #$01		; "Invalid MLI function code number"
        sec
        ; Fall through

iobuf_nop:
        rts
