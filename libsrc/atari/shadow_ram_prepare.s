;
; Atari XL shadow RAM preparation routines
;
; Tasks:
; - move screen memory below load address
; - copy ROM chargen to its new place
; - copy shadow RAM contents to their destination
;
; Christian Groessler, chris@groessler.org, 2013
;

;DEBUG   =       1

.ifdef __ATARIXL__

        .export         sramprep

        .import         __SRPREP_LOAD__, __SRPREPCHNK_LAST__
        .import         __SHADOW_RAM_LOAD__, __SHADOW_RAM_SIZE__, __SHADOW_RAM_RUN__
        .import         __SHADOW_RAM2_LOAD__, __SHADOW_RAM2_SIZE__, __SHADOW_RAM2_RUN__
        .import         __CHARGEN_START__, __CHARGEN_SIZE__
        .import         __STARTADDRESS__       ; needed by xlmemchk.inc

        .include        "zeropage.inc"
        .include        "atari.inc"
        .include        "save_area.inc"

.macro print_string text
        .local  start, cont
        jmp     cont
start:  .byte   text, ATEOL
cont:   ldx     #0              ; channel 0
        lda     #<start
        sta     ICBAL,x         ; address
        lda     #>start
        sta     ICBAH,x
        lda     #<(cont - start)
        sta     ICBLL,x         ; length
        lda     #>(cont - start)
        sta     ICBLH,x
        lda     #PUTCHR
        sta     ICCOM,x
        jsr     CIOV_org
.endmacro

; ------------------------------------------------------------------------
; EXE load chunk header

.segment        "SRPREPHDR"

        .word   __SRPREP_LOAD__
        .word   __SRPREPCHNK_LAST__ - 1

; ------------------------------------------------------------------------
; Actual code

.segment        "SRPREP"

; ***** entry point *****

sramprep:
.ifdef DEBUG
        print_string "entering stage #2"
.endif

; save values of modified system variables and ports

        lda     RAMTOP
        sta     RAMTOP_save
        lda     MEMTOP
        sta     MEMTOP_save
        lda     MEMTOP+1
        sta     MEMTOP_save+1
        lda     APPMHI
        sta     APPMHI_save
        lda     APPMHI+1
        sta     APPMHI_save+1
        lda     PORTB
        sta     PORTB_save

; disable BASIC

        lda     PORTB
        ora     #2
        sta     PORTB

        .include "xlmemchk.inc" ; calculate lowest address used and new value for RAMTOP

        ldx     lowadr
        stx     MEMTOP
        lda     lowadr+1
        sta     MEMTOP+1
        lda     lodadr+1
        sta     RAMTOP

        ; set APPMHI to MEMLO (+ 1 for sanity)
        lda     MEMLO
        clc
        adc     #1
        sta     APPMHI
        lda     MEMLO+1
        adc     #0
        sta     APPMHI+1


; issue a GRAPHICS 0 call (copied'n'pasted from TGI drivers) to move screen memory down


        jsr     findfreeiocb
.ifdef DEBUG            ; only check in debug version, this shouldn't really happen(tm)
        beq     iocbok
        print_string "Internal error, no free IOCB!"
        jsr     delay
        jsr     delay
        jsr     delay
        jsr     restore                 ; restore stuff we've changed
        jmp     (DOSVEC)                ; abort loading
iocbok:
.endif

        ; reopen it in Graphics 0
        lda     #OPEN
        sta     ICCOM,x
        lda     #OPNIN | OPNOT
        sta     ICAX1,x
        lda     #0
        sta     ICAX2,x
        lda     #<screen_device
        sta     ICBAL,x
        lda     #>screen_device
        sta     ICBAH,x
        lda     #<screen_device_length
        sta     ICBLL,x
        lda     #>screen_device_length
        sta     ICBLH,x
        jsr     CIOV_org
        bpl     scrok

; shouldn't happen(tm)
        print_string "Internal error, aborting..."
        jsr     delay
        jsr     delay
        jsr     delay
        jsr     restore                 ; restore stuff we've changed
        jmp     (DOSVEC)                ; abort loading


scrok:  ; now close it again -- we don't need it anymore
        lda     #CLOSE
        sta     ICCOM,x
        jsr     CIOV_org


; copy chargen to low memory, just after the next possible address beyond our loaded chunk data

.ifdef DEBUG
        print_string "copy chargen to low memory"
.endif

        lda     #>__SRPREPCHNK_LAST__
        sta     ptr3+1
        lda     #<__SRPREPCHNK_LAST__
        sta     ptr3
        beq     cg_addr_ok

        ; page align the new chargen address
        inc     ptr3+1
        lda     #0
        sta     ptr3

cg_addr_ok:

        lda     ptr3+1
        and     #3
        beq     cg_addr_ok2

        ; align to next 1K boundary
        lda     ptr3+1
        and     #$fc
        clc
        adc     #4
        sta     ptr3+1

cg_addr_ok2:

        lda     #<DCSORG
        sta     ptr1
        lda     #>DCSORG
        sta     ptr1+1
        lda     ptr3
        sta     ptr2
        lda     ptr3+1
        pha                             ; needed later to set CHBAS/CHBASE
        sta     ptr2+1
        lda     #>__CHARGEN_SIZE__
        sta     tmp2
        lda     #<__CHARGEN_SIZE__
        sta     tmp1
        jsr     memcopy

