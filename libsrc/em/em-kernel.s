;
; Ullrich von Bassewitz, 2002-11-29
;
; Common functions of the extended memory API.
;

        .export         _em_install, _em_deinstall

        .import         return0
        .importzp       ptr1

        .include        "em-kernel.inc"
        .include        "em-error.inc"


;----------------------------------------------------------------------------
; Variables


.bss
_em_drv:        .res    2      		; Pointer to driver

; Jump table for the driver functions.
.data
emd_vectors:
emd_install:   	jmp     $0000
emd_deinstall: 	jmp     $0000
emd_pagecount:  jmp     $0000
emd_map:        jmp     $0000
emd_commit:     jmp     $0000
emd_copyfrom:	jmp     $0000
emd_copyto:     jmp     $0000

; Driver header signature
.rodata
emd_sig:        .byte   $65, $6d, $64, $00      ; "emd", version
emd_sig_len     = * - emd_sig


;----------------------------------------------------------------------------
; unsigned char __fastcall__ em_install (void* driver);
; /* Install the driver once it is loaded */


_em_install:
       	sta     _em_drv
  	sta	ptr1
  	stx     _em_drv+1
  	stx    	ptr1+1

; Check the driver signature

        ldy     #emd_sig_len-1
@L0:    lda     (ptr1),y
        cmp     emd_sig,y
        bne     inv_drv
        dey
        bpl     @L0

; Copy the jump vectors

        ldy     #EMD_HDR_JUMPTAB
        ldx     #0
@L1:    inx                             ; Skip the JMP opcode
        jsr     copy                    ; Copy one byte
        jsr     copy                    ; Copy one byte
        cpx     #(EMD_HDR_JUMPCOUNT*3)
        bne     @L1

        jmp     emd_install             ; Call driver install routine

; Driver signature invalid

inv_drv:
        lda     #EM_ERR_INV_DRIVER
        ldx     #0
        rts

; Copy one byte from the jump vectors

copy:   lda     (ptr1),y
        iny
set:    sta     emd_vectors,x
        inx
        rts

;----------------------------------------------------------------------------
; void __fastcall__ em_deinstall (void);
; /* Deinstall the driver before unloading it */

_em_deinstall:
        jsr     emd_deinstall           ; Call driver routine

; Point all jump vectors to return0

        ldx     #0
@L1:    inx                             ; Skip JMP opcode
        lda     #<return0
        jsr     set
        lda     #>return0
        jsr     set
        cpx     #(EMD_HDR_JUMPCOUNT*3)
        bne     @L1

        rts


