;/*****************************************************************************/
;/*                                                                           */
;/*                              em-kernel.inc                                */
;/*                                                                           */
;/*                           EM kernel interface                             */
;/*                                                                           */
;/*                                                                           */
;/*                                                                           */
;/* (C) 2002      Ullrich von Bassewitz                                       */
;/*               Wacholderweg 14                                             */
;/*               D-70597 Stuttgart                                           */
;/* EMail:        uz@musoftware.de                                            */
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
; Driver header stuff

EMD_HDR_ID              = 0                     ; 0x65, 0x6d, 0x64 ("emd")
EMD_HDR_VERSION         = 3                     ; Interface version

EMD_HDR_JUMPTAB         = 4
EMD_HDR_INSTALL         = EMD_HDR_JUMPTAB+0     ; INSTALL routine
EMD_HDR_DEINSTALL       = EMD_HDR_JUMPTAB+2     ; DEINSTALL routine
EMD_HDR_PAGECOUNT       = EMD_HDR_JUMPTAB+4     ; PAGECOUNT routine
EMD_HDR_MAP             = EMD_HDR_JUMPTAB+6     ; MAP routine
EMD_HDR_MAPCLEAN        = EMD_HDR_JUMPTAB+8     ; MAPCLEAN routine
EMD_HDR_COPYFROM        = EMD_HDR_JUMPTAB+10    ; COPYFROM routine
EMD_HDR_COPYTO          = EMD_HDR_JUMPTAB+12    ; COPYTO routine

EMD_HDR_JUMPCOUNT       = 7                     ; Number of jump vectors

;------------------------------------------------------------------------------
; Offsets into the em_copy structure

EM_COPY_PAGE            = 0
EM_COPY_OFFS            = 2
EM_COPY_BUF             = 3
EM_COPY_COUNT           = 5

;------------------------------------------------------------------------------
; Variables

       	.global	_em_drv                         ; Pointer to driver

;------------------------------------------------------------------------------
; Driver entry points

        .global emd_install
        .global emd_deinstall
        .global emd_pagecount
        .global emd_map
        .global emd_mapclean
	.global	emd_copyfrom
        .global emd_copyto

;------------------------------------------------------------------------------
; ASM functions

	.global	_em_install
	.global _em_deinstall
	.global	_em_pagecount
	.global	_em_map
	.global	_em_mapclean
	.global _em_copyfrom
	.global	_em_copyto


