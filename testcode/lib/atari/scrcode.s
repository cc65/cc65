; Christian Groessler, 30-Aug-2005
;
; scrcode macro test
;
; compile with
;       ca65 -I../../../asminc -tatari -o scrcode.o scrcode.s
;       ld65 -tatari -o scrcode.com scrcode.o

.import         __CODE_LOAD__, __BSS_LOAD__

.include        "atari.inc"
.macpack        atari

.code

                rts             ; SpartaDOS workaround

; entry point

                lda     #0
                tay
                tax

; display dispdata

disp:           lda     dispdata,x
                sta     (SAVMSC),y
                inx
                iny
                cpx     #disp_len
                bne     disp

; wait for key press

key:            lda     CH
                cmp     #255
                beq     key

                rts

.data

dispdata:       scrcode         "fooBa", 'r', $66, 3+4
disp_len        =       * - dispdata

.segment        "AUTOSTRT"

                .word   $02E0
                .word   $02E1
                .word   __CODE_LOAD__+1

.segment        "EXEHDR"

                .word   $FFFF
                .word   __CODE_LOAD__
                .word   __BSS_LOAD__ - 1

.end
