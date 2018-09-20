;
; Freddy Offenga & Christian Groessler, December 2004
;
; function to get default device: char *_getdefdev(void);
;
; AtariDOS/MyDOS:
; Default device number is derived from DUNIT. Therefore "default
; device" is the one the program was loaded from.
;
; SpartaDOS/RealDOS:
; the ZCRNAME routine is only used to get the default drive because
; ZCRNAME has two disadvantages:
; 1. It will convert D: into D1: instead of Dn: (n = default drive)
; 2. It will give a 'no arguments' status if it detects something
;    like Dn: (without filename).
;
; OS/A+ DOS:
; ZCRNAME is slightly different from SpartaDOS. It will convert D:
; into Dn: where n is the default drive.

        .include        "atari.inc"
        .import         __dos_type
        .export         __getdefdev             ; get default device
        .export         __defdev                ; this is the default device string (e.g. "D1:")
.ifdef  DYNAMIC_DD
        .constructor    __getdefdev, 24
.endif

; Get default device (LBUF will be destroyed!!)

__getdefdev:

        lda     __dos_type      ; which DOS?
        cmp     #XDOS
        beq     xdos            ; XDOS detected
;       cmp     #OSADOS+1       ; (redundant: #OSADOS+1 = #XDOS)
        bcs     use_DUNIT       ; neither XDOS, nor OS/A+ or SpartaDOS

        ldy     #BUFOFF
        lda     #0
        sta     (DOSVEC),y      ; reset buffer offset

; Store dummy argument

        ldy     #LBUF
        lda     #'X'
        sta     (DOSVEC),y
        iny
        lda     #ATEOL
        sta     (DOSVEC),y

; One extra store to avoid the buggy sequence from OS/A+ DOS:
; <D><RETURN><:> => drive number = <RETURN>

        iny
        sta     (DOSVEC),y

; Create crunch vector

        ldy     #ZCRNAME+1
        lda     (DOSVEC),y
        sta     crvec+1
        iny
        lda     (DOSVEC),y
        sta     crvec+2

        jsr     crvec

; Get default device

        ldy     #COMFNAM        ;  COMFNAM is always "Dn:"
        lda     (DOSVEC),y
        sta     __defdev
        iny
        lda     (DOSVEC),y
done:   sta     __defdev+1

; Return pointer to default device

finish: lda     #<__defdev
        ldx     #>__defdev
        rts

; On AtariDOS or MyDOS, use the DUNIT variable to setup the default
; device. The default device will then be the one the program was
; loaded from.

use_DUNIT:
        lda     DUNIT
        clc
        adc     #'0'
        bne     done            ; jump always

; XDOS default device retrieval

xdos:

; check XDOS version (we need >= 2.4)

        lda     XGLIN
        cmp     #$4C            ; there needs to be a 'JMP' opcode here
        bne     finish          ; older version, use DEFAULT_DEVICE or D1:
        lda     XVER            ; get BCD encoded version ($24 for 2.4)
        cmp     #$24
        bcc     finish          ; too old, below 2.4

; good XDOS version, get default drive

        lda     #ATEOL
        sta     XLINE           ; simulate empty command line
        ldy     #0
        jsr     XMOVE           ; create an FMS filename (which in this case only contains the drive)
        lda     XFILE+1
        bne     done

        .data

crvec:  jmp     $FFFF           ; target address will be set to crunch vector

; Default device string

__defdev:
.ifdef  DEFAULT_DEVICE
        .byte   'D', '0'+DEFAULT_DEVICE, ':', 0
.else
        .byte   "D1:", 0
.endif
