;/*****************************************************************************/
;/*                                                                           */
;/*                              joy-kernel.inc                               */
;/*                                                                           */
;/*                    Internally used joystick functions                     */
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

JOY_HDR_ID              = 0                     ; $6A, $6F, $79 ("joy")
JOY_HDR_VERSION         = 3                     ; Interface version

JOY_MASKS               = 4                     ; Joystick state mask array
JOY_MASK_COUNT          = 8                     ; Size of the array

JOY_HDR_JUMPTAB         = 12
JOY_HDR_INSTALL         = JOY_HDR_JUMPTAB+0     ; INSTALL routine
JOY_HDR_DEINSTALL       = JOY_HDR_JUMPTAB+2     ; DEINSTALL routine
JOY_HDR_COUNT           = JOY_HDR_JUMPTAB+4     ; COUNT routine
JOY_HDR_READ            = JOY_HDR_JUMPTAB+6     ; READ routine

JOY_HDR_JUMPCOUNT       = 4                     ; Number of jump vectors

;------------------------------------------------------------------------------
; Variables

       	.global	_joy_drv                        ; Pointer to driver

;------------------------------------------------------------------------------
; Driver entry points

        .global joy_install
        .global joy_deinstall
        .global joy_count
        .global joy_read

;------------------------------------------------------------------------------
; ASM functions

       	.global	_joy_install
       	.global _joy_deinstall
       	.global	_joy_count
       	.global	_joy_read

