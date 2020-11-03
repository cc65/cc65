;
; Marc 'BlackJack' Rintsch, 18.03.2001
;
; void __fastcall__ cbm_close (unsigned char lfn);
;

        .import _cbm_k_close
        .import cbm_read_ended_files
        .export _cbm_close

MAX_LFNS        =   10          ; Maximum number of logical file numbers open at once.

.proc _cbm_close
; Remove LFN from list of ended files.
        lda     $B8             ; LFN
        ldx     #MAX_LFNS - 1
@L1:    cmp     cbm_read_ended_files,x
        beq     @L2
        dex
        bpl     @L1
        bmi     @L3             ; (jmp)

@L2:    lda     #0
        sta     cbm_read_ended_files,x
@L3:    jmp _cbm_k_close
.endproc
