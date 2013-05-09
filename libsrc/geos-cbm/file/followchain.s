;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char FollowChain  (struct tr_se *myTrSe, char *buffer);

            .export _FollowChain
            .import popax, setoserror
            .import gettrse

            .include "jumptab.inc"
            .include "geossym.inc"
        
_FollowChain:
        sta r3L
        stx r3H
        jsr popax
        jsr gettrse
        sta r1L
        stx r1H
        jsr FollowChain
        jmp setoserror
