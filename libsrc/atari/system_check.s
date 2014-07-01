;
; Atari startup system check
;
; This routine gets loaded prior to the main part of the executable
; and checks if the system is compatible to run the program.
; For the XL target it checks whether the system is an XL type one
; and that enough memory is installed (which isn't the case for a 600XL).
; For the non-XL target it checks whether there is enough memory
; installed to run the program.
; For both targets it checks that the program won't load below MEMLO.
; If one of the checks fails, the loading of the main program
; is aborted by jumping to DOSVEC.
;
; Christian Groessler, chris@groessler.org, 2013
;

;DEBUG   =       1

        .export         __SYSTEM_CHECK__: absolute = 1
        .import         __SYSCHK_LOAD__
        .import         __STARTADDRESS__

        ; the following imports are only needed for the 'atari' target version
        .import         __BSS_SIZE__, __BSS_RUN__
        .import         __STACKSIZE__
        .import         __RESERVED_MEMORY__

        .include        "zeropage.inc"
        .include        "atari.inc"


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

.macro print_string2 addr, len
        ldx     #0              ; channel 0
        lda     #<addr
        sta     ICBAL,x         ; address
        lda     #>addr
        sta     ICBAH,x
        lda     #<len
        sta     ICBLL,x         ; length
        lda     #>len
        sta     ICBLH,x
        lda     #PUTCHR
        sta     ICCOM,x
        jsr     CIOV_org
.endmacro


; ------------------------------------------------------------------------
; code

.segment        "SYSCHK"

.ifdef __ATARIXL__

lmemerrxl_txt:
        .byte   "Not enough memory to move screen", ATEOL
        .byte   "memory to low memory. Consider using", ATEOL
        .byte   "a higher load address.", ATEOL
lmemerrxl_txt_len = * - lmemerrxl_txt

; no XL machine
no_xl:  print_string "This program needs an XL machine."
        jmp     fail


; ***** entry point (atarixl) *****

syschk: lda     $fcd8           ; from ostype.s
        cmp     #$a2
        beq     no_xl

; we have an XL machine, now check memory
        lda     RAMSIZ
        cmp     #$80
        bcs     sys_ok

        jmp     mem_err

sys_ok:
        .include "xlmemchk.inc"         ; calculate lowest address we will use when we move the screen buffer down

        lda     MEMLO
        cmp     lowadr
        lda     MEMLO+1
        sbc     lowadr+1
        bcc     memlo_ok

; load address was too low
        print_string2 lmemerrxl_txt, lmemerrxl_txt_len
        jsr     delay           ; long text takes longer to read, give user additional time
        jmp     fail

.else   ; above 'atarixl', below 'atari'

.define CIOV_org CIOV           ; the print_string macros use CIOV_org, map this to CIOV

lmemerr_txt:
        .byte   "Program would load below MEMLO.", ATEOL
        .byte   "Consider using a higher load address.", ATEOL
lmemerr_txt_len = * - lmemerr_txt


; ***** entry point (atari) *****

syschk:
        sec
        lda     MEMTOP
        sbc     #<__RESERVED_MEMORY__
        sta     tmp
        lda     MEMTOP+1
        sbc     #>__RESERVED_MEMORY__
        sta     tmp+1
        lda     tmp
        sec
        sbc     #<__STACKSIZE__
        sta     tmp
        lda     tmp+1
        sbc     #>__STACKSIZE__
        sta     tmp+1

;tmp contains address which must be above .bss's end

        lda     tmp
        cmp     #<(__BSS_RUN__ + __BSS_SIZE__)
        lda     tmp+1
        sbc     #>(__BSS_RUN__ + __BSS_SIZE__)

        bcc     mem_err         ; program doesn't fit into memory

        lda     MEMLO
        cmp     #<__STARTADDRESS__
        lda     MEMLO+1
        sbc     #>__STARTADDRESS__
        bcc     memlo_ok

; load address was too low
        print_string2 lmemerr_txt, lmemerr_txt_len
        jsr     delay           ; long text takes longer to read, give user additional time
        jmp     fail

.endif

; all is well(tm), launch the application
memlo_ok:
.ifdef DEBUG
        print_string "Stage #1 OK"
        jsr     delay
.endif
        rts

; not enough memory
mem_err:print_string "Not enough memory."
fail:   jsr     delay
        jmp     (DOSVEC)

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
@loop:  dey
        bne     @loop
        dex
        bne     @loop
        rts

.endproc

end:

.ifndef __ATARIXL__
tmp:            ; outside of the load chunk, some kind of poor man's .bss
.endif

; ------------------------------------------------------------------------
; Chunk header

.segment        "SYSCHKHDR"

        .word   __SYSCHK_LOAD__
        .word   end - 1

; ------------------------------------------------------------------------
; Chunk "trailer" - sets INITAD

.segment        "SYSCHKTRL"

        .word   INITAD
        .word   INITAD+1
        .word   syschk
