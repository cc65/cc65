;/*****************************************************************************/
;/*                                                                           */
;/*                            mouse-kernel.inc                               */
;/*                                                                           */
;/*				   Mouse API				      */
;/*                                                                           */
;/*                                                                           */
;/*                                                                           */
;/* (C) 2003-2004 Ullrich von Bassewitz                                       */
;/*               Römerstraße 52                                              */
;/*               D-70794 Filderstadt                                         */
;/* EMail:        uz@cc65.org                                                 */
;/*                                                                           */
;/*                                                                           */
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
; Error codes

.enum
        MOUSE_ERR_OK                    ; No error
        MOUSE_ERR_NO_DRIVER             ; No driver available
        MOUSE_ERR_CANNOT_LOAD           ; Error loading driver
        MOUSE_ERR_INV_DRIVER            ; Invalid driver
        MOUSE_ERR_NO_DEVICE             ; Mouse hardware not found
        MOUSE_ERR_INV_IOCTL             ; Invalid ioctl code

        MOUSE_ERR_COUNT                 ; Special: Number of error codes
.endenum

;------------------------------------------------------------------------------
; The driver header

.struct	MOUSE_HDR
	ID   	        .byte  	3       ; Contains 0x6D, 0x6F, 0x75 ("mou")
	VERSION         .byte  	1      	; Interface version
	JUMPTAB         .struct
	    INSTALL  	.addr
	    UNINSTALL	.addr
       	    HIDE        .addr
	    SHOW        .addr
	    BOX         .addr
	    MOVE        .addr
	    BUTTONS     .addr
	    POS         .addr
            INFO        .addr
            IOCTL       .addr
	    IRQ	     	.addr
	.endstruct
        CALLBACKS .struct               ; Jump instructions
                        .byte           ; JMP opcode
            CHIDE       .addr           ; Jump address
                        .byte
            CSHOW       .addr
                        .byte
            CMOVEX      .addr
                        .byte
            CMOVEY      .addr
        .endstruct
.endstruct

;------------------------------------------------------------------------------
; The mouse callback structure

.struct	MOUSE_CALLBACKS
	HIDE	.addr			; Hide the mouse cursor
	SHOW	.addr			; Show the mouse cursor
       	MOVEX	.addr			; Move the mouse cursor
        MOVEY   .addr                   ; Dito for Y
.endstruct

;------------------------------------------------------------------------------
; The mouse API version, stored in MOUSE_HDR::VERSION

MOUSE_API_VERSION       = $00

;------------------------------------------------------------------------------
; Mouse button definitions

MOUSE_BTN_LEFT 	        = $10
MOUSE_BTN_RIGHT         = $01

;------------------------------------------------------------------------------
; Structures used to return data from the mouse driver

.struct MOUSE_POS
        XCOORD  .word
        YCOORD  .word
.endstruct

.struct MOUSE_INFO
        POS     .tag    MOUSE_POS
        BUTTONS .byte
.endstruct


;------------------------------------------------------------------------------
; Variables

       	.global	_mouse_drv              ; Pointer to driver
        .global _mouse_hidden           ; Counter, 0 = mouse is visible

;------------------------------------------------------------------------------
; C callable functions

        .global _mouse_load_driver
        .global _mouse_unload
        .global _mouse_install
        .global _mouse_uninstall
        .global _mouse_geterrormsg
        .global _mouse_hide
        .global _mouse_show
        .global _mouse_box
        .global _mouse_move
        .global _mouse_buttons
        .global _mouse_pos
        .global _mouse_info
        .global _mouse_ioctl

;------------------------------------------------------------------------------
; Driver entry points

        .global mouse_install
        .global mouse_uninstall
        .global mouse_hide
        .global mouse_show
        .global mouse_box
        .global mouse_move
        .global mouse_buttons
        .global mouse_pos
        .global mouse_info
        .global mouse_ioctl


