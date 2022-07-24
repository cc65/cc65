;
; Oliver Schmidt, 30.12.2004
;
; int open (const char* name, int flags, ...);
;

        .export         _open, closedirect, freebuffer
        .export         __filetype, __auxtype, __datetime
        .constructor    raisefilelevel
        .destructor     closeallfiles, 5

        .import         pushname, popname, __dos_type
        .import         iobuf_alloc, iobuf_free
        .import         addysp, incsp4, incaxy, pushax, popax

        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "fcntl.inc"
        .include        "mli.inc"
        .include        "filedes.inc"

        .segment        "ONCE"

raisefilelevel:
        ; Raise file level
        lda     __dos_type
        beq     :+
        inc     LEVEL
:       rts

        .code

_open:
        ; Throw away all parameters except name
        ; and flags occupying together 4 bytes
        dey
        dey
        dey
        dey
        jsr     addysp

        ; Start with first fdtab slot
        ldy     #$00

        ; Check for free fdtab slot
:       lda     fdtab + FD::REF_NUM,y
        beq     found

        ; Advance to next fdtab slot
        .assert .sizeof(FD) = 4, error
        iny
        iny
        iny
        iny

        ; Check for end of fdtab
        cpy     #MAX_FDS * .sizeof(FD)
        bcc     :-

        ; Load errno code
        lda     #EMFILE

        ; Cleanup stack
errno:  jsr     incsp4          ; Preserves A

        ; Set __errno
        jmp     __directerrno

        ; Save fdtab slot
found:  tya
        pha

        ; Alloc I/O buffer
        lda     #<(fdtab + FD::BUFFER)
        ldx     #>(fdtab + FD::BUFFER)
        jsr     incaxy
        jsr     pushax
        lda     #$00
        ldx     #>$0100
        jsr     pushax          ; Preserves A
        ldx     #>$0400
        jsr     iobuf_alloc
        tay                     ; Save errno code

        ; Restore fdtab slot
        pla
        sta     tmp2            ; Save fdtab slot

        ; Check for error
        tya                     ; Restore errno code
        bne     errno

        ; Get and save flags
        jsr     popax
        sta     tmp3

        ; Get and push name
        jsr     popax
        jsr     pushname
        bne     oserr1

        ; Set pushed name
        lda     sp
        ldx     sp+1
        sta     mliparam + MLI::OPEN::PATHNAME
        stx     mliparam + MLI::OPEN::PATHNAME+1

        ; Check for create flag
        lda     tmp3            ; Restore flags
        and     #O_CREAT
        beq     open

        ; PATHNAME already set
        .assert MLI::CREATE::PATHNAME = MLI::OPEN::PATHNAME, error

        ; Set all other parameters from template
        ldx     #(MLI::CREATE::CREATE_TIME+1) - (MLI::CREATE::PATHNAME+1) - 1
:       lda     CREATE,x
        sta     mliparam + MLI::CREATE::ACCESS,x
        dex
        bpl     :-

        ; Create file
        lda     #CREATE_CALL
        ldx     #CREATE_COUNT
        jsr     callmli
        bcc     open

        ; Check for ordinary errors
        cmp     #$47            ; "Duplicate filename"
        bne     oserr2

        ; Check for exclusive flag
        lda     tmp3            ; Restore flags
        and     #O_EXCL
        beq     open

        lda     #$47            ; "Duplicate filename"

        ; Cleanup name
oserr2: jsr     popname         ; Preserves A

oserr1: ldy     tmp2            ; Restore fdtab slot

        ; Cleanup I/O buffer
        pha                     ; Save oserror code
        jsr     freebuffer
        pla                     ; Restore oserror code

        ; Set __oserror
        jmp     __mappederrno

open:   ldy     tmp2            ; Restore fdtab slot

        ; Set allocated I/O buffer
        ldx     fdtab + FD::BUFFER+1,y
        sta     mliparam + MLI::OPEN::IO_BUFFER         ; A = 0
        stx     mliparam + MLI::OPEN::IO_BUFFER+1

        ; Open file
        lda     #OPEN_CALL
        ldx     #OPEN_COUNT
        jsr     callmli
        bcs     oserr2

        ; Get and save fd
        ldx     mliparam + MLI::OPEN::REF_NUM
        stx     tmp1            ; Save fd

        ; Set flags and check for truncate flag
        lda     tmp3            ; Restore flags
        sta     fdtab + FD::FLAGS,y
        and     #O_TRUNC
        beq     done

        ; Set fd and zero size
        stx     mliparam + MLI::EOF::REF_NUM
        ldx     #$02
        lda     #$00
:       sta     mliparam + MLI::EOF::EOF,x
        dex
        bpl     :-

        ; Set file size
        lda     #SET_EOF_CALL
        ldx     #EOF_COUNT
        jsr     callmli
        bcc     done

        ; Cleanup file
        pha                     ; Save oserror code
        lda     tmp1            ; Restore fd
        jsr     closedirect
        pla                     ; Restore oserror code
        bne     oserr2          ; Branch always

        ; Store fd
done:   lda     tmp1            ; Restore fd
        sta     fdtab + FD::REF_NUM,y

        ; Convert fdtab slot to handle
        .assert .sizeof(FD) = 4, error
        tya
        lsr
        lsr

        ; Cleanup name
        jsr     popname         ; Preserves A

        ; Return success
        ldx     #$00
        stx     __oserror
        rts

freebuffer:
        ; Free I/O buffer
        lda     #$00
        ldx     fdtab + FD::BUFFER+1,y
        jmp     iobuf_free

closedirect:
        ; Set fd
        sta     mliparam + MLI::CLOSE::REF_NUM

        ; Call close
        lda     #CLOSE_CALL
        ldx     #CLOSE_COUNT
        jmp     callmli

closeallfiles:
        ; All open files with current level (or higher)
        lda     #$00
        jsr     closedirect

        ; Restore original file level
        lda     __dos_type
        beq     :+
        dec     LEVEL
:       rts

        .data

CREATE: .byte   %11000011       ; ACCESS:       Standard full access
__filetype:
        .byte   $06             ; FILE_TYPE:    Standard binary file
__auxtype:
        .word   $0000           ; AUX_TYPE:     Load address N/A
        .byte   $01             ; STORAGE_TYPE: Standard seedling file
__datetime:
        .word   $0000           ; CREATE_DATE:  Current date
        .word   $0000           ; CREATE_TIME:  Current time
