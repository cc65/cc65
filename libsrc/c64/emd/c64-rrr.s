;
; Extended Memory Driver for the Action Replay/Retro Replay RAM
;
; original Version 1.0 by Johannes Braun 2006-08-22 <hannenz@freenet.de>
; ------------------------------------------------------------------------

        .include        "zeropage.inc"

        .include        "em-kernel.inc"
        .include        "em-error.inc"

        .macpack        generic
        .macpack        module

; ------------------------------------------------------------------------

c64_ram = ptr1  ; use some more expressive identifiers...
rr_ram  = ptr2
len     = ptr3
aux     = ptr4
temp    = tmp1

Lo_Mem  = $0100 ; location of Lo_Code (must be below $1000 or above $e000)

RRMODE_OFF          = $02
RRMODE_CART_RAM     = $23

; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _c64_rrr_emd

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
; Data.

.bss
window:     .res 256        ; the memory window (256 bytes)
pagecount:  .res   1        ; Number of available pages

.rodata
dummy:
        .word window    ; a "pseudo"-em_copy_struct, used by em_map/ em_commit
        .byte 0         ; to pass over to COPYTO/COPYFROM
curpage:
        .byte $ff       ; just this byte is changed according to the desired page
        .byte 0
        .word 256

.code

;----------------------------------------------------------------------------------------
;unsigned char __fastcall__ em_install(void *driver);
;returns an error code
;----------------------------------------------------------------------------------------
INSTALL:
        ldx #c2-c1
:       lda c1,x
        sta Lo_Mem,x
        dex
        bpl :-
        ;ldx #$ff
        stx curpage     ; invalidate current page

        ldy #RRMODE_OFF
        sei
        jmp Lo_Mem+8  ; jump to the code below

        ; copied to Lo_Mem
c1:

;detectmodes:
        .byte RRMODE_CART_RAM | $00
        .byte RRMODE_CART_RAM | $08
        .byte RRMODE_CART_RAM | $10
        .byte RRMODE_CART_RAM | $18
        .byte RRMODE_CART_RAM | $80
        .byte RRMODE_CART_RAM | $88
        .byte RRMODE_CART_RAM | $90
        .byte RRMODE_CART_RAM | $98

        ; first save c64 memory
        lda $8888
        pha

        ; tag c64 memory
        lda #$00
        sta $8888

        ldx #$07
:
        ; try accessing rr-ram
        ;lda detectmodes, x
        lda Lo_Mem, x
        sta $de00

        ; tag (hopefully) rr memory
        txa
        ora #$80
        sta $8888

        dex
        bpl :-

        ;ldy #RRMODE_OFF
        sty $de00

        ; now if C64 memory is $80, there is no AR/RR
        ; if C64 memory is $00, there is a AR/RR.

        lda $8888
        beq detectpages

        lda #0
        beq hasnopages

detectpages:
        ; we can now read the highest available bank nr from the highest bank :)

        lda #RRMODE_CART_RAM | $98
        sta $de00

        ldx $8888
        inx
        txa

        ; 8k = 32 pages
        asl
        asl
        asl
        asl
        asl

hasnopages:

        ;ldy #RRMODE_OFF
        sty $de00       ; c64 ram again

        sta pagecount

        ; restore c64 memory
        pla
        sta $8888

        cli

        ldx pagecount
        beq no

        ; no error
        lda #0
        tax
        rts

no:
        lda #4 ; return #4: error code for "device not present"
        rts
c2:

;----------------------------------------------------------------------------------------
;void em_uninstall(void);
;----------------------------------------------------------------------------------------
UNINSTALL:
return_null:
        lda #$00
        tax
        rts

;----------------------------------------------------------------------------------------
;unsigned __fastcall__ em_pagecount(void);
;----------------------------------------------------------------------------------------
PAGECOUNT:
        lda pagecount        ; always return 32kb (128 pages)
        ldx #$00
        rts

;----------------------------------------------------------------------------------------
;void* __fastcall__ em_use(unsigned page);
;----------------------------------------------------------------------------------------
USE:
        cmp #$80            ; valid page?
        bcs return_null     ; no, return NULL pointer
        sta curpage         ; set to current page
return_win:
        lda #<window        ; return pointer to window
        ldx #>window
return: rts

;----------------------------------------------------------------------------------------
;void* __fastcall__ em_map(unsigned page);
;----------------------------------------------------------------------------------------
MAP:
        cmp pagecount
        bcs return_null
        sta curpage
        lda #<dummy        ; load .A/.X with adress of data for COPYFROM-call (which expects the
        ldx #>dummy        ; adress in .A/.X)
        jsr COPYFROM
        bcs return_win     ; function returns pointer to window (returns always with carry set!)

;----------------------------------------------------------------------------------------
;void __fastcall__ em_commit(void);
;----------------------------------------------------------------------------------------
COMMIT:
        lda curpage
        cmp pagecount
        bcs return
        lda #<dummy        ; load .A/.X with adress of data for COPYTO-call (which expects the
        ldx #>dummy        ; adress in .A/.X)

