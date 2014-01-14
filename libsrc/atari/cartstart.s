; Cartridge start routine
;
; Christian Groessler, 06-Jan-2014

.ifndef __ATARIXL__

.export         cartstart

.import         start
.import         __DATA_LOAD__, __DATA_SIZE__, __DATA_RUN__
.importzp       ptr1, ptr2, tmp1, tmp2

.include        "atari.inc"

.segment         "STARTUP"

; start routine of cartridge
; copy data segment to RAM and chain to entry point of crt0.s

cartstart:      lda     #<__DATA_LOAD__
                sta     ptr1
                lda     #>__DATA_LOAD__
                sta     ptr1+1
                lda     #<__DATA_RUN__
                sta     ptr2
                lda     #>__DATA_RUN__
                sta     ptr2+1
                lda     #>__DATA_SIZE__
                sta     tmp2
                lda     #<__DATA_SIZE__
                sta     tmp1
                jsr     memcopy
                jsr     start                   ; run program
                jmp     (DOSVEC)                ; return to DOS


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
pagecp:         lda     (ptr1),y
                sta     (ptr2),y
                iny
                bne     pagecp
                inc     ptr1+1
                inc     ptr2+1
                dex
                bne     pagecp
last:           cpy     tmp1
                beq     done
                lda     (ptr1),y
                sta     (ptr2),y
                iny
                bne     last
done:           rts

.endproc

.endif  ; .ifndef __ATARIXL__
