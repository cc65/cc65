;*****************************************************************************/
;*                                                                           */
;*                                   modload.s                               */
;*                                                                           */
;*                    o65 module loader for the cc65 library                 */
;*                                                                           */
;*                                                                           */
;*                                                                           */
;* (C) 2002      Ullrich von Bassewitz                                       */
;*               Wacholderweg 14                                             */
;*               D-70597 Stuttgart                                           */
;* EMail:        uz@musoftware.de                                            */
;*                                                                           */
;*                                                                           */
;* This software is provided 'as-is', without any expressed or implied       */
;* warranty.  In no event will the authors be held liable for any damages    */
;* arising from the use of this software.                                    */
;*                                                                           */
;* Permission is granted to anyone to use this software for any purpose,     */
;* including commercial applications, and to alter it and redistribute it    */
;* freely, subject to the following restrictions:                            */
;*                                                                           */
;* 1. The origin of this software must not be misrepresented; you must not   */
;*    claim that you wrote the original software. If you use this software   */
;*    in a product, an acknowledgment in the product documentation would be  */
;*    appreciated but is not required.                                       */
;* 2. Altered source versions must be plainly marked as such, and must not   */
;*    be misrepresented as being the original software.                      */
;* 3. This notice may not be removed or altered from any source              */
;*    distribution.                                                          */
;*                                                                           */
;*****************************************************************************/



        .include        "o65.inc"
        .include        "modload.inc"

        .import         pushax, pusha0, push0, push1, decax1
        .import         _malloc, _free, _bzero
        .import         __ZP_START__    ; Linker generated
        .importzp       sp, ptr1, tmp1, regbank

        .macpack        generic

;------------------------------------------------------------------------------
; Variables stored in the register bank in the zero page. Placing the variables
; here will protect them when calling other C functions.

Module          = regbank+0             ; Pointer to module memory
Ctrl            = regbank+2             ; Pointer to mod_ctrl structure
TPtr            = regbank+4             ; Pointer to module data for relocation

;------------------------------------------------------------------------------
; Static module data

.bss

; Save areas and error recovery data
Stack:          .byte   0               ; Old stackpointer
RegBankSave:    .res    6               ; Save area for register bank

; The header of the o65 file. Since we don't need the first 8 bytes any
; longer, once we've checked them, we will overlay them with other data to
; save a few bytes.
Header:         .res    O65_HDR_SIZE    ; The o65 header

; Input
InputByte       = Header                ; Byte read from input

; Relocation
RelocVal        = Header + 1            ; Relocation value

.data      
Read:   jmp     $FFFF                   ; Jump to read routine

.rodata
ExpectedHdr:
        .byte   $01, $00                ; non C64 marker
        .byte   $6F, $36, $35           ; Magic ("o65")
        .byte   $00                     ; Version
        .word   O65_CPU_6502|O65_RELOC_BYTE|O65_SIZE_16BIT|O65_FTYPE_EXE|O65_ADDR_SIMPLE|O65_ALIGN_1

ExpectedHdrSize = * - ExpectedHdr


;------------------------------------------------------------------------------
; PushCtrl: Push the address of the control structure onto the C stack.

.code
PushCtrl:
        lda     Ctrl
        ldx     Ctrl+1
        jmp     pushax

;------------------------------------------------------------------------------
; RestoreRegBank: Restore the register bank contents from the save area. Will
;                 destroy A and X (the latter will be zero on return).

.code
RestoreRegBank:
        ldx     #6
@L1:    lda     RegBankSave-1,x
        sta     regbank-1,x
        dex
        bne     @L1
        rts

;------------------------------------------------------------------------------
; GetReloc: Return a relocation value based on the segment in A.
; The routine uses some knowledge about the values to make the code shorter.

.code
GetReloc:
        cmp     #O65_SEGID_TEXT
        bcc     FormatError
        cmp     #O65_SEGID_ZP
        beq     @L1
        bcs     FormatError

; Text, data and bss segment

        lda     Module
        ldx     Module+1                ; Return start address of buffer
        rts

; Zero page relocation

@L1:    lda     #<__ZP_START__
        ldx     #>__ZP_START__
        rts

;------------------------------------------------------------------------------
; FormatError: Bail out with an o65 format error

.code
FormatError:
        lda     #MLOAD_ERR_FMT
;       bne     CleanupAndExit          ; Branch always

;------------------------------------------------------------------------------
; CleanupAndExit: Free any allocated resources, restore the stack and return
;                 to the caller.

.code
CleanupAndExit:

; Restore the stack so we may return to the caller from here

        ldx     Stack
        txs

; Save the error return code

        pha

