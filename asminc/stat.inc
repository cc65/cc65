;****************************************************************************
;*                                                                          *
;*                              stat.inc                                    *
;*                                                                          *
;*                             Stat struct                                  *
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

        .include        "time.inc"

;------------------------------------------------------------------------------
; st_mode values

S_IFDIR = $01
S_IFREG = $02

;------------------------------------------------------------------------------
; struct stat

.struct stat
        st_dev            .dword
        st_ino            .dword
        st_mode           .byte
        st_nlink          .dword
        st_uid            .byte
        st_gid            .byte
        st_size           .dword
        st_atim           .tag timespec
        st_ctim           .tag timespec
        st_mtim           .tag timespec
        .ifdef __APPLE2__
        st_access         .byte
        st_type           .byte
        st_auxtype        .word
        st_storagetype    .byte
        st_blocks         .word
        st_mod_date       .word
        st_mod_time       .word
        st_create_date    .word
        st_create_time    .word
        .endif
.endstruct
