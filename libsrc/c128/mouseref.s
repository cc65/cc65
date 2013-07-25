;
; Pointer for library references by device drivers.
;
; 2013-07-25, Greg King
;

        .export         mouse_libref, _pen_adjuster

        .data

mouse_libref:                   ; generic label for mouse-kernel

; A program optionally can set this pointer to a function that gives
; a calibration value to a driver.  If this pointer isn't NULL,
; then a driver that wants a value can call that function.
;
; The function might read a value from a file; or, it might ask the user
; to help calibrate the driver.
;
; void __fastcall__ (*pen_adjuster)(unsigned char *) = NULL;
;
_pen_adjuster:
        .addr   $0000
