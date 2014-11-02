;
; Extended memory driver for the Atari 130XE memory standard
; Shawn Jefferson, 2012-08-11
;
; Banking Register $D301:
; Bit 7: Self-Test on/off
; Bit 6: Unused
; Bit 5: Antic sees bank
; Bit 4: CPU sees bank
; Bit 3: bank control
; Bit 2: bank control
; Bit 1: BASIC on/off
; Bit 0: OS RAM on/off
; 
; Masks: %11100011  $E3     Bank 0
;        %11100111  $E7     Bank 1
;        %11101011  $EB     Bank 2
;        %11101111  $EF     Bank 3
;        %11111111  $FF     Off
;
; Based on the Extended memory driver for the Apple II auxiliary memory
;  by
; Stefan Haubenthal, 2003-12-12
; Ullrich von Bassewitz, 2002-12-02
;

        .include        "zeropage.inc"

        .include        "em-kernel.inc"
        .include        "em-error.inc"


        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

.ifdef __ATARIXL__
        module_header   _atrx130_emd
.else
        module_header   _atr130_emd
.endif

; Driver signature

        .byte   $65, $6d, $64           ; "emd"
        .byte   EMD_API_VERSION         ; EM API version number

; Library reference

        .addr   $0000

; Jump table

        .addr   INSTALL
        .addr   UNINSTALL
        .addr   PAGECOUNT
        .addr   MAP
        .addr   USE
        .addr   COMMIT
        .addr   COPYFROM
        .addr   COPYTO

; ------------------------------------------------------------------------
; Constants

BANK    = $4000                         ; bank window
STACK   = $0100                         ; stack location 
PAGES   = 256                           ; 4 x 16k banks


; ------------------------------------------------------------------------
; Data.

.rodata
banks:          .byte $E3,$E7,$EB,$EF   ; 130XE banks for cpu banking

stacktest:      sei
                ldy banks
                sty $D301
                tax                     ; A is set by caller
                inx
                stx $4000               ; change $4000
                ldy #$FF                ; STACK+13
                sty $D301
                cmp $4000               ; changed?
                beq @1
                sec                     ; error
                bcs @2
@1:             clc
@2:             sta $4000               ; restore
                cli
                rts
stacktest_end: 

stackcopy:      sei                     ; disable interrupts
@1:             dex                     ; pre-decrement (full page x=0)
                ldy #$FF                ; this will be replaced  STACK+3
                sty $D301               ; set bank 
                lda $FF00,x             ; address to copy from   STACK+8,+9
                ldy #$FF                ; this will be replaced  STACK+11
                sty $D301 
                sta $FF00,x             ; address to copy to     STACK+16,+17
                cpx #0 
                bne @1
                ldy #$FF                ; portb_save             STACK+23
                sty $D301
                cli                     ; enable interrupts
                rts
stackcopy_end:

stackcopy_byte: sei
                ldy #$FF                ; STACK+2
                sty $D301
                lda $FFFF               ; STACK+7 +8
                ldy #$FF                ; STACK+10
                sty $D301
                sta $FFFF               ; STACK+15 +16
                ldy #$FF                ; STACK+18
                sty $D301
                cli
                rts
stackcopy_byte_end: 


.data
curpage:        .byte   $FF             ; Current page number in bank (invalid)
curbank:        .byte   $FF             ; Current bank number

.bss
window:         .res    256             ; Memory "window"
portb_save:     .res    1               ; portb state

.code

install_transfer:
                ldx #stackcopy_end - stackcopy - 1
@1:             lda stackcopy,x
                sta STACK,x
                dex
                bpl @1
                rts

install_byte_transfer:
                ldx #stackcopy_byte_end - stackcopy_byte - 1
@2:             lda stackcopy_byte,x
                sta STACK,x
                dex
                bpl @2
                rts

install_test:
                ldx #stacktest_end - stacktest - 1
@3:             lda stacktest,x
                sta STACK,x
                dex
                bpl @3
                rts

setpage:
                tax                             ; save page
                and     #$C0                    ; mask out bank
                clc
                ror
                ror                             ; divide by 64
                ror                             ; 64 pages in each bank
                ror
                ror
                ror
                sta     curbank                 ; Remember the new bank
                txa                             ; bring back page
                and     #$3F                    ; mask out page
                sta     curpage                 ; curpage in bank
                rts


; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an EM_ERR_xx code in a/x.
;

INSTALL:
        lda     $D301                   ; save state of portb
        sta     portb_save
        tay 

        jsr     install_test            ; doesn't touch Y
        sty     STACK+13

        lda     $4000                   ; test for extended memory
        jsr     STACK
        bcs     @1 
        lda     #EM_ERR_OK
        rts
@1:     lda     #EM_ERR_NO_DEVICE
        rts

; ------------------------------------------------------------------------
; UNINSTALL routine. Is called before the driver is removed from memory.
; Can do cleanup or whatever. Must not return anything.
;

UNINSTALL:
        rts


; ------------------------------------------------------------------------
; PAGECOUNT: Return the total number of available pages in a/x.
;

PAGECOUNT:
        lda     #<PAGES
        ldx     #>PAGES
        rts

; ------------------------------------------------------------------------
; MAP: Map the page in a/x into memory and return a pointer to the page in
; a/x. The contents of the currently mapped page (if any) may be discarded
; by the driver.
;

MAP:    jsr setpage                     ; extract the bank/page
        add     #>BANK                  ; $4000 + page  (carry is cleared)
        sta     ptr1+1
        ;ldy     #$00
        ;sty     ptr1

        lda     #<window
        sta     ptr2
        lda     #>window
        sta     ptr2+1

; Transfer one page

        jsr     install_transfer        ; Transfer one page

        ldx     curbank
        lda     banks,x
        sta     STACK+3                 ; set bank to copy from
;        lda     ptr1
;        sta     STACK+8 
        lda     ptr1+1
        sta     STACK+9                 ; set copy from address
        lda     portb_save
        sta     STACK+11                ; set portb restore
        sta     STACK+23                ; set final portb restore
        lda     ptr2
        sta     STACK+16
        lda     ptr2+1
        sta     STACK+17                ; set copy to address 

        ldx     #0                      ; full page copy
        jsr     STACK                   ; do the copy! 

; Return the memory window

        lda     #<window
        ldx     #>window                ; Return the window address

        rts

; ------------------------------------------------------------------------
; USE: Tell the driver that the window is now associated with a given page.

USE:    ;sta     curpage                 ; Remember the page
        jsr setpage                     ; extract bank/page
        lda     #<window
        ldx     #>window                ; Return the window
        rts

; ------------------------------------------------------------------------
; COMMIT: Commit changes in the memory window to extended storage.

COMMIT: lda     curpage                 ; Get the current page
        cmp     #$FF
        beq     commit_done             ; Jump if no page mapped

        clc
        add     #>BANK
        sta     ptr2+1
        ;ldy     #$00
        ;sty     ptr2

        lda     #<window
        sta     ptr1
        lda     #>window
        sta     ptr1+1

; Transfer one page/all bytes

        jsr     install_transfer        ; Transfer one page

        lda     portb_save
        sta     STACK+3                 ; set bank to copy from
        sta     STACK+23                ; set final portb restore
        lda     ptr1
        sta     STACK+8 
        lda     ptr1+1
        sta     STACK+9                 ; set copy from address
        ldx     curbank
        lda     banks,x
        sta     STACK+11                ; set bank to copy to
        ;lda     ptr2
        ;sta     STACK+16
        lda     ptr2+1
        sta     STACK+17                ; set copy to address 

        ldx     #0                      ; full page copy
        jsr     STACK                   ; do the copy! 

commit_done:
        rts


