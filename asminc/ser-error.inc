;****************************************************************************
;*                                                                          *
;*                              ser-error.inc                               *
;*                                                                          *
;*                        Serial communication API                          *
;*                                                                          *
;*                                                                          *
;*                                                                          *
;*(C) 2003      Ullrich von Bassewitz                                       *
;*              Römerstrasse 52                                             *
;*              D-70794 Filderstadt                                         *
;*EMail:        uz@cc65.org                                                 *
;*                                                                          *
;*                                                                          *
;*This software is provided 'as-is', without any expressed or implied       *
;*warranty.  In no event will the authors be held liable for any damages    *
;*arising from the use of this software.                                    *
;*                                                                          *
;*Permission is granted to anyone to use this software for any purpose,     *
;*including commercial applications, and to alter it and redistribute it    *
;*freely, subject to the following restrictions:                            *
;*                                                                          *
;*1. The origin of this software must not be misrepresented; you must not   *
;*   claim that you wrote the original software. If you use this software   *
;*   in a product, an acknowledgment in the product documentation would be  *
;*   appreciated but is not required.                                       *
;*2. Altered source versions must be plainly marked as such, and must not   *
;*   be misrepresented as being the original software.                      *
;*3. This notice may not be removed or altered from any source              *
;*   distribution.                                                          *
;*                                                                          *
;****************************************************************************



;------------------------------------------------------------------------------
; Error codes

SER_ERR_OK     	       	=       $00    	; Not an error - relax
SER_ERR_NO_DRIVER       =       $01     ; No driver available
SER_ERR_CANNOT_LOAD     =       $02     ; Error loading driver
SER_ERR_INV_DRIVER      =       $03     ; Invalid driver
SER_ERR_NO_DEVICE       =       $04     ; Device (hardware) not found
SER_ERR_BAUD_UNAVAIL   	=       $05    	; Baud rate not available
SER_ERR_NO_DATA	       	=       $06   	; Nothing to read
SER_ERR_OVERFLOW       	=       $07   	; No room in send buffer
SER_ERR_INIT_FAILED     =       $08   	; Initialization failed
SER_ERR_INV_IOCTL       =       $09     ; IOCTL not supported


