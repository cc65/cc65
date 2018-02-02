;
; Oliver Schmidt, 2018-02-02
;

        .export         tgi_libref
        .interruptor    tgi_irq         ; Export as IRQ handler

        .data

tgi_libref:
tgi_irq:        .byte   $60, $00, $00   ; RTS plus two dummy bytes