; ------------------------------------------------------------------------
; COPYFROM: Copy from extended into linear memory. A pointer to a structure
; describing the request is passed in a/x.
; The function must not return anything.
;
COPYFROM:
        sta     ptr3
        stx     ptr3+1                  ; Save the passed em_copy pointer

        jsr     install_byte_transfer   ; install the stack copy routine

        ldy     #EM_COPY::OFFS
        lda     (ptr3),y
        sta     STACK+7                 ; offset goes into BANK low 

        ldy     #EM_COPY::PAGE
        lda     (ptr3),y
        sta     tmp1                    ; save page for later
        ;add     #>BANK
        ;sta     STACK+8                 ; BANK + page goes into BANK high

        ldy     #EM_COPY::BUF
        lda     (ptr3),y
        sta     STACK+15                ; buffer goes into dest low
        iny
        lda     (ptr3),y
        sta     STACK+16                ; buffer goes into dest high

        ldy     #EM_COPY::COUNT
        lda     (ptr3),y                ; Get bytes in last page
        sta     ptr4
        iny
        lda     (ptr3),y                ; Get number of pages
        sta     ptr4+1

        lda     tmp1                    ; extract bank/page
        jsr     setpage                 ; sets curbank/curpage
        lda     curpage
        add     #>BANK                  ; add to BANK address
        sta     STACK+8                 ; current page in bank
        ldx     curbank
        lda     banks,x 
        sta     STACK+2                 ; set bank in stack 
        lda     portb_save 
        sta     STACK+10                ; set bank restore in stack
        sta     STACK+18                ; set final restore too

copyfrom_copy:
        lda     ptr4                    ; check if count is zero
        bne     @4
        lda     ptr4+1
        beq     done

@4:     jsr     STACK                   ; copy one byte

        sec
        lda     ptr4
        sub     #1
        sta     ptr4
        bcs     @1
        lda     ptr4+1
        beq     @1
        sub     #1
        sta     ptr4+1

@1:     inc     STACK+7                 ; increment address in BANK
        bne     @2
        inc     STACK+8
        lda     STACK+8
        cmp     #$80                    ; we stepped outside bank
        bne     @2

        inc     curbank                 ; get next bank
        ldx     curbank
        lda     banks,x
        sta     STACK+2                 ; set new bank
        lda     #$40                    ; set address back to $4000
        sta     STACK+8

@2:     inc     STACK+15                ; increment buffer address
        bne     @3
        inc     STACK+16

@3:     jmp     copyfrom_copy           ; copy another byte 

done:
        rts

; ------------------------------------------------------------------------
; COPYTO: Copy from linear into extended memory. A pointer to a structure
; describing the request is passed in a/x.
; The function must not return anything.
;

COPYTO:
        sta     ptr3
        stx     ptr3+1                  ; Save the passed em_copy pointer

        jsr     install_byte_transfer   ; install the stack copy routine

        ldy     #EM_COPY::OFFS
        lda     (ptr3),y
        sta     STACK+15                 ; offset goes into BANK low 

        ldy     #EM_COPY::PAGE
        lda     (ptr3),y
        sta     tmp1                    ; save page for later
        ;add     #>BANK
        ;sta     STACK+16                ; BANK + page goes into BANK high

        ldy     #EM_COPY::BUF
        lda     (ptr3),y
        sta     STACK+7                ; buffer goes into dest low
        iny
        lda     (ptr3),y
        sta     STACK+8                ; buffer goes into dest high

        ldy     #EM_COPY::COUNT
        lda     (ptr3),y                ; Get bytes in last page
        sta     ptr4
        iny
        lda     (ptr3),y                ; Get number of pages
        sta     ptr4+1

        lda     tmp1                    ; extract bank/page
        jsr     setpage                 ; sets curbank/curpage
        lda     curpage
        add     #>BANK                  ; add to BANK address
        sta     STACK+16                ; current page in bank
        ldx     curbank
        lda     banks,x 
        sta     STACK+10                ; set bank in stack 
        lda     portb_save 
        sta     STACK+2                 ; set bank restore in stack
        sta     STACK+18                ; set final restore too

copyto_copy:
        lda     ptr4                    ; check if count is zero
        bne     @4
        lda     ptr4+1
        beq     done

@4:     jsr     STACK                   ; copy one byte

        sec
        lda     ptr4
        sub     #1
        sta     ptr4
        bcs     @1
        lda     ptr4+1
        beq     @1
        sub     #1
        sta     ptr4+1

@1:     inc     STACK+15               ; increment address in BANK
        bne     @2
        inc     STACK+16
        lda     STACK+16
        cmp     #$80                    ; we stepped outside bank
        bne     @2

        inc     curbank                 ; get next bank
        ldx     curbank
        lda     banks,x
        sta     STACK+10                ; set new bank
        lda     #$40                    ; set address back to $4000
        sta     STACK+16

@2:     inc     STACK+7                ; increment buffer address
        bne     @3
        inc     STACK+8

@3:     jmp     copyto_copy           ; copy another byte 

