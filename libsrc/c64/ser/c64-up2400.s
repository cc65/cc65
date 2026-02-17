; This is an implementation of a reliable 2400b serial driver for the
; userport of a C64.
;
; Johan Van den Brande, (c) 2014
;
; Based on George Hug's 'Towards 2400' article in the Transactor Magazine volume 9 issue 3.
; (https://archive.org/details/transactor-magazines-v9-i03)
;
; I used parts of this fix to get around the 0x0d problem when a 0x00 byte is received.
; (https://modelrail.otenko.com/electronics/commodore-64-fixing-rs-232-serial-limitations)

        .include        "cbm_kernal.inc"
        .include        "zeropage.inc"
        .include        "ser-kernel.inc"
        .include        "ser-error.inc"
        .include        "c64.inc"

        .macpack        module

; ------------------------------------------------------------------------
; Constants

; FIXME: use more constants for the zeropage locations used in the code below
; FIXME: move most (if not all) kernal locations into c64.inc (or cbm_kernal.inc)

ribuf  = $f7
robuf  = $f9

oldout = $f1ca
oldchk = $f21b
findfn = $f30f
devnum = $f31f
nofile = $f701
rstkey = $fe56
norest = $fe72
return = $febc

; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _c64_up2400_ser

; Driver signature

        .byte   $73, $65, $72           ; "ser"
        .byte   SER_API_VERSION         ; Serial API version number

; Library reference

        .addr   $0000

; Jump table

        .word   SER_INSTALL
        .word   SER_UNINSTALL
        .word   SER_OPEN
        .word   SER_CLOSE
        .word   SER_GET
        .word   SER_PUT
        .word   SER_STATUS
        .word   SER_IOCTL
        .word   SER_IRQ

.bss

.data

; FIXME: these are likely calculated for NTSC?
strt24:  .word $01cb     ; 459   start bit times
strt12:  .word $0442     ; 1090
strt03:  .word $1333     ; 4915

full24:  .word $01a5     ; 421   full bit times
full12:  .word $034d     ; 845
full03:  .word $0d52     ; 3410

.rodata

baudrate = 10
databits = 0
stopbit = 0

wire = 1
duplex = 0
parity = 0

serial_config:
        .byte baudrate + databits + stopbit
        .byte wire + duplex + parity

.code

;----------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present.
; Must return an SER_ERR_xx code in a/x.

SER_INSTALL:
        lda     #<SER_ERR_OK
        tax
        rts

;----------------------------------------------------------------------------
; UNINSTALL routine. Is called before the driver is removed from memory.
; Must return an SER_ERR_xx code in a/x.

SER_UNINSTALL:
        lda     #<SER_ERR_OK
        tax
        rts

;----------------------------------------------------------------------------
; PARAMS routine. A pointer to a ser_params structure is passed in ptr1.
; Must return an SER_ERR_xx code in a/x.

SER_OPEN:
        lda #2
        ldx #<serial_config
        ldy #>serial_config
        jsr SETNAM

        lda #2
        tax
        tay
        jsr SETLFS
        jsr OPEN

        jsr setup

        lda #<SER_ERR_OK
        tax
        rts

;----------------------------------------------------------------------------
; CLOSE: Close the port, disable interrupts and flush the buffer. Called
; without parameters. Must return an error code in a/x.
;
; We can omit close now, save for cleaning up ;-)
SER_CLOSE:
        lda     #<SER_ERR_OK
        tax
        rts

;----------------------------------------------------------------------------
; GET: Will fetch a character from the receive buffer and store it into the
; variable pointer to by ptr1. If no data is available, SER_ERR_NO_DATA is
; return.
;

SER_GET:
        ldx #2
        jsr CHKIN
        jsr rshavedata
        beq SER_GET_NO_DATA
        jsr SER_OURBASIN
        ldx #$00
        sta (ptr1,x)
        jsr CLRCH
        lda #<SER_ERR_OK
        tax
        rts

SER_GET_NO_DATA:
        lda #<SER_ERR_NO_DATA
        ldx #>SER_ERR_NO_DATA
        rts

;----------------------------------------------------------------------------
; OURBASIN: This is a minimised call to get the character from the buffer.
; The Kernal code does not allow zero bytes (0x00)... this does.
;

SER_OURBASIN:
        jsr $F14E
        bcc @exit
        jmp $F1B4
@exit:
        clc
        rts

;----------------------------------------------------------------------------
; PUT: Output character in A.
; Must return an error code in a/x.
;

SER_PUT:
        pha
        ldx #2
        jsr CHKOUT
        pla
        jsr BSOUT
        jsr CLRCH
        lda #<SER_ERR_OK
        tax
        rts

;----------------------------------------------------------------------------
; STATUS: Return the status in the variable pointed to by ptr1.
; Must return an error code in a/x.
;

SER_STATUS:
        lda     #<SER_ERR_OK
        tax
        rts

;----------------------------------------------------------------------------
; IOCTL: Driver defined entry point. The wrapper will pass a pointer to ioctl
; specific data in ptr1, and the ioctl code in A.
; Must return an error code in a/x.
;
; IOCTL will so the enable / disbale part
SER_IOCTL:
        cmp #0
        beq @disable
@enable:
        jsr inable
        sec
        bcs @exit
@disable:
        jsr disabl
@exit:
        lda #<SER_ERR_OK
        tax
        rts

;----------------------------------------------------------------------------
; IRQ: Not used on the C64
;

SER_IRQ     = $0000

;--------------------------------------
setup:
        lda #<nmi64
        ldy #>nmi64
        sta $0318
        sty $0319
        lda #<nchkin
        ldy #>nchkin
        sta $031e
        sty $031f
        lda #<nbsout
        ldy #>nbsout
        sta $0326
        sty $0327
        rts
;--------------------------------------

;        .segment "LOWCODE"  ; FIXME: put into low memory, which never gets banked out
nmi64:
        pha             ; new nmi handler
        txa
        pha
        tya
        pha
nmi128:
        cld
        ldx $dd07       ; sample timer b hi byte
        lda #$7f        ; disable cia nmi's
        sta $dd0d
        lda $dd0d       ; read/clear flags
        bpl notcia      ; (restore key)
        cpx $dd07       ; tb timeout since timer b sampled?
        ldy $dd01       ; (sample pin c)
        bcs mask        ; no
        ora #$02        ; yes, set flag in acc.
        ora $dd0d       ; read/clear flags again
mask:
        and RS232ENABLES       ; mask out non-enabled
        tax             ; these must be serviced
        lsr             ; timer a? (bit 0)
        bcc ckflag      ; no
        lda $dd00       ; yes, put but on pin m
        and #$fb
        ora $b5
        sta $dd00
ckflag:
        txa
        and #$10        ; *flag nmi (bit 4)
        beq nmion       ; no
strtlo:
        lda #$42        ; yes, start-bit to tb
        sta $dd06
strthi:
        lda #$04
        sta $dd07
        lda #$11        ; start tb counting
        sta $dd0f
        lda #$12        ; *flag nmi off, tb on
        eor RS232ENABLES       ; update mask
        sta RS232ENABLES
        sta $dd0d       ; enable new config
fulllo:
        lda #$4d        ; change reload latch
        sta $dd06       ;   to full-bit time
fullhi:
        lda #$03
        sta $dd07
        lda #$08        ; # of bits to receive
        sta $a8
        bne chktxd      ; branch always
notcia:
        ldy #$00
        jmp rstkey      ; or jmp norest
nmion:
        lda RS232ENABLES       ; re-enable nmi's
        sta $dd0d
        txa
        and #$02        ; timer b? (bit 1)
        beq chktxd      ; no
        tya             ; yes, get sample of pin c
        lsr
        ror $aa         ; rs232 is lsb first
        dec $a8         ; byte finished?
        bne txd         ; no
        ldy RS232INPUTIDX       ; yes, byte to buffer
        lda $aa
        sta (ribuf),y   ; (no overrun test)
        inc RS232INPUTIDX
        lda #$00        ; stop timer b
        sta $dd0f
        lda #$12        ; tb nmi off, *flag on
switch:
        ldy #$7f        ; disable nmi's
        sty $dd0d       ; twice
        sty $dd0d
        eor RS232ENABLES       ; update mask
        sta RS232ENABLES
        sta $dd0d       ; enable new config
txd:
        txa
        lsr             ; timer a?
chktxd:
        bcc exit        ; no
        dec $b4         ; yes, byte finished?
        bmi char        ; yes
        lda #$04        ; no, prep next bit
        ror $b6         ; (fill with stop bits)
        bcs store
low:
        lda #$00
store:
        sta $b5
exit:
        jmp return      ; restore regs, rti
char:
        ldy RS232OUTPUTBUFFER
        cpy RS232OUTPUTIDX       ; buffer empty?
        beq txoff       ; yes
getbuf:
        lda (robuf),y   ; no, prep next byte
        inc RS232OUTPUTBUFFER
        sta $b6
        lda #$09        ; # bits to send
        sta $b4
        bne low         ; always - do start bit
txoff:
        ldx #$00        ; stop timer a
        stx $dd0e
        lda #$01        ; disable ta nmi
        bne switch      ; always
;--------------------------------------
disabl:
        pha             ; turns off modem port
test:
        lda RS232ENABLES
        and #$03        ; any current activity?
        bne test        ; yes, test again
        lda #$10        ; no, disable *flag nmi
        sta $dd0d
        lda #$02
        and RS232ENABLES       ; currently receiving?
        bne test        ; yes, start over
        sta RS232ENABLES       ; all off, update mask
        pla
        rts
;--------------------------------------
nbsout:
        pha             ; new bsout
        lda $9a
        cmp #02
        bne notmod
        pla
rsout:
        sta $9e         ; output to modem
        sty $97
point:
        ldy RS232OUTPUTIDX
        sta (robuf),y   ; not official till pointer bumped
        iny
        cpy RS232OUTPUTBUFFER       ; buffer full?
        beq fulbuf      ; yes
        sty RS232OUTPUTIDX       ; no, bump pointer
strtup:
        lda RS232ENABLES
        and #$01        ; transmitting now?
        bne ret3        ; yes
        sta $b5         ; no, prep start bit,
        lda #$09
        sta $b4         ;   # bits to send,
        ldy RS232OUTPUTBUFFER
        lda (robuf),y
        sta $b6         ;   and next byte
        inc RS232OUTPUTBUFFER
        lda RS232BAUD      ; full tx bit time to ta
        sta $dd04
        lda RS232BAUD+1
        sta $dd05
        lda #$11        ; start timer a
        sta $dd0e
        lda #$81        ; enable ta nmi
change:
        sta $dd0d       ; nmi clears flag if set
        php             ; save irq status
        sei             ; disable irq's
        ldy #$7f        ; disable nmi's
        sty $dd0d       ; twice
        sty $dd0d
        ora RS232ENABLES       ; update mask
        sta RS232ENABLES
        sta $dd0d       ; enable new config
        plp             ; restore irq status
ret3:
        clc
        ldy $97
        lda $9e
        rts
fulbuf:
        jsr strtup
        jmp point
notmod:
        pla             ; back to old bsout
        jmp oldout
;--------------------------------------
nchkin:
        jsr findfn      ; new chkin
        bne nosuch
        jsr devnum
        lda $ba
        cmp #$02
        bne back
        sta $99
inable:
        sta $9e         ; enable rs232 input
        sty $97
baud:
        lda RS232BAUD+1    ; set receive to same
        and #$06        ;   baud rate as xmit
        tay
        lda strt24,y
        sta strtlo+1    ; overwrite values in nmi handler
        lda strt24+1,y
        sta strthi+1
        lda full24,y
        sta fulllo+1
        lda full24+1,y
        sta fullhi+1
        lda RS232ENABLES
        and #$12        ; *flag or tb on?
        bne ret1        ; yes
        sta $dd0f       ; no, stop tb
        lda #$90        ; turn on flag nmi
        jmp change
nosuch:
        jmp nofile
back:
        lda $ba
        jmp oldchk
;--------------------------------------
rsget:
        sta $9e         ; input from modem
        sty $9f
        ldy RS232INPUTBUFFER
        cpy RS232INPUTIDX       ; buffer empty?
        beq ret2        ; yes
        lda (ribuf),y   ; no, fetch character
        sta $9e
        inc RS232INPUTBUFFER
ret1:
        clc             ; cc = char in acc.
ret2:
        ldy $9f
        lda $9e
last:
        rts             ; cs = buffer was empty

;----------------------------------------
; A = 0 when no data
; A = 1 when data
rshavedata:
        lda #0
        ldy RS232INPUTBUFFER
        cpy RS232INPUTIDX       ; buffer empty?
        beq rsempty     ; no
        lda #1
rsempty:
        rts
