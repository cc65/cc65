;/*****************************************************************************/
;/*                                                                           */
;/*                              joy-kernel.inc                               */
;/*                                                                           */
;/*                    Internally used joystick functions                     */
;/*                                                                           */
;/*                                                                           */
;/*                                                                           */
;/* (C) 2002-2006, Ullrich von Bassewitz                                      */
;/*                Roemerstrasse 52                                           */
;/*                D-70794 Filderstadt                                        */
;/* EMail:         uz@cc65.org                                                */
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

.struct JOY_HDR
        ID      .byte   3       ; $6A, $6F, $79 ("joy")
        VERSION .byte   1       ; Interface version
        LIBREF  .addr           ; Library reference
        JUMPTAB .struct
            INSTALL     .addr   ; INSTALL routine
            UNINSTALL   .addr   ; UNINSTALL routine
            COUNT       .addr   ; COUNT routine
            READ        .addr   ; READ routine
        .endstruct
.endstruct

;------------------------------------------------------------------------------
; The JOY API version, stored in JOY_HDR::VERSION

JOY_API_VERSION         = $05

;------------------------------------------------------------------------------
; Variables

        .global _joy_drv                        ; Pointer to driver
        .global _joy_masks

;------------------------------------------------------------------------------
; Driver entry points

        .global joy_install
        .global joy_uninstall
        .global joy_count
        .global joy_read

;------------------------------------------------------------------------------
; C callable functions

        .global _joy_load_driver
        .global _joy_unload
        .global _joy_install
        .global _joy_uninstall
        .global _joy_count
        .global _joy_read

        .global _joy_clear_ptr