.ifdef DEBUG
        print_string "now setting up high memory"
.endif

; disable ROM
        sei
        ldx     #0
        stx     NMIEN                   ; disable NMI
        lda     PORTB
        and     #$fe
        tax
        pla                             ; get temp. chargen address
        sta     WSYNC                   ; wait for horiz. retrace
        stx     PORTB                   ; now ROM is mapped out

; switch to temporary chargen

        sta     CHBASE
        sta     CHBAS

; copy shadow RAM contents to their destination (segment SHADOW_RAM)

        lda     #<__SHADOW_RAM_SIZE__
        bne     do_copy
        lda     #>__SHADOW_RAM_SIZE__
        beq     no_copy                 ; we have no shadow RAM contents

        ; ptr1 - src; ptr2 - dest; tmp1, tmp2 - len
do_copy:lda     #<__SHADOW_RAM_LOAD__
        sta     ptr1
        lda     #>__SHADOW_RAM_LOAD__
        sta     ptr1+1
        lda     #<__SHADOW_RAM_RUN__
        sta     ptr2
        lda     #>__SHADOW_RAM_RUN__
        sta     ptr2+1
        lda     #<__SHADOW_RAM_SIZE__
        sta     tmp1
        lda     #>__SHADOW_RAM_SIZE__
        sta     tmp2

        jsr     memcopy

no_copy:

; copy shadow RAM #2 contents to their destination (segment SHADOW_RAM2)

        lda     #<__SHADOW_RAM2_SIZE__
        bne     do_copy2
        lda     #>__SHADOW_RAM2_SIZE__
        beq     no_copy2                ; we have no shadow RAM #2 contents

        ; ptr1 - src; ptr2 - dest; tmp1, tmp2 - len
do_copy2:
        lda     #<__SHADOW_RAM2_LOAD__
        sta     ptr1
        lda     #>__SHADOW_RAM2_LOAD__
        sta     ptr1+1
        lda     #<__SHADOW_RAM2_RUN__
        sta     ptr2
        lda     #>__SHADOW_RAM2_RUN__
        sta     ptr2+1
        lda     #<__SHADOW_RAM2_SIZE__
        sta     tmp1
        lda     #>__SHADOW_RAM2_SIZE__
        sta     tmp2

        jsr     memcopy

no_copy2:

; copy chargen to its new (final) location

        lda     ptr3
        sta     ptr1
        lda     ptr3+1
        sta     ptr1+1
        lda     #<__CHARGEN_START__
        sta     ptr2
        lda     #>__CHARGEN_START__
        sta     ptr2+1
        lda     #>__CHARGEN_SIZE__
        sta     tmp2
        lda     #<__CHARGEN_SIZE__
        sta     tmp1
        jsr     memcopy

; re-enable ROM

        lda     PORTB
        ora     #1
        ldx     #>DCSORG
        sta     WSYNC                   ; wait for horiz. retrace
        sta     PORTB
        stx     CHBASE
        stx     CHBAS
        lda     #$40
        sta     NMIEN                   ; enable VB again
        cli                             ; and enable IRQs

.ifdef DEBUG
        print_string "Stage #2 OK"
        print_string "loading main chunk"
        jsr     delay
.endif
        rts

.include "findfreeiocb.inc"

; routine taken from http://www.obelisk.demon.co.uk/6502/algorithms.html
;
; copy memory
; ptr1      - source
; ptr2      - destination
; tmp2:tmp1 - len

.proc   memcopy

        ldy     #0
        ldx     tmp2
        beq     last
pagecp: lda     (ptr1),y
        sta     (ptr2),y
        iny
        bne     pagecp
        inc     ptr1+1
        inc     ptr2+1
        dex
        bne     pagecp
last:   cpy     tmp1
        beq     done
        lda     (ptr1),y
        sta     (ptr2),y
        iny
        bne     last
done:   rts

.endproc


; clean up after a fatal error

restore:lda     RAMTOP_save
        sta     RAMTOP
        lda     MEMTOP_save
        sta     MEMTOP
        lda     MEMTOP_save+1
        sta     MEMTOP+1
        lda     APPMHI_save
        sta     APPMHI
        lda     APPMHI_save+1
        sta     APPMHI+1
        rts

; short delay
.proc   delay

        lda     #10
@loop:  jsr     delay1
        clc
        sbc     #0
        bne     @loop
        rts

delay1: ldx     #0
        ldy     #0
@loop:   dey
        bne     @loop
        dex
        bne     @loop
        rts

.endproc

.ifdef DEBUG

.byte "HERE ****************** HERE ***************>>>>>>"

sramsize:
        .word   __SHADOW_RAM_SIZE__

.endif          ; .ifdef DEBUG

screen_device:  .byte "S:",0
screen_device_length = * - screen_device

.ifdef DEBUG
        .byte   " ** srprep ** end-->"
.endif

; ------------------------------------------------------------------------
; Provide empty SHADOW_RAM and SHADOW_RAM2 segments in order that the
; linker is happy if the user program doesn't have these segments.

.segment        "SHADOW_RAM"
.segment        "SHADOW_RAM2"


; ------------------------------------------------------------------------
; EXE load chunk "trailer" - sets INITAD

.segment        "SRPREPTRL"

        .word   INITAD
        .word   INITAD+1
        .word   sramprep

.endif  ; .ifdef __ATARIXL__
