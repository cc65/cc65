;/*****************************************************************************/
;/*                                                                           */
;/*                                 time.inc                                  */
;/*                                                                           */
;/*                              Date and time                                */
;/*                                                                           */
;/*                                                                           */
;/*                                                                           */
;/* (C) 2009      Ullrich von Bassewitz                                       */
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
; Struct tm - must match the struct defined in time.h

.struct tm
        tm_sec      .word
        tm_min      .word
        tm_hour     .word
        tm_mday     .word
        tm_mon      .word
        tm_year     .word
        tm_wday     .word
        tm_yday     .word
        tm_isdst    .word
.endstruct


;------------------------------------------------------------------------------
; Struct timespec - must match the struct defined in time.h

.struct timespec
        tv_sec      .dword
        tv_nsec     .dword
.endstruct


;------------------------------------------------------------------------------
; Exported functions

.global         _clock_getres
.global         _clock_gettime
.global         _clock_settime
.global         _localtime
.global         _mktime
