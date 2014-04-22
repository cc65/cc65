;
; Oliver Schmidt, 2013-05-31
;

        .export         em_libref, joy_libref, tgi_libref
        .import         _exit

em_libref       := _exit
joy_libref      := _exit
.ifdef __ATARIXL__
        .import CIO_handler
tgi_libref      := CIO_handler
.else
tgi_libref      := _exit
.endif
