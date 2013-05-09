;
; Ullrich von Bassewitz, 16.11.2002
;
; int open (const char* name, int flags, ...);  /* May take a mode argument */
;
; Be sure to keep the value priority of closeallfiles lower than that of
; closeallstreams (which is the high level C file I/O counterpart and must be
; called before closeallfiles).


        .export         _open
        .destructor     closeallfiles, 5

        .import         SETLFS, OPEN, CLOSE
        .import         addysp, popax
        .import         scratch, fnparse, fnaddmode, fncomplete, fnset
        .import         opencmdchannel, closecmdchannel, readdiskerror
        .import         fnunit, fnisfile
        .import         _close
        .importzp       sp, tmp2, tmp3

        .include        "errno.inc"
        .include        "fcntl.inc"
        .include        "filedes.inc"


;--------------------------------------------------------------------------
; closeallfiles: Close all open files.

.proc   closeallfiles

        ldx     #MAX_FDS-1
loop:   lda     fdtab,x
        beq     next            ; Skip unused entries

; Close this file

        txa
        pha                     ; Save current value of X
        ldx     #0
        jsr     _close
        pla
        tax

; Next file

next:   dex
        bpl     loop

        rts

.endproc

;--------------------------------------------------------------------------
; _open

.proc   _open

; Throw away any additional parameters passed through the ellipsis

        dey                     ; Parm count < 4 shouldn't be needed to be...
        dey                     ; ...checked (it generates a c compiler warning)
        dey
        dey
        beq     parmok          ; Branch if parameter count ok
        jsr     addysp          ; Fix stack, throw away unused parameters

; Parameters ok. Pop the flags and save them into tmp3

parmok: jsr     popax           ; Get flags
        sta     tmp3

; Get the filename from stack and parse it. Bail out if is not ok

        jsr     popax           ; Get name
        jsr     fnparse         ; Parse it
        tax
        bne     oserror         ; Bail out if problem with name

; Get a free file handle and remember it in tmp2

        jsr     freefd
        lda     #EMFILE         ; Load error code
        bcs     seterrno        ; Jump in case of errors
        stx     tmp2

; Check the flags. We cannot have both, read and write flags set, and we cannot
; open a file for writing without creating it.

        lda     tmp3
        and     #(O_RDWR | O_CREAT)
        cmp     #O_RDONLY       ; Open for reading?
        beq     doread          ; Yes: Branch
        cmp     #(O_WRONLY | O_CREAT)   ; Open for writing?
        beq     dowrite

; Invalid open mode

        lda     #EINVAL

; Error entry. Sets _errno, clears _oserror, returns -1

seterrno:
        jmp     __directerrno

; Error entry: Close the file and exit. OS error code is in A on entry

closeandexit:
        pha
        lda     tmp2
        clc
        adc     #LFN_OFFS
        jsr     CLOSE
        ldx     fnunit
        jsr     closecmdchannel
        pla

; Error entry: Set oserror and errno using error code in A and return -1

oserror:jmp     __mappederrno

; Read bit is set. Add an 'r' to the name

doread: lda     #'r'
        jsr     fnaddmode       ; Add the mode to the name
        lda     #LFN_READ
        bne     common          ; Branch always

; If O_TRUNC is set, scratch the file, but ignore any errors

dowrite:
        lda     tmp3
        and     #O_TRUNC
        beq     notrunc
        jsr     scratch

; Complete the the file name. Check for append mode here.

notrunc:
        lda     tmp3            ; Get the mode again
        and     #O_APPEND       ; Append mode?
        bne     append          ; Branch if yes

; Setup the name for create mode

        lda     #'w'
        jsr     fncomplete      ; Add type and mode to the name
        jmp     appendcreate

; Append bit is set. Add an 'a' to the name

append: lda     #'a'
        jsr     fnaddmode       ; Add open mode to file name
appendcreate:
        lda     #LFN_WRITE

; Common read/write code. Flags in A, handle in tmp2

common: sta     tmp3
        jsr     fnset           ; Set the file name

        lda     tmp2
        clc
        adc     #LFN_OFFS
        ldx     fnunit
        ldy     fnisfile        ; Is this a standard file on disk?
        beq     nofile          ; Branch if not
        tay                     ; Use the LFN also as SA for files
nofile:                         ; ... else use SA=0 (read)
        jsr     SETLFS          ; Set the file params

        jsr     OPEN
        bcs     oserror

; Open the the drive command channel and read it

        ldx     fnunit
        jsr     opencmdchannel
        bne     closeandexit
        ldx     fnunit
        jsr     readdiskerror
        bne     closeandexit    ; Branch on error

; File is open. Mark it as open in the table

        ldx     tmp2
        lda     tmp3
        sta     fdtab,x
        lda     fnunit
        sta     unittab,x       ; Remember

; Done. Return the handle in a/x

        txa                     ; Handle
        ldx     #0
        stx     __oserror       ; Clear _oserror
        rts

.endproc


           
