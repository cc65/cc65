;
; Oliver Schmidt, 2013-05-31
;

        .export         em_libref, joy_libref, tgi_libref, ser_libref
        .import         _exit
        .import         atari_ser_libref

em_libref       := _exit
joy_libref      := _exit
ser_libref      := atari_ser_libref
.ifdef __ATARIXL__
        .import CIO_handler
tgi_libref      := CIO_handler
.else
tgi_libref      := _exit
.endif
