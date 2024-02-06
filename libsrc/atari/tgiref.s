;
; Oliver Schmidt, 2013-05-31
;

        .export         tgi_libref

.ifdef __ATARIXL__
        .import CIO_handler
tgi_libref      := CIO_handler
.else
        .import _exit
tgi_libref      := _exit
.endif
