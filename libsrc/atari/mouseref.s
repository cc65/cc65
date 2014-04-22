;
; Christian Groessler, 2014-04-22
;

        .export         mouse_libref

.ifdef __ATARIXL__
        .import set_VTIMR1_handler
mouse_libref    := set_VTIMR1_handler
.else
        .import _exit
mouse_libref    := _exit
.endif
