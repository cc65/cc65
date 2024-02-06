;*****************************************************************************/
;*                                                                           */
;*                                  o65.inc                                  */
;*                                                                           */
;*                    Definitions for the o65 file format                    */
;*                                                                           */
;*                                                                           */
;*                                                                           */
;* (C) 2002-2009, Ullrich von Bassewitz                                      */
;*                Roemerstrasse 52                                           */
;*                D-70794 Filderstadt                                        */
;* EMail:         uz@cc65.org                                                */
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



; This files exports structures and constants to handle the o65 relocatable
; file format as defined by Andre Fachat.



; The o65 header structure (6502 format)
.struct O65_HDR
        MARKER          .byte   2       ; Non-C64 marker: $01 $00
        MAGIC           .byte   3       ; o65 magic: "o65"
        VERSION         .byte   1       ; Version number
        MODE            .word           ; Mode word
        TBASE           .word           ; Original text (code) segment address
        TLEN            .word           ; Size of text (code) segment
        DBASE           .word           ; Original data segment address
        DLEN            .word           ; Size of data segment
        BBASE           .word           ; Original bss segment address
        BLEN            .word           ; Size of bss segment
        ZBASE           .word           ; Original zp segment address
        ZLEN            .word           ; Size of zp segment
        STACK           .word           ; Stacksize needed
.endstruct

; Marker, magic and version number
O65_MARKER_0            =       $01
O65_MARKER_1            =       $00
O65_MAGIC_0             =       $6F     ; 'o'
O65_MAGIC_1             =       $36     ; '6'
O65_MAGIC_2             =       $35     ; '5'
O65_VERSION             =       $00

; Defines for the mode word
O65_CPU_65816           =       $8000   ; Executable is for 65816
O65_CPU_6502            =       $0000   ; Executable is for the 6502
O65_CPU_MASK            =       $8000   ; Mask to extract CPU type

O65_RELOC_PAGE          =       $4000   ; Page wise relocation
O65_RELOC_BYTE          =       $0000   ; Byte wise relocation
O65_RELOC_MASK          =       $4000   ; Mask to extract relocation type

O65_SIZE_32BIT          =       $2000   ; All size words are 32bit
O65_SIZE_16BIT          =       $0000   ; All size words are 16bit
O65_SIZE_MASK           =       $2000   ; Mask to extract size

O65_FTYPE_OBJ           =       $1000   ; Object file
O65_FTYPE_EXE           =       $0000   ; Executable file
O65_FTYPE_MASK          =       $1000   ; Mask to extract type

O65_ADDR_SIMPLE         =       $0800   ; Simple addressing
O65_ADDR_DEFAULT        =       $0000   ; Default addressing
O65_ADDR_MASK           =       $0800   ; Mask to extract addressing

O65_CHAIN               =       $0400   ; Chained file, another one follows
O65_CHAIN_MASK          =       $0400   ; Mask to extract chain flag

O65_BSSZERO             =       $0200   ; BSS segment must be zeroed
O65_BSSZERO_MASK        =       $0200   ; Mask to extract bss zero flag

; The following is used if O65_CPU == 6502
O65_CPU2_6502           =       $0000   ; Executable is for 6502
O65_CPU2_65C02          =       $0010   ; Executable is for 65C02
O65_CPU2_65SC02         =       $0020   ; Executable is for 65SC02
O65_CPU2_65CE02         =       $0030   ; Executable is for 65CE02
O65_CPU2_6502X          =       $0040   ; Executable is for NMOS 6502
O65_CPU2_65816_EMU      =       $0050   ; Executable is for 65816 in emul mode
O65_CPU2_MASK           =       $00F0   ; Mask to extract CPU2 field

O65_ALIGN_1             =       $0000   ; Bytewise alignment
O65_ALIGN_2             =       $0001   ; Align words
O65_ALIGN_4             =       $0002   ; Align longwords
O65_ALIGN_256           =       $0003   ; Align pages (256 bytes)
O65_ALIGN_MASK          =       $0003   ; Mask to extract alignment

; The mode word as generated by the ld65 linker
O65_MODE_CC65           = O65_CPU_6502 | O65_RELOC_BYTE | O65_SIZE_16BIT | O65_FTYPE_EXE | O65_ADDR_SIMPLE | O65_ALIGN_1

; Relocation type codes
O65_RTYPE_WORD          =       $80
O65_RTYPE_HIGH          =       $40
O65_RTYPE_LOW           =       $20
O65_RTYPE_SEGADDR       =       $C0
O65_RTYPE_SEG           =       $A0
O65_RTYPE_MASK          =       $E0

; Segment IDs
O65_SEGID_UNDEF         =       $00
O65_SEGID_ABS           =       $01
O65_SEGID_TEXT          =       $02
O65_SEGID_DATA          =       $03
O65_SEGID_BSS           =       $04
O65_SEGID_ZP            =       $05
O65_SEGID_MASK          =       $07

; Option tags
O65_OPT_FILENAME        =       0
O65_OPT_OS              =       1
O65_OPT_ASM             =       2
O65_OPT_AUTHOR          =       3
O65_OPT_TIMESTAMP       =       4

; Operating system codes for O65_OPT_OS
O65_OS_OSA65            =       1
O65_OS_LUNIX            =       2
O65_OS_CC65             =       3
O65_OS_OPENCBM          =       4

; Load errors
O65_LOAD_OK             =       0       ; Module load successful
O65_LOAD_ERR_READ       =       1       ; Read error
O65_LOAD_ERR_HDR        =       2       ; Header error
O65_LOAD_ERR_OS         =       3       ; Wrong OS
O65_LOAD_ERR_FMT        =       4       ; Data format error
O65_LOAD_ERR_MEM        =       5       ; Not enough memory



