;
; Oliver Schmidt, 2013-05-31
;

        .export         joy_libref, ser_libref, tgi_libref
        .import         _exit, PLOT, BSOUT

joy_libref      := _exit
ser_libref      := _exit
tgi_libref      := PLOT
