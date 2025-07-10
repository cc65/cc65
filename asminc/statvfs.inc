;****************************************************************************
;*                                                                          *
;*                              statvfs.inc                                 *
;*                                                                          *
;*                             Statvfs struct                               *
;*                                                                          *
;*                                                                          *
;*                                                                          *
;*(C) 2023 Colin Leroy-Mira <colin@colino.net>                              *
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
; struct statvfs

.struct statvfs
        f_bsize           .dword
        f_frsize          .dword
        f_blocks          .dword
        f_bfree           .dword
        f_bavail          .dword
        f_files           .dword
        f_ffree           .dword
        f_favail          .dword
        f_fsid            .dword
        f_flag            .dword
        f_namemax         .dword
.endstruct
