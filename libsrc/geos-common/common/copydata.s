;
; Maciej 'YTM/Elysium' Witkowiak 15.07.2001
;
; Copy the data segment from the LOAD to the RUN location
;

            .export copydata
            .import __DATA_LOAD__, __DATA_RUN__, __DATA_SIZE__

            .include "geossym.inc"
            .include "jumptab.inc"

copydata:
        lda #<__DATA_SIZE__     ; no need to check if it is == 0
        ldx #>__DATA_SIZE__
        sta r2L
        stx r2H

        lda #<__DATA_RUN__
        ldx #>__DATA_RUN__
        sta r1L
        stx r1H

        lda #<__DATA_LOAD__
        ldx #>__DATA_LOAD__
        sta r0L
        stx r0H
        jmp MoveData
