;
; Oliver Schmidt, 15.09.2009
;
; ProDOS 8 I/O buffer management for memory between
; location $0800 and the cc65 program start address
;

        .constructor    initiobuf
        .export         iobuf_alloc, iobuf_free
        .import         __MAIN_START__
        .import         incsp2, popptr1

        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "../filedes.inc"

        .segment        "ONCE"

initiobuf:
        ; Convert end address highbyte to table index
        lda     #>__MAIN_START__
        sec
        sbc     #>$0800
        lsr
        lsr

        ; Mark all remaining table entries as used
        tax
        lda     #$FF
:       cpx     #MAX_FDS
        bcc     :+
        rts
:       sta     table,x
        inx
        bne     :--             ; Branch always

; ------------------------------------------------------------------------

        .code

iobuf_alloc:
        ; Get and save "memptr"
        jsr     incsp2
        jsr     popptr1

        ; Search table for free entry
        ldx     #$00
:       lda     table,x
        beq     :+
        inx
        cpx     #MAX_FDS
        bcc     :-
        lda     #ENOMEM
        rts

        ; Mark table entry as used
:       dec     table,x

        ; Convert table index to address hibyte
        txa
        asl
        asl
        ; Skip clearing carry, it can't be set as long as MAX_FDS*4 is
        ; less than 64.
        .assert MAX_FDS*4 < $40, error
        adc     #>$0800

        ; Store address in "memptr"
        ; (Y still equals 0 from popptr1)
        iny
        sta     (ptr1),y
        dey
        tya
        sta     (ptr1),y
        rts

iobuf_free:
        ; Convert address hibyte to table index
        txa
        sec
        sbc     #>$0800
        lsr
        lsr

        ; Mark table entry as free
        tax
        inc     table,x
        rts

; ------------------------------------------------------------------------

        .data

table:  .res    MAX_FDS
