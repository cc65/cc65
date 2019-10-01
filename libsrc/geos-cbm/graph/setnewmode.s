;
; Scott Hutter
;
; 18.12.18

; void SetNewMode(void);

            .export _SetNewMode

            .include "jumptab.inc"
            .include "geossym.inc"
        
_SetNewMode:
            lda graphMode
            eor #$80
            sta graphMode
            jmp SetNewMode

