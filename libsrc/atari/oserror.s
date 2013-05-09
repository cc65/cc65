;
; Christian Groessler, May-2000
;
; os specific error code mapping
; int __fastcall__ _osmaperrno (unsigned char oserror);
;

        .include "errno.inc"
        .export __osmaperrno

.proc   __osmaperrno

        cmp     #$80            ; error or success
        bcs     errcode         ; error, jump

        lda     #0              ; no error, return 0
        tax
        rts

errcode:and     #$7f            ; create index from error number
        tax
        cpx     #MAX_OSERR_VAL  ; valid number?
        bcs     inverr          ; no

        lda     maptable,x
        ldx     #0
        rts

inverr: lda     #<EUNKNOWN
        ldx     #>EUNKNOWN
        rts

.endproc

.rodata

maptable:
        .byte   EINTR   ;BRKABT = 128           ;($80) BREAK key abort
        .byte   EBUSY   ;PRVOPN = 129           ;($81) IOCB already open error
        .byte   ENODEV  ;NONDEV = 130           ;($82) nonexistent device error
        .byte   EACCES  ;WRONLY = 131           ;($83) IOCB opened for write only error
        .byte   ENOSYS  ;NVALID = 132           ;($84) invalid command error
        .byte   EINVAL  ;NOTOPN = 133           ;($85) device/file not open error
        .byte   EINVAL  ;BADIOC = 134           ;($86) invalid IOCB index error
        .byte   EACCES  ;RDONLY = 135           ;($87) IOCB opened for read only error
        .byte   EINVAL  ;EOFERR = 136           ;($88) end of file error (should never come,
                                                ;      specially handled by read.s)
        .byte   EIO     ;TRNRCD = 137           ;($89) truncated record error
        .byte   EIO     ;TIMOUT = 138           ;($8A) peripheral device timeout error
        .byte   EIO     ;DNACK  = 139           ;($8B) device does not acknowledge command
        .byte   EIO     ;FRMERR = 140           ;($8C) serial bus framing error
        .byte   EINVAL  ;CRSROR = 141           ;($8D) cursor out of range error
        .byte   EIO     ;OVRRUN = 142           ;($8E) serial bus data overrun error
        .byte   EIO     ;CHKERR = 143           ;($8F) serial bus checksum error
        .byte   EIO     ;DERROR = 144           ;($90) general device failure
        .byte   EINVAL  ;BADMOD = 145           ;($91) bad screen mode number error
        .byte   ENOSYS  ;FNCNOT = 146           ;($92) function not implemented in handler
        .byte   ENOMEM  ;SCRMEM = 147           ;($93) insufficient memory for screen mode
; codes below taken from "Mein Atari Computer" (german version of "Your Atari Computer")
; also SpartaDOS codes from http://www.atari-central.com/programming/cio_errors.txt
; MyDOS and XDOS codes from Stefan Haubenthal
        .byte   EUNKNOWN        ; 148 - [SpartaDOS] unrecognized disk format
        .byte   EUNKNOWN        ; 149 - [SpartaDOS] disk created by incompatible version of SD
        .byte   EBUSY           ; 150 - serial port already open
                                ;       [SpartaDOS] directory not found
        .byte   EACCES          ; 151 - concurrent mode I/O not enabled (serial)
                                ;       [SpartaDOS] file exists
        .byte   EINVAL          ; 152 - invalid buffer address for concurrent mode
                                ;       [SpartaDOS] not binary format
        .byte   EAGAIN          ; 153 - concurrent mode enabled (and another access tried)
        .byte   EACCES          ; 154 - concurrent mode I/O not active (serial)
                                ;       [SpartaDOS X] loader symbol not defined
        .byte   EUNKNOWN        ; 155 - haven't found documentation
        .byte   EUNKNOWN        ; 156 - [SpartaDOS X] bad parameter
        .byte   EUNKNOWN        ; 157 - haven't found documentation
        .byte   EUNKNOWN        ; 158 - [SpartaDOS X] out of memory
        .byte   EUNKNOWN        ; 159 - haven't found documentation
        .byte   ENOENT          ; 160 - drive number error (DOS)
        .byte   EMFILE          ; 161 - too many open files
        .byte   ENOSPC          ; 162 - disk full
        .byte   EIO             ; 163 - unrecoverable system data I/O error
        .byte   ESPIPE          ; 164 - file number mismatch (inv. seek or disk data strucs damaged)
        .byte   ENOENT          ; 165 - invalid file name (e.g. lowercase)
        .byte   ESPIPE          ; 166 - point data length error
        .byte   EACCES          ; 167 - file locked (read-only)
        .byte   ENOSYS          ; 168 - command invalid for disk
        .byte   ENOSPC          ; 169 - directory full
        .byte   ENOENT          ; 170 - file not found
        .byte   ESPIPE          ; 171 - point command invalid
        .byte   EEXIST          ; 172 - [MYDOS] already exists in parent directory
        .byte   EUNKNOWN        ; 173 - bad disk - format couldn't complete
        .byte   EUNKNOWN        ; 174 - [MYDOS] directory not in parent directory
        .byte   EUNKNOWN        ; 175 - [MYDOS] directory not empty
        .byte   EUNKNOWN        ; 176 - [DOS 3] incompatible file system
        .byte   EUNKNOWN        ; 177 - haven't found documentation
        .byte   EUNKNOWN        ; 178 - haven't found documentation
        .byte   EUNKNOWN        ; 179 - haven't found documentation
        .byte   EUNKNOWN        ; 180 - not a binary file
        .byte   EUNKNOWN        ; 181 - [MYDOS] invalid address range
        .byte   EUNKNOWN        ; 182 - [XDOS] invalid parameter

        .byte   EINVAL          ; 183 - dummy (used by cc65 rtl, see sysremove.s)

MAX_OSERR_VAL = (* - maptable)
