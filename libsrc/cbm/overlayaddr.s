;
; Oliver Schmidt, 2013-05-28
;
; This module supplies the load addresses that are expected by Commodore
; machines in the first two bytes of overlay disk files.
;


        ; The following symbol is used by linker config to force the module
        ; to get included into the output file
        .export         __OVERLAYADDR__: absolute = 1

.segment        "OVL1ADDR"

        .addr   *+2

.segment        "OVL2ADDR"

        .addr   *+2

.segment        "OVL3ADDR"

        .addr   *+2

.segment        "OVL4ADDR"

        .addr   *+2

.segment        "OVL5ADDR"

        .addr   *+2

.segment        "OVL6ADDR"

        .addr   *+2

.segment        "OVL7ADDR"

        .addr   *+2

.segment        "OVL8ADDR"

        .addr   *+2

.segment        "OVL9ADDR"

        .addr   *+2
