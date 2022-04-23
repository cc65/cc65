;/*****************************************************************************/
;/*                                                                           */
;/*                              em-kernel.inc                                */
;/*                                                                           */
;/*                           EM kernel interface                             */
;/*                                                                           */
;/*                                                                           */
;/*                                                                           */
;/* (C) 2002-2003 Ullrich von Bassewitz                                       */
;/*               Roemerstrasse 52                                            */
;/*               D-70794 Filderstadt                                         */
;/* EMail:        uz@cc65.org                                                 */
;/*                                                                           */
;/*                                                                           */
;/* This software is provided 'as-is', without any expressed or implied       */
;/* warranty.  In no event will the authors be held liable for any damages    */
;/* arising from the use of this software.                                    */
;/*                                                                           */
;/* Permission is granted to anyone to use this software for any purpose,     */
;/* including commercial applications, and to alter it and redistribute it    */
;/* freely, subject to the following restrictions:                            */
;/*                                                                           */
;/* 1. The origin of this software must not be misrepresented; you must not   */
;/*    claim that you wrote the original software. If you use this software   */
;/*    in a product, an acknowledgment in the product documentation would be  */
;/*    appreciated but is not required.                                       */
;/* 2. Altered source versions must be plainly marked as such, and must not   */
;/*    be misrepresented as being the original software.                      */
;/* 3. This notice may not be removed or altered from any source              */
;/*    distribution.                                                          */
;/*                                                                           */
;/*****************************************************************************/



;------------------------------------------------------------------------------
; The driver header

.struct EMD_HDR
        ID      .byte   3       ; Contains 0x65, 0x6d, 0x64 ("emd")
        VERSION .byte   1       ; Interface version
        LIBREF  .addr           ; Library reference
        JUMPTAB .struct
            INSTALL     .addr   ; INSTALL routine
            UNINSTALL   .addr   ; UNINSTALL routine
            PAGECOUNT   .addr   ; PAGECOUNT routine
            MAP         .addr   ; MAP routine
            USE         .addr   ; USE routine
            MAPCLEAN    .addr   ; MAPCLEAN routine
            COPYFROM    .addr   ; COPYFROM routine
            COPYTO      .addr   ; COPYTO routine
        .endstruct
.endstruct

;------------------------------------------------------------------------------
; The EMD API version, stored in EMD_HDR::VERSION

EMD_API_VERSION         = $02

;------------------------------------------------------------------------------
; The asm equivalent to the C em_copy structure

.struct EM_COPY
        BUF     .addr           ; Memory buffer to copy from or to
        OFFS    .byte           ; Offset into page
        PAGE    .word           ; Starting page to copy from or to
        COUNT   .word           ; Number of bytes to copy
        UNUSED  .byte           ; Make the size 8 bytes
.endstruct

;------------------------------------------------------------------------------
; Variables

        .global _em_drv                         ; Pointer to driver

;------------------------------------------------------------------------------
; Driver entry points

        .global emd_install
        .global emd_uninstall
        .global emd_pagecount
        .global emd_map
        .global emd_use
        .global emd_commit
        .global emd_copyfrom
        .global emd_copyto

;------------------------------------------------------------------------------
; ASM functions

        .global em_clear_ptr

;------------------------------------------------------------------------------
; C callable functions

        .global _em_load_driver
        .global _em_unload
        .global _em_install
        .global _em_uninstall
        .global _em_pagecount
        .global _em_map
        .global _em_use
        .global _em_commit
        .global _em_copyfrom
        .global _em_copyto

