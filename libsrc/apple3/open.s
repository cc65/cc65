;
; Oliver Schmidt, 30.12.2004
; Robert Justice, 2026
;
; int open (const char* name, int flags, ...);
;

        .export         _open, closedirect
        .export         __filetype, __auxtype
        .constructor    raisefilelevel
        .destructor     closeallfiles, 5

        .import         pushname_tos, popname, sos_set_pathname_tos
        .import         iobuf_alloc, iobuf_free
        .import         addysp, incsp4, incaxy, pushax, popax

        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "fcntl.inc"
        .include        "sos.inc"
        .include        "filedes.inc"
        .include        "time.inc"

        .segment        "ONCE"

raisefilelevel:
        ; Raise file level
        lda     #GET_LEVEL_CALL
        ldx     #GET_LEVEL_COUNT
        jsr     callsos
        
        ldx     sosparam + SOS::LEVEL::LEVEL
        stx     LEVEL
        inx
        stx     sosparam + SOS::LEVEL::LEVEL

        lda     #SET_LEVEL_CALL
        ldx     #SET_LEVEL_COUNT
        jsr     callsos
        rts

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
        .assert .sizeof(FD) = 2, error
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
        jmp     ___directerrno

        ; Save fdtab slot
found:  sty     tmp2

        ; Get and save flags
        jsr     popax
        sta     tmp3

        ; Get and push name
        jsr     pushname_tos
        bne     oserr1

        ; Set pushed name
        jsr     sos_set_pathname_tos

        ; Check for create flag
        lda     tmp3            ; Restore flags
        and     #O_CREAT
        beq     open

        ; PATHNAME already set
        .assert SOS::CREATE::PATHNAME = SOS::OPEN::PATHNAME, error

        ; Set all other parameters from template
        ldx     #(OPTION::CREATE::STORAGE_TYPE+1) - (OPTION::CREATE::FILE_TYPE)
        stx     sosparam + SOS::CREATE::LENGTH
        dex
:       lda     CREATE,x
        sta     sosoption + OPTION::CREATE::FILE_TYPE,x
        dex
        bpl     :-

        ; Create file
        lda     #CREATE_CALL
        ldx     #CREATE_COUNT
        jsr     callsos
        beq     open

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

        ; Set ___oserror
        jmp     ___mappederrno

open:   lda     #0              ; no option list
        sta     sosparam + SOS::OPEN::LENGTH

        ; Open file
        lda     #OPEN_CALL
        ldx     #OPEN_COUNT
        jsr     callsos
        bne     oserr2

        ; Get and save fd
        ldx     sosparam + SOS::OPEN::REF_NUM
        stx     tmp1            ; Save fd

        ; Set flags and check for truncate flag
        ldy     tmp2            ; Restore fdtab slot
        lda     tmp3            ; Restore flags
        sta     fdtab + FD::FLAGS,y
        and     #O_TRUNC
        beq     done

        ; Set fd and zero size
        stx     sosparam + SOS::SET_EOF::REF_NUM
        ldx     #$04
        lda     #$00
:       sta     sosparam + SOS::SET_EOF::BASE,x   ; base & eof = 0
        dex
        bpl     :-

        ; Set file size
        lda     #SET_EOF_CALL
        ldx     #SET_EOF_COUNT
        jsr     callsos
        beq     done

        ; Cleanup file
        pha                     ; Save oserror code
        lda     tmp1            ; Restore fd
        jsr     closedirect
        pla                     ; Restore oserror code
        bne     oserr2          ; Branch always

        ; Store fd
done:   ldy     tmp2            ; Restore fdtab slot
        lda     tmp1            ; Restore fd
        sta     fdtab + FD::REF_NUM,y

        ; Convert fdtab slot to handle
        .assert .sizeof(FD) = 2, error
        tya
        lsr

        ; Cleanup name
        jsr     popname         ; Preserves A

        ; Return success
        ldx     #>$0000
        stx     ___oserror
        rts

closedirect:
        ; Set fd
        sta     sosparam + SOS::CLOSE::REF_NUM

        ; Call close
        lda     #CLOSE_CALL
        ldx     #CLOSE_COUNT
        jmp     callsos

closeallfiles:
        ; All open files with current level (or higher)
        lda     #$00
        jsr     closedirect

        ; Restore original file level
        lda     LEVEL
        sta     sosparam + SOS::LEVEL::LEVEL

        lda     #SET_LEVEL_CALL
        ldx     #SET_LEVEL_COUNT
        jsr     callsos
        rts


        .data

LEVEL:  .byte   0

CREATE:
__filetype:
        .byte   $06             ; FILE_TYPE:    Standard binary file
__auxtype:
        .word   $0000           ; AUX_TYPE:     Load address N/A
        .byte   $01             ; STORAGE_TYPE: Standard seedling file