;----------------------------------------------------------------------------------------
;void __fastcall__ em_copyto (struct em_copy *copy_data);
;----------------------------------------------------------------------------------------
COPYTO:
        jsr get_struct_data    ;read the  parameters passed in the em_struct pointed to by .A/.X upon call

        ;copy the main copyto routine into Lo_Mem

        ldy #Lo_Code1_End - Lo_Code1
:       lda Lo_Code1-1,y
        sta Lo_Mem-1,y
        dey
        bne :-
COMMON:
        sei
        jmp Lo_Mem

        ;this part will be executed in Lo_Mem (!) by COPYFROM

Lo_Code2:
        ; copy byte rr -> c64
        stx $de00       ;map in rr-ram
        lda (rr_ram),y  ;get byte from rr-ram
        sty $de00       ;RR-ROM will be mapped to $8000-$a000 but write access will go to c64-ram anyway!!
        sta (c64_ram),y ;and write to c64-ram
        nop             ;pad to same size as Lo_Code1
        nop
Lo_Code2_End:


        ;this part will be executed in Lo_Mem (!) by COPYTO

Lo_Code1:
        ; copy byte c64 -> rr
        lda (c64_ram),y ;read 1 byte from c64-ram
        stx $de00       ;map in rr-ram
        sta (rr_ram),y  ;write byte to rr-ram
        lda #$02        ;map in c64-ram again
        sta $de00
                        ;12 bytes

        ;this part is common for both COPYFROM/COPYTO and executed in Lo_Mem, too

Lo_Code_Common:
        inc c64_ram         ;increase pointers
        bne :+
        inc c64_ram+1
:       inc rr_ram
        bne @skip
        inc rr_ram+1
        lda rr_ram+1
        cmp #$a0            ;wrap around 16k boundary in rr-ram window ($8000-$a000)
        bne @skip

        lda #$80            ;reset pointer to $8000
        sta rr_ram+1
        txa                 ;adjust value in .X to map in next 16k-bank in rr-ram
        adc #7              ;carry is set because of former CMP, so it adds 8
        tax
                    ;27 bytes
@skip:  lda c64_ram
        cmp len
        lda c64_ram+1
        sbc len+1
        bcc Lo_Code1
        lda #2              ;CHANGE to LDA #0 if driver is called from ROM
        sta $de00
        cli
        rts                 ;17 bytes    = 56 bytes Lo_Code ($38)
Lo_Code1_End:
;----------------------------------------------------------------------------------------
;void __fastcall__ em_copyfrom(struct em_copy *copy_data);
;copy from extended memory into linear memory
;----------------------------------------------------------------------------------------
COPYFROM:
        jsr get_struct_data

        ldy #Lo_Code2_End - Lo_Code2    ;copy routine into Lo_Mem
:       lda Lo_Code2-1,y
        sta Lo_Mem-1,y
        dey
        bne :-
        ldy #Lo_Code1_End-Lo_Code_Common
:       lda Lo_Code_Common-1,y
        sta Lo_Mem+11,y
        dey
        bne :-
        beq COMMON          ;and execute...
;----------------------------------------------------------------------------------------
;read the struct data located at (.A/.X)
;and setup parameters for stash/ fetch operation
;----------------------------------------------------------------------------------------
get_struct_data:

        ;read and process the values from the em_copy struct passed to as parameters rameter to the
        ;functions em_copyto and em_copyfrom

        sta aux         ;store adress of struct (passed in .A/.X) into a zp pointer
        stx aux+1
        ldy #0          ;index 0

        lda (aux),y     ;read c64-adress lo
        sta c64_ram
        iny
        lda (aux),y     ;read c64-adress hi
        sta c64_ram+1   ;(c64_ram) --> points to c64-adress space
        iny
        lda (aux),y     ;read rr-adress lo
        sta rr_ram
        iny
        lda (aux),y     ;rr-adress hi
        pha             ;remember
        and #$1f
        ora #$80        ;adjust into 16k-window ($8000-$a000)
        sta rr_ram+1
        pla             ;re-get hi byte of rr-adress
        and #$60        ;isolate bits 5 and 6
        lsr
        lsr             ;shift into bits 3 and 4
        ora #$23        ;set bit 5 (select ram) and 1+2 (game/exrom setting for ULTIMAX-mode)
        tax             ;.X has now the value to write into $de00 to acess rr-ram at desired 16k-bank
        iny
        iny             ;skip unused byte
        lda (aux),y     ;read length lo-byte
        clc
        adc c64_ram     ;add to c64-addres
        sta len
        iny
        lda (aux),y     ;length hi-byte
        adc c64_ram+1
        sta len+1       ;tmp2: length, tmp3 contains end adress of transfer in c64-ram.
        rts
                        ;55 bytes

