;
; 2013-05-31, Oliver Schmidt
; 2020-01-06, Greg King
;

        .export         em_libref
        .export         joy_libref
        .export         mouse_libref
        .export         ser_libref
        .export         tgi_libref

        .import         _exit

em_libref       := _exit
joy_libref      := _exit
mouse_libref    := _exit
ser_libref      := _exit
tgi_libref      := _exit
