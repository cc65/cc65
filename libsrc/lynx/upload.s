; ***
; CC65 Lynx Library
;
; Originally by Bastian Schick
; http://www.geocities.com/SiliconValley/Byte/4242/lynx/
;
; Ported to cc65 (http://www.cc65.org) by
; Shawn Jefferson, June 2004
;
; ***
;
; void install_uploader(unsigned char divider);
;
; Installs an interrupt handler that hooks the comlynx rx/tx interrupts to
; allow upload of code to the lynx from another computer via the comlynx
; cable.  divider values are in lynx.h
;
; Loader is installed under the mikey chip at $FE00.
;

        .include        "lynx.inc"
     	.export         _lynx_install_uploader
     	.import         _install_irq
     	.import         popa, pusha


; ------------------------------------------------------------------------

.segment        "EXTZP": zeropage

load_len:       .res    2
load_ptr:       .res    2
load_ptr2:      .res    2


      	.code

; ------------------------------------------------------------------------
; The following code will get moved to $FE00 when installing the uploader.

.proc   Loader

.org    $fe00

run:    ldy     #4
loop0:  jsr     read_byte
	sta     load_len-1,y
	dey
	bne     loop0       ; get destination and length
        tax                 ; lowbyte of length

        lda     load_ptr
        sta     load_ptr2
        lda     load_ptr+1
        sta     load_ptr2+1

loop1:  inx
        bne     cont1
        inc     load_len+1
        bne     cont1
        jmp     (load_ptr)

cont1:  jsr     read_byte
        sta     (load_ptr2),y
        sta     PALETTE         ; feedback ;-)
        iny
        bne     loop1
        inc     load_ptr2+1
        bra     loop1

read_byte:
        bit     SERCTL
        bvc     read_byte
        lda     SERDAT
        rts

.reloc

.endproc


; ------------------------------------------------------------------------

.code

_lynx_install_uploader:

        ldx     #.sizeof(Loader)-1      ; put Loader in the right place
loop:   lda     Loader,x                ; x is length of loader
        sta     Loader::run,x
        dex
        bpl     loop

;
; install serial-irq  vector
;
        lda     #4
        jsr     pusha
        lda     #<UpLoader
        ldx     #>UpLoader
        jsr     _install_irq    ; set vector
;
; set baudrate
;
        jsr     popa            ; get divider
        sta     $fd10
        lda     #%00011000      ; Baudrate = 1MHz/16/(divider+1)
        sta     $fd11
;
; set ComLynx parameters
;
        lda     #%00011101      ; even par
        sta     SERCTL          ; set 8E1
;
; clear Rx-buffer
;
clear:  bit     SERCTL
        bvc     ok0
        ldx     SERDAT
        bra     clear
;
; enable Rx-interrupt
;
ok0:    ora     #$40
        sta     SERCTL
        rts


; ------------------------------------------------------------------------
; Rx-IRQ-handler

UpLoader:
        lda     SERDAT          ; wait for the start sequence
        bit     flag            ; already seen $81 ?
        bpl     again           ; >= 0 => no
        cmp     #$50            ; "P" ?
        bne     again           ; not correct, so clear flag
        jmp     Loader::run

again:  stz     flag
        cmp     #$81
        bne     exit
        sta     flag
;
; last action : clear interrupt
;
exit:   lda     #$10
        sta     INTRST
        rts


.data

flag:           .byte   0