; Check if we have to free the allocated block

        lda     Module
        ora     Module+1
        beq     @L1                     ; Jump if no memory allocated

        lda     Module
        ldx     Module+1
        jsr     _free                   ; Free the allocated block

; Restore the register bank

@L1:    jsr     RestoreRegBank

; Restore the  error code and return to the caller

        ldx     #$00                    ; Load the high byte
        pla
        rts

;------------------------------------------------------------------------------
; ReadByte: Read one byte with error checking into InputByte and A.
; ReadAndCheckError: Call read with the current C stack and check for errors.

.code
ReadByte:

; C->read (C, &B, 1)

        jsr     PushCtrl
        lda     #<InputByte
        ldx     #>InputByte
        jsr     pushax
        jsr     push1

; This is a second entry point used by the other calls to Read

ReadAndCheckError:
        jsr     Read

; Check the return code and bail out in case of problems

        tax
        beq     @L1                     ; Jump if ok
        lda     #MLOAD_ERR_READ
        bne     CleanupAndExit

; Done

@L1:    lda     InputByte               ; If called ReadByte, load the byte read
Done:   rts

;------------------------------------------------------------------------------
; RelocSeg: Relocate the segment pointed to by a/x

.code
RelocSeg:
        jsr     decax1                  ; Start value is segment-1
        sta     TPtr
        stx     TPtr+1

Loop:   jsr     ReadByte                ; Read byte from relocation table
        beq     Done                    ; Bail out if end of table reached

        cmp     #255                    ; Special offset?
        bne     @L1

; Increment offset by 254 and continue

        lda     TPtr
        add     #254
        sta     TPtr
        bcc     Loop
        inc     TPtr+1
        jmp     Loop

; Increment offset by A

@L1:    add     TPtr
        sta     TPtr
        bcc     @L2
        inc     TPtr+1

; Read the relocation byte, extract the segment id, fetch the corresponding
; relocation value and place it into ptr1

@L2:    jsr     ReadByte
        and     #O65_SEGID_MASK
        jsr     GetReloc
        sta     RelocVal
        stx     RelocVal+1

; Get the relocation byte again, this time extract the relocation type.

        lda     InputByte
        and     #O65_RTYPE_MASK

; Check for and handle the different relocation types.

        cmp     #O65_RTYPE_WORD
        beq     RelocWord
        cmp     #O65_RTYPE_HIGH
        beq     RelocHigh
        cmp     #O65_RTYPE_LOW
        bne     FormatError

; Relocate the low byte

RelocLow:
        ldy     #0
        clc
        lda     RelocVal
        bcc     AddCommon

; Relocate a high byte

RelocHigh:
        jsr     ReadByte                ; Read low byte from relocation table
        ldy     #0
        clc
        adc     RelocVal                ; We just need the carry
AddHigh:
        lda     RelocVal+1
AddCommon:
        adc     (TPtr),y
        sta     (TPtr),y
        jmp     Loop                    ; Done, next entry

; Relocate a word

RelocWord:
        ldy     #0
        clc
        lda     RelocVal
        adc     (TPtr),y
        sta     (TPtr),y
        iny
        bne     AddHigh                 ; Branch always (add high byte)

;------------------------------------------------------------------------------
; mod_load: Load and relocate an o65 module

.code
_mod_load:

; Save the register bank and clear the Module pointer

        pha
        ldy     #6
@L1:    lda     regbank-1,y
        sta     RegBankSave-1,y
        dey
        bne     @L1
        sty     Module
        sty     Module+1
        pla

; Save the passed parameter

        sta     Ctrl
        stx     Ctrl+1

; Save the stack pointer so we can bail out even from subroutines

        tsx
        stx     Stack

; Get the read function pointer from the control structure and place it into
; our call vector

        ldy     #MODCTRL_READ
        lda     (Ctrl),y
        sta     Read+1
        iny
        lda     (Ctrl),y
        sta     Read+2

; Read the o65 header: C->read (C, &H, sizeof (H))

        jsr     PushCtrl
        lda     #<Header
        ldx     #>Header
        jsr     pushax
        lda     #O65_HDR_SIZE
        jsr     pusha0                  ; Always less than 256
        jsr     ReadAndCheckError       ; Bails out in case of errors

; We read the o65 header successfully. Validate it.

        ldy     #ExpectedHdrSize-1
ValidateHeader:
        lda     Header,y
        cmp     ExpectedHdr,y
        bne     HeaderError
        dey
        bpl     ValidateHeader

; Header is ok as far as we can say now. Read all options, check for the
; OS option and ignore all others. The OS option contains a version number
; and the module id as additional data.

        iny                             ; Y = $00
        sty     TPtr+1                  ; Flag for OS option read
Opt:    jsr     ReadByte                ; Read the length byte
        beq     OptDone                 ; Jump if done
        sta     TPtr                    ; Use TPtr as a counter

