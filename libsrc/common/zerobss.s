;
; Zero the bss segment.
;

        .export         zerobss
        .import         __BSS_RUN__, __BSS_SIZE__
        .import         _bzero
        .import         pushax

.code

zerobss:
        lda     #<__BSS_RUN__
        ldx     #>__BSS_RUN__

        jsr     pushax

        lda     #<__BSS_SIZE__
        ldx     #>__BSS_SIZE__

        jmp     _bzero
