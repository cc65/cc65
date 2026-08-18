;
; Robert Justice, 07.06.2026
;
; void __fastcall__ cputc (char c);
;

        .constructor    initconio
        .export         _cputc, putcdirect
        .export         consref, consdev, consinvflg
        .export         consvpwidth, consvpheight
        .export         setconioscr, setstdioscr, consscrflg
        .import         cursor

        .include        "apple3.inc"
        .include        "sos.inc"

        .segment        "ONCE"


initconio:

        ; Open .CONSOLE
        brk
        .byte   OPEN_CALL
        .addr   opencon

        lda     openref
        sta     consref
        sta     initconref
        sta     writeref

        ; Init console to 80x24
        brk
        .byte   WRITE_CALL
        .addr   initconw

        ; get console device number
        brk
        .byte   GET_DEV_NUM_CALL
        .addr   getconsdev

        lda     cdev
        sta     consdev
        sta     setecho+1

        ; Set no echo
        lda     #CTRL_SCREEN_ECHO
        sta     setecho+2
        brk
        .byte   D_CONTROL_CALL
        .addr   setecho

        lda     #0
        sta     consinvflg       ; init inverse flag = off
        sta     consscrflg       ; init scroll flag = off

        lda     #1               ; init cursor enabled
        sta     cursor

        rts

; open console param list
opencon:    .byte   4
            .addr   consname
openref:    .byte   0
            .word   0
            .byte   0

consname:
        .byte 8
        .byte ".CONSOLE"

; write console param list
initconw:
        .byte   3
initconref:
        .byte   0
        .addr   initscr
        .word   5
		
initscr: 
        .byte   16            ; set text mode
        .byte   3             ; 80x24
        .byte   28            ; clear viewport
        .byte   21            ; cursor movement control
        .byte   5             ; Scroll off

;get dev num param list
getconsdev:
        .byte   2
        .addr   consname
cdev:   .byte   0

;control param list
setecho:
        .byte   3
        .byte   0             ; dev_num
        .byte   0
        .addr   setechooff    ; ctrl list

setechooff: 
        .byte   01            ; ctrl list length
        .byte   00            ; no echo


        .code

; Plot a character - also used as internal function
;  inline the call & param list to keep the code short
_cputc:
        cmp     #$0A
        beq     :+
        cmp     #$0D
        beq     :+
        ora     #$80
:       bit     consscrflg    ; check if wrap is off
        beq     putcdirect
        pha
        jsr     setconioscr
        pla
putcdirect:
        sta     charbuf
        brk
        .byte   WRITE_CALL
        .addr   writecon
        rts

; Set the console scroll off for conio
setconioscr:
        inc     consscrflg
        lda     #CONSOLE_CURSOR_MCTL
        jsr     putcdirect
        lda     #$05         ; Scroll off
        jsr     putcdirect
        rts

; Set the console wrap on for stdio
setstdioscr:
        dec     consscrflg
        lda     #CONSOLE_CURSOR_MCTL
        jsr     putcdirect
        lda     #$0D         ; Scroll on
        jsr     putcdirect
        rts


        .data

; write console param list
writecon: .byte   3
writeref: .byte   0
          .addr   charbuf
          .word   1

charbuf:  .byte   00


; Console ref_num and dev_num
consref:
        .byte   0
consdev:
        .byte   0
consinvflg:
        .byte   0
consscrflg:                   ; wrap/scroll flag on/off for conio/stdio
        .byte   0
consvpwidth:
        .byte   80
consvpheight:
        .byte   24
