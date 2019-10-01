;
; 2019-09-16, Greg King
;
; This module supplies the load addresses that are expected
; by a Commander X16 in the first two bytes of banked RAM load files.
;

        ; The following symbol is used by a linker config. to force
        ; this module to get included into the output files.
        .export         __BANKRAMADDR__: abs = 1

.segment        "BRAM00ADDR"

        .addr   *+2

.segment        "BRAM01ADDR"

        .addr   *+2

.segment        "BRAM02ADDR"

        .addr   *+2

.segment        "BRAM03ADDR"

        .addr   *+2

.segment        "BRAM04ADDR"

        .addr   *+2

.segment        "BRAM05ADDR"

        .addr   *+2

.segment        "BRAM06ADDR"

        .addr   *+2

.segment        "BRAM07ADDR"

        .addr   *+2

.segment        "BRAM08ADDR"

        .addr   *+2

.segment        "BRAM09ADDR"

        .addr   *+2
