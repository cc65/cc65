;
; Christian Groessler, Jul-2005
;
; int __fastcall__ read(int fd,void *buf,int count)
;

        .include "atari.inc"
        .import __rwsetup,__do_oserror,__inviocb,__oserror
        .export _read

_read:  jsr     __rwsetup       ; do common setup for read and write
        beq     done            ; if size 0, it's a no-op
        cpx     #$FF            ; invalid iocb?
        beq     _inviocb

.ifdef  LINEBUF
        ; E: should be always at IOCB #0
        ; fixme: what happens when user closes and reopens stdin?
        cpx     #0              ; E: handler (line oriented keyboard input)?
        beq     do_line
.endif

        lda     #GETCHR         ; iocb command code
        sta     ICCOM,x
        jsr     CIOV            ; read it
        bpl     done
        cpy     #EOFERR         ; eof is treated specially
        beq     done
        jmp     __do_oserror    ; update errno

done:   lda     ICBLL,x         ; buf len lo
        pha                     ; save
        lda     ICBLH,x         ; get buf len hi
        tax                     ; to X
okdone: lda     #0
        sta     __oserror       ; clear system dependend error code
        pla                     ; get buf len lo
        rts

_inviocb:
        jmp     __inviocb


.ifdef  LINEBUF

; line oriented input

        .segment        "EXTZP" : zeropage

index:  .res    1               ; index into line buffer
cbs:    .res    1               ; current buffer size: buflen - index
dataptr:.res    2               ; temp pointer to user buffer
copylen:.res    1               ; temp counter

        .bss

buflen: .res    1               ; length of used part of buffer
linebuf:.res    LINEBUF         ; the line buffer

        .code

do_line:
        lda     buflen          ; line buffer active?
        bne     use_buf         ; yes, get data from there

        ; save user buffer address & length
        ; update IOCB to point to line buffer
        lda     ICBLL,x
        pha
        lda     #LINEBUF
        sta     ICBLL,x
        ;--------
        lda     ICBLH,x
        pha
        lda     #0
        sta     ICBLH,x
        ;--------
        lda     ICBAL,x
        pha
        lda     #<linebuf
        sta     ICBAL,x
        ;--------
        lda     ICBAH,x
        pha
        lda     #>linebuf
        sta     ICBAH,x

        lda     #GETREC
        sta     ICCOM,x
        jsr     CIOV            ; read input data
        bpl     newbuf
        cpy     #EOFERR         ; eof is treated specially
        beq     newbuf
        pla                     ; fix stack
        pla
        pla
        pla
        jmp     __do_oserror    ; update errno

newbuf:
        lda     ICBLL,x         ; get # of bytes read
        sta     buflen
        lda     #0
        sta     index           ; fresh buffer

        ; restore user buffer address & length
        pla
        sta     ICBAH,x
        ;--------
        pla
        sta     ICBAL,x
        ;--------
        pla
        sta     ICBLH,x
        ;--------
        pla
        sta     ICBLL,x

        ; fall into use_buf
        lda     buflen

; return bytes from line buffer
; use buflen and index to access buffer
; update index
; use dataptr as a temporary pointer

use_buf:
        sec
        sbc     index           ; size of unread data in the buffer
        sta     cbs

        lda     ICBLL,x         ; buf len lo
        cmp     cbs             ; larger than buffer size?
        beq     bl1
        bcs     btsmall         ; yes, adjust length

bl1:    lda     ICBLH,x         ; get buf len hi
        bne     btsmall         ; buffer too small: buffer contents < read size

; copy ICBLL,x bytes

icbll_copy:

        lda     ICBAL,x         ; buffer address
        sta     dataptr
        lda     ICBAH,x         ; buffer address
        sta     dataptr+1
        lda     ICBLL,x
        sta     copylen
        pha                     ; remember for return value
        ldy     #0
        ldx     index

copy:   lda     linebuf,x
        sta     (dataptr),y
        iny
        inx
        dec     copylen
        bne     copy

        pla                     ; length
        pha                     ; save length to return at okdone

        clc
        adc     index
        sta     index
        cmp     buflen          ; buffer used up?
        bcc     c1              ; not yet

        lda     #0
        sta     buflen          ; indicate empty line buffer

c1:     ldx     #0
        jmp     okdone          ; return to caller

btsmall:
        lda     cbs
        sta     ICBLL,x
        bpl     icbll_copy

.endif          ; .ifdef LINEBUF

