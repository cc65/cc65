;
; Freddy Offenga, Stefan Haubenthal, Christian Groessler, March 2007
;
; detect the DOS version we're running on
;

        .include        "atari.inc"
        .constructor    detect, 26
        .export         __dos_type

; ------------------------------------------------------------------------
; DOS type detection

.segment        "ONCE"

detect: lda     DOS
        cmp     #'S'            ; SpartaDOS
        beq     spdos
        cmp     #'M'            ; MyDOS
        beq     mydos
        cmp     #'X'            ; XDOS
        beq     xdos
        cmp     #'R'            ; RealDOS
        beq     rdos

        lda     #$4C            ; probably default
        ldy     #COMTAB
        cmp     (DOSVEC),y
        bne     done
        ldy     #ZCRNAME
        cmp     (DOSVEC),y
        bne     done

        ldy     #6              ; OS/A+ has a jmp here
        cmp     (DOSVEC),y
        beq     done
        lda     #OSADOS
        .byte   $2C             ; BIT <abs>

spdos:  lda     #SPARTADOS
        .byte   $2C             ; BIT <abs>

mydos:  lda     #MYDOS
        .byte   $2C             ; BIT <abs>

rdos:   lda     #REALDOS
        .byte   $2C             ; BIT <abs>

xdos:   lda     #XDOS
        sta     __dos_type
done:   rts

; ------------------------------------------------------------------------
; Data

        .data

__dos_type:     .byte   ATARIDOS; default to ATARIDOS
