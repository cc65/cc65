; The Atari Lynx Font
; 96 characters from ASCII 32 to 127
; 8 pixels wide, 8 pixels high (first column is not used as a workaround for a Lynx bug)
; bit value 0 = foreground, bit value 1 = background / transparent
        
.rodata

font:
    
        .byte $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF  ;32
        .byte $FF, $E7, $FF, $FF, $E7, $E7, $E7, $E7  ;33
        .byte $FF, $FF, $FF, $FF, $FF, $99, $99, $99  ;34
        .byte $FF, $D7, $D7, $01, $D7, $01, $D7, $D7  ;35
        .byte $FF, $E7, $83, $F9, $C3, $9F, $C1, $E7  ;36
        .byte $FF, $B9, $99, $CF, $E7, $F3, $99, $9D  ;37
        .byte $81, $B3, $31, $8F, $87, $33, $87, $FF  ;38
        .byte $FF, $FF, $FF, $FF, $FF, $E7, $F3, $F9  ;39
        .byte $FF, $F3, $E7, $CF, $CF, $CF, $E7, $F3  ;40
        .byte $FF, $CF, $E7, $F3, $F3, $F3, $E7, $CF  ;41
        .byte $FF, $99, $C3, $81, $C3, $99, $FF, $FF  ;42
        .byte $FF, $FF, $E7, $E7, $81, $E7, $E7, $FF  ;43
        .byte $CF, $E7, $E7, $FF, $FF, $FF, $FF, $FF  ;44
        .byte $FF, $FF, $FF, $FF, $81, $FF, $FF, $FF  ;45
        .byte $FF, $E7, $E7, $FF, $FF, $FF, $FF, $FF  ;46
        .byte $FF, $BF, $9F, $CF, $E7, $F3, $F9, $FD  ;47
        .byte $FF, $C3, $99, $99, $89, $91, $99, $C3  ;48
        .byte $FF, $81, $E7, $E7, $E7, $C7, $E7, $E7  ;49
        .byte $FF, $81, $9F, $CF, $F3, $F9, $99, $C3  ;50
        .byte $FF, $C3, $99, $F9, $E3, $F9, $99, $C3  ;51
        .byte $FF, $F3, $F3, $01, $33, $C3, $E3, $FB  ;52
        .byte $FF, $C3, $99, $F9, $F9, $83, $9F, $81  ;53
        .byte $FF, $C3, $99, $99, $83, $9F, $99, $C3  ;54
        .byte $FF, $E7, $E7, $E7, $E7, $F3, $99, $81  ;55
        .byte $FF, $C3, $99, $99, $C3, $99, $99, $C3  ;56
        .byte $FF, $C3, $99, $F9, $C1, $99, $99, $C3  ;57
        .byte $FF, $FF, $E7, $FF, $FF, $E7, $FF, $FF  ;58
        .byte $CF, $E7, $E7, $FF, $FF, $E7, $FF, $FF  ;59
        .byte $FF, $F1, $E7, $CF, $9F, $CF, $E7, $F1  ;60
        .byte $FF, $FF, $FF, $81, $FF, $81, $FF, $FF  ;61
        .byte $FF, $8F, $E7, $F3, $F9, $F3, $E7, $8F  ;62
        .byte $FF, $E7, $FF, $E7, $F3, $F9, $99, $C3  ;63


        .byte $FF, $C3, $9D, $9F, $91, $91, $99, $C3  ;0
        .byte $FF, $99, $99, $99, $81, $99, $C3, $E7  ;1
        .byte $FF, $83, $99, $99, $83, $99, $99, $83  ;2
        .byte $FF, $C3, $99, $9F, $9F, $9F, $99, $C3  ;3
        .byte $FF, $87, $93, $99, $99, $99, $93, $87  ;4
        .byte $FF, $81, $9F, $9F, $87, $9F, $9F, $81  ;5
        .byte $FF, $9F, $9F, $9F, $87, $9F, $9F, $81  ;6
        .byte $FF, $C3, $99, $99, $91, $9F, $99, $C3  ;7
        .byte $FF, $99, $99, $99, $81, $99, $99, $99  ;8
        .byte $FF, $C3, $E7, $E7, $E7, $E7, $E7, $C3  ;9
        .byte $FF, $C7, $93, $F3, $F3, $F3, $F3, $E1  ;10
        .byte $FF, $99, $93, $87, $8F, $87, $93, $99  ;11
        .byte $FF, $81, $9F, $9F, $9F, $9F, $9F, $9F  ;12
        .byte $FF, $39, $39, $39, $29, $01, $11, $39  ;13
        .byte $FF, $99, $99, $91, $81, $81, $89, $99  ;14
        .byte $FF, $C3, $99, $99, $99, $99, $99, $C3  ;15
        .byte $FF, $9F, $9F, $9F, $83, $99, $99, $83  ;16
        .byte $FF, $F1, $C3, $99, $99, $99, $99, $C3  ;17
        .byte $FF, $99, $93, $87, $83, $99, $99, $83  ;18
        .byte $FF, $C3, $99, $F9, $C3, $9F, $99, $C3  ;19
        .byte $FF, $E7, $E7, $E7, $E7, $E7, $E7, $81  ;20
        .byte $FF, $C3, $99, $99, $99, $99, $99, $99  ;21
        .byte $FF, $E7, $C3, $99, $99, $99, $99, $99  ;22
        .byte $FF, $39, $11, $01, $29, $39, $39, $39  ;23
        .byte $FF, $99, $99, $C3, $E7, $C3, $99, $99  ;24
        .byte $FF, $E7, $E7, $E7, $C3, $99, $99, $99  ;25
        .byte $FF, $81, $9F, $CF, $E7, $F3, $F9, $81  ;26
        .byte $FF, $C3, $CF, $CF, $CF, $CF, $CF, $C3  ;27
        .byte $FF, $03, $9D, $CF, $83, $CF, $ED, $F3  ;28
        .byte $FF, $C3, $F3, $F3, $F3, $F3, $F3, $C3  ;29
        .byte $E7, $E7, $E7, $E7, $81, $C3, $E7, $FF  ;30
        .byte $FF, $DF, $9F, $01, $01, $9F, $DF, $FF  ;31


