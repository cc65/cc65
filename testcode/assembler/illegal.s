
        .setcpu         "6502X"


.macro  test    opc

        opc     $00
        opc     $00,x
        opc     ($00,x)
        opc     ($00),y
        opc     $1234
        opc     $1234,x
        opc     $1234,y

.endmacro


        test    slo
        test    rla
        test    sre
        test    rra
        test    dcp
        test    isc

        sax     $00
        sax     $00,y
        sax     ($00,x)
        sax     $1234

        lax     $00
        lax     $00,y
        lax     ($00,x)
        lax     ($00),y
        lax     $1234
        lax     $1234,y

        anc     #$55
        alr     #$55
        arr     #$55
        axs     #$55

        las     $1234,y