; An option has a length of at least 2 bytes

        cmp     #2
        bcc     HeaderError             ; Must be 2 bytes total at least

; Check for the OS option

        dec     TPtr
        jsr     ReadByte                ; Get the option type
        cmp     #O65_OPT_OS             ; OS option?
        bne     SkipOpt                 ; No: Skip

        lda     TPtr                    ; Get remaining length+1
        cmp     #5                      ; CC65 has 6 bytes total
        bne     OSError

        jsr     ReadByte                ; Get the operating system
        cmp     #O65_OS_CC65_MODULE
        bne     OSError                 ; Wrong operating system

        jsr     ReadByte                ; Get the version number, expect zero
        bne     OSError                 ; Wrong version

        jsr     ReadByte                ; Get low byte of id
        ldy     #MODCTRL_MODULE_ID
        sta     (Ctrl),y
        jsr     ReadByte
        ldy     #MODCTRL_MODULE_ID+1
        sta     (Ctrl),y

        inc     TPtr+1                  ; Remember that we got the OS

        jmp     Opt

; Skip one option

SkipOpt:
        dec     TPtr
        beq     Opt                     ; Next option
        jsr     ReadByte                ; Skip one byte
        jmp     SkipOpt

; Operating system error

OSError:
        lda     #MLOAD_ERR_OS
        jmp     CleanupAndExit

; Options done, check that we got the OS option

OptDone:
        lda     TPtr+1
        bne     CalcSizes

; Entry point for header errors

HeaderError:
        lda     #MLOAD_ERR_HDR
        jmp     CleanupAndExit

; Skipped all options. Calculate the size of text+data and of text+data+bss
; (the latter is the size of the memory block we need). We will store the
; total module size also into the control structure for evaluation by the
; caller

CalcSizes:
        lda     Header + O65_HDR_TLEN
        add     Header + O65_HDR_DLEN
        sta     TPtr
        lda     Header + O65_HDR_TLEN + 1
        adc     Header + O65_HDR_DLEN + 1
        sta     TPtr+1
        lda     TPtr
        add     Header + O65_HDR_BLEN
        pha                             ; Save low byte of total size
        ldy     #MODCTRL_MODULE_SIZE
        sta     (Ctrl),y
        lda     TPtr+1
        adc     Header + O65_HDR_BLEN + 1
        iny
        sta     (Ctrl),y
        tax
        pla                             ; Restore low byte of total size

; Total memory size is now in a/x. Allocate memory and remember the result,
; both, locally and in the control structure so it the caller can access
; the memory block. After that, check if we got the requested memory.

        jsr     _malloc
        sta     Module
        stx     Module+1

        ldy     #MODCTRL_MODULE
        sta     (Ctrl),y
        txa
        iny
        sta     (Ctrl),y
        ora     Module
        bne     GotMem

; Could not allocate memory

        lda     #MLOAD_ERR_MEM
        jmp     CleanupAndExit

; Control structure is complete now. Clear the bss segment.
; bzero (bss_addr, bss_size)

        lda     Module
        add     TPtr
        pha
        lda     Module+1
        adc     TPtr+1                  ; Module + tlen + dlen
        tax
        pla
        jsr     pushax
        lda     Header + O65_HDR_BLEN
        ldx     Header + O65_HDR_BLEN+1
        jsr     _bzero                  ; bzero (bss, bss_size);

; Load code and data segment into memory. The sum of the sizes of
; code+data segment is still in TPtr.
; C->read (C, C->module, H.tlen + H.dlen)

GotMem: jsr     PushCtrl
        lda     Module
        ldx     Module+1
        jsr     pushax
        lda     TPtr
        ldx     TPtr+1
        jsr     pushax
        jsr     ReadAndCheckError       ; Bails out in case of errors

; We've got the code and data segments in memory. Next section contains
; undefined references which we don't support. So check if the count of
; undefined references is actually zero.

        jsr     ReadByte
        bne     Undef
        jsr     ReadByte
        beq     Reloc
Undef:  jmp     FormatError

; Number of undefined references was zero. Next come the relocation tables
; for code and data segment. Relocate the code segment

Reloc:  lda     Module
        ldx     Module + 1              ; Code segment address
        jsr     RelocSeg

; Relocate the data segment

        lda     Module
        add     Header + O65_HDR_TLEN
        pha
        lda     Module + 1
        adc     Header + O65_HDR_TLEN + 1
        tax
        pla                             ; Data segment address in a/x
        jsr     RelocSeg

; We're done. Restore the register bank and return a success code

        jsr     RestoreRegBank          ; X will be zero on return
        lda     #MLOAD_OK
        rts