; gemena
        .byte $FF, $C3, $9D, $9F, $91, $91, $99, $C3  ;224
        .byte $FF, $C1, $99, $C1, $F9, $C3, $FF, $FF  ;225
        .byte $FF, $83, $99, $99, $83, $9F, $9F, $FF  ;226
        .byte $FF, $C3, $9F, $9F, $9F, $C3, $FF, $FF  ;227
        .byte $FF, $C1, $99, $99, $C1, $F9, $F9, $FF  ;228
        .byte $FF, $C3, $9F, $81, $99, $C3, $FF, $FF  ;229
        .byte $FF, $E7, $E7, $E7, $C1, $E7, $F1, $FF  ;230
        .byte $83, $F9, $C1, $99, $99, $C1, $FF, $FF  ;231
        .byte $FF, $99, $99, $99, $83, $9F, $9F, $FF  ;232
        .byte $FF, $C3, $E7, $E7, $C7, $FF, $E7, $FF  ;233
        .byte $C3, $F9, $F9, $F9, $F9, $FF, $F9, $FF  ;234
        .byte $FF, $99, $93, $87, $93, $9F, $9F, $FF  ;235
        .byte $FF, $C3, $E7, $E7, $E7, $E7, $C7, $FF  ;236
        .byte $FF, $39, $29, $01, $83, $93, $FF, $FF  ;237
        .byte $FF, $99, $99, $99, $99, $83, $FF, $FF  ;238
        .byte $FF, $C3, $99, $99, $99, $C3, $FF, $FF  ;239
        .byte $9F, $9F, $83, $99, $99, $83, $FF, $FF  ;240
        .byte $F9, $F9, $C1, $99, $99, $C1, $FF, $FF  ;241
        .byte $FF, $9F, $9F, $9F, $99, $83, $FF, $FF  ;242
        .byte $FF, $83, $F9, $C3, $9F, $C1, $FF, $FF  ;243
        .byte $FF, $F1, $E7, $E7, $E7, $81, $E7, $FF  ;244
        .byte $FF, $C1, $99, $99, $99, $99, $FF, $FF  ;245
        .byte $FF, $E7, $C3, $99, $99, $99, $FF, $FF  ;246
        .byte $FF, $93, $83, $01, $29, $39, $FF, $FF  ;247
        .byte $FF, $99, $C3, $E7, $C3, $99, $FF, $FF  ;248
        .byte $87, $F3, $C1, $99, $99, $99, $FF, $FF  ;249
        .byte $FF, $81, $CF, $E7, $F3, $81, $FF, $FF  ;250
        .byte $FF, $C3, $CF, $CF, $CF, $CF, $CF, $C3  ;251
        .byte $FF, $03, $9D, $CF, $83, $CF, $ED, $F3  ;252
        .byte $FF, $C3, $F3, $F3, $F3, $F3, $F3, $C3  ;253
        .byte $E7, $E7, $E7, $E7, $81, $C3, $E7, $FF  ;254
        .byte $FF, $DF, $9F, $01, $01, $9F, $DF, $FF  ;255

