;
; Ullrich von Bassewitz, 16.09.2001
;
; char cgetc (void);
;

        .export         _cgetc
        .condes         k_blncur, 2
        .import         cursor

        .include        "cbm510.inc"
        .include        "extzp.inc"


; ------------------------------------------------------------------------

.proc   _cgetc

        lda     keyidx                  ; Characters waiting?
        bne     L3                      ; Jump if so

; Switch on the cursor if needed

        lda     CURS_FLAG
        pha
        lda     cursor
        jsr     setcursor
L1:     lda     keyidx
        beq     L1
        ldx     #0
        pla
        bne     L2
        inx
L2:     txa
        jsr     setcursor

; Read the character from the keyboard buffer

L3:     ldx     #$00            ; Get index
        ldy     keybuf          ; Get first character in the buffer
        sei
L4:     lda     keybuf+1,x      ; Move up the remaining chars
        sta     keybuf,x
        inx
        cpx     keyidx
        bne     L4
        dec     keyidx
        cli

        ldx     #$00            ; High byte
        tya                     ; First char from buffer
        rts

.endproc

; ------------------------------------------------------------------------
;

.proc   setcursor

        ldy     #$00                    ;
        tax                             ; On or off?
        bne     @L9                     ; Go set it on
        lda     CURS_FLAG               ; Is the cursor currently off?
        bne     @L8                     ; Jump if yes
        lda     #1
        sta     CURS_FLAG               ; Mark it as off
        lda     CURS_STATE              ; Cursor currently displayed?
        sty     CURS_STATE              ; Cursor will be cleared later
        beq     @L8                     ; Jump if no

; Switch to the system bank, load Y with the cursor X coordinate

        lda     #$0F
        sta     IndReg                  ; Access system bank
        ldy     CURS_X

; Reset the current cursor

        lda     CURS_COLOR
        sta     (CRAM_PTR),y            ; Store cursor color
        lda     ExecReg
        sta     IndReg                  ; Switch to our segment
        lda     (SCREEN_PTR),y
        eor     #$80                    ; Toggle reverse flag
        sta     (SCREEN_PTR),y

; Done

@L8:    rts

@L9:    sty     CURS_FLAG               ; Cursor on (Y = 0)
        rts

.endproc


; ------------------------------------------------------------------------
; Blink the cursor in the interrupt. A blinking cursor is only available if
; we use the cgetc() function, so we will export this IRQ handler only in
; case the module is included into a program.


.proc   k_blncur

        lda     CURS_FLAG               ; Is the cursor on?
        bne     curend                  ; Jump if not
        dec     CURS_BLINK
        bne     curend

; Re-initialize the blink counter

        lda     #20                     ; Initial value
        sta     CURS_BLINK

; Load Y with the cursor X coordinate

        ldy     CURS_X

; Check if the cursor state was on or off before

        lda     CURS_COLOR              ; Load color behind cursor
        lsr     CURS_STATE              ; Cursor currently displayed?
        bcs     curset                  ; Jump if yes

; Cursor was off before, switch it on

        inc     CURS_STATE              ; Mark as displayed
        lda     (CRAM_PTR),y            ; Get color behind cursor...
        sta     CURS_COLOR              ; ...and remember it
        lda     CHARCOLOR               ; Use character color

; Set the cursor with color in A

curset: sta     (CRAM_PTR),y            ; Store cursor color
        lda     ExecReg
        sta     IndReg                  ; Switch to our segment
        lda     (SCREEN_PTR),y
        eor     #$80                    ; Toggle reverse flag
        sta     (SCREEN_PTR),y

; Switch back to the system bank

        lda     #$0F
        sta     IndReg

curend: rts

.endproc



