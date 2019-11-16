;
; 2019-11-05, Greg King
;

        .export         ST: zp

.segment        "EXTZP": zp

; This is a temporary hack.

; A zero-page copy of the IEC status byte.
; This is needed because the Commander X16's Kernal's status
; variable was moved out of the zero page.  But, the common
; CBM file function modules import this as a zero-page variable.

ST:     .res    1
