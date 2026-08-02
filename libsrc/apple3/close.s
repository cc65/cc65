;
; Oliver Schmidt, 30.12.2004
; Robert Justice, 2026
;
; int __fastcall__ close (int fd);
;

        .export         _close

        .import         closedirect, freebuffer

        .include        "errno.inc"
        .include        "filedes.inc"

_close:
        ; Process fd
        jsr     getfd           ; Returns A, Y and C
        bcs     errno

        ; Check for device
        cmp     #$80
        beq     zerofd

        ; Close file
        jsr     closedirect     ; Preserves Y
        bne     oserr

        ; Mark fdtab slot as free
zerofd: lda     #$00
        sta     fdtab + FD::REF_NUM,y

        ; Return success
        lda     #$00

        ; Set ___oserror
oserr:  jmp     ___mappederrno

        ; Set __errno
errno:  jmp     ___directerrno
