;
; Maciej 'YTM/Elysium' Witkowiak
;
; 30.10.99, 17.04.2003

; void EnterDeskTop (void);

            .import _exit
            .export _EnterDeskTop

_EnterDeskTop:
        lda #0
        tax
        jmp _exit
