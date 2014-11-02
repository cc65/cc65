;
; Ullrich von Bassewitz, 2002-11-29
;
; Common functions of the extended memory API.
;

        .import         return0, em_libref
        .importzp       ptr1

        .include        "em-kernel.inc"
        .include        "em-error.inc"


;----------------------------------------------------------------------------
; Variables


.bss
_em_drv:        .res    2               ; Pointer to driver

; Jump table for the driver functions.
.data
emd_vectors:
emd_install:    jmp     return0
emd_uninstall:  jmp     return0
emd_pagecount:  jmp     return0
emd_map:        jmp     return0
emd_use:        jmp     return0
emd_commit:     jmp     return0
emd_copyfrom:   jmp     return0
emd_copyto:     jmp     return0

; Driver header signature
.rodata
emd_sig:        .byte   $65, $6d, $64, EMD_API_VERSION  ; "emd", version


;----------------------------------------------------------------------------
; unsigned char __fastcall__ em_install (void* driver);
; /* Install the driver once it is loaded */


_em_install:
        sta     _em_drv
        sta     ptr1
        stx     _em_drv+1
        stx     ptr1+1

; Check the driver signature

        ldy     #.sizeof(emd_sig)-1
@L0:    lda     (ptr1),y
        cmp     emd_sig,y
        bne     inv_drv
        dey
        bpl     @L0

; Set the library reference

        ldy     #EMD_HDR::LIBREF
        lda     #<em_libref
        sta     (ptr1),y
        iny
        lda     #>em_libref
        sta     (ptr1),y

; Copy the jump vectors

        ldy     #EMD_HDR::JUMPTAB
        ldx     #0
@L1:    inx                             ; Skip the JMP opcode
        jsr     copy                    ; Copy one byte
        jsr     copy                    ; Copy one byte
        cpy     #(EMD_HDR::JUMPTAB + .sizeof(EMD_HDR::JUMPTAB))
        bne     @L1

        jmp     emd_install             ; Call driver install routine

; Driver signature invalid

inv_drv:
        lda     #EM_ERR_INV_DRIVER
        ldx     #0
        rts

; Copy one byte from the jump vectors

copy:   lda     (ptr1),y
        sta     emd_vectors,x
        iny
        inx
        rts

;----------------------------------------------------------------------------
; unsigned char em_uninstall (void);
; /* Uninstall the currently loaded driver and return an error code.
; ** Note: This call does not free allocated memory.
; */

_em_uninstall:
        jsr     emd_uninstall           ; Call driver routine

em_clear_ptr:                           ; External entry point
        lda     #0
        sta     _em_drv
        sta     _em_drv+1               ; Clear the driver pointer

        tax
        rts                             ; Return zero
