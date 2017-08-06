;
; Ullrich von Bassewitz, 02.06.1998
;
; Character specification table.
;

; The tables are readonly, put them into the rodata segment

.rodata

; The following 256 byte wide table specifies attributes for the isxxx type
; of functions. Doing it by a table means some overhead in space, but it
; has major advantages:
;
;   * It is fast. If it were'nt for the slow parameter passing of cc65, one
;     could even define macros for the isxxx functions (this is usually
;     done on other platforms).
;
;   * It is highly portable. The only unportable part is the table itself,
;     all real code goes into the common library.
;
;   * We save some code in the isxxx functions.
;
;
; Bit assignments:
;
;   0 - Lower case char
;   1 - Upper case char
;   2 - Numeric digit
;   3 - Hex digit (both, lower and upper)
;   4 - Control character
;   5 - The space character itself
;   6 - Other whitespace (that is: '\f', '\n', '\r', '\t' and '\v')
;   7 - Space or tab character

        .export         __ctype

__ctype:

.repeat 2       ; 2 times for normal and inverted

        .byte   $10     ;   0/00 ___ctrl_@___
        .byte   $10     ;   1/01 ___ctrl_A___
        .byte   $10     ;   2/02 ___ctrl_B___
        .byte   $10     ;   3/03 ___ctrl_C___
        .byte   $10     ;   4/04 ___ctrl_D___
        .byte   $10     ;   5/05 ___ctrl_E___
        .byte   $10     ;   6/06 ___ctrl_F___
        .byte   $10     ;   7/07 ___ctrl_G___
        .byte   $10     ;   8/08 ___ctrl_H___
        .byte   $D0     ;   9/09 ___ctrl_I___
        .byte   $50     ;  10/0a ___ctrl_J___
        .byte   $50     ;  11/0b ___ctrl_K___
        .byte   $50     ;  12/0c ___ctrl_L___
        .byte   $50     ;  13/0d ___ctrl_M___
        .byte   $10     ;  14/0e ___ctrl_N___
        .byte   $10     ;  15/0f ___ctrl_O___
        .byte   $10     ;  16/10 ___ctrl_P___
        .byte   $10     ;  17/11 ___ctrl_Q___
        .byte   $10     ;  18/12 ___ctrl_R___
        .byte   $10     ;  19/13 ___ctrl_S___
        .byte   $10     ;  20/14 ___ctrl_T___
        .byte   $10     ;  21/15 ___ctrl_U___
        .byte   $10     ;  22/16 ___ctrl_V___
        .byte   $10     ;  23/17 ___ctrl_W___
        .byte   $10     ;  24/18 ___ctrl_X___
        .byte   $10     ;  25/19 ___ctrl_Y___
        .byte   $10     ;  26/1a ___ctrl_Z___
        .byte   $10     ;  27/1b ___ctrl_[___
        .byte   $10     ;  28/1c ___ctrl_\___
        .byte   $10     ;  29/1d ___ctrl_]___
        .byte   $10     ;  30/1e ___ctrl_^___
        .byte   $10     ;  31/1f ___ctrl_____
        .byte   $A0     ;  32/20 ___SPACE___
        .byte   $00     ;  33/21 _____!_____
        .byte   $00     ;  34/22 _____"_____
        .byte   $00     ;  35/23 _____#_____
        .byte   $00     ;  36/24 _____$_____
        .byte   $00     ;  37/25 _____%_____
        .byte   $00     ;  38/26 _____&_____
        .byte   $00     ;  39/27 _____'_____
        .byte   $00     ;  40/28 _____(_____
        .byte   $00     ;  41/29 _____)_____
        .byte   $00     ;  42/2a _____*_____
        .byte   $00     ;  43/2b _____+_____
        .byte   $00     ;  44/2c _____,_____
        .byte   $00     ;  45/2d _____-_____
        .byte   $00     ;  46/2e _____._____
        .byte   $00     ;  47/2f _____/_____
        .byte   $0C     ;  48/30 _____0_____
        .byte   $0C     ;  49/31 _____1_____
        .byte   $0C     ;  50/32 _____2_____
        .byte   $0C     ;  51/33 _____3_____
        .byte   $0C     ;  52/34 _____4_____
        .byte   $0C     ;  53/35 _____5_____
        .byte   $0C     ;  54/36 _____6_____
        .byte   $0C     ;  55/37 _____7_____
        .byte   $0C     ;  56/38 _____8_____
        .byte   $0C     ;  57/39 _____9_____
        .byte   $00     ;  58/3a _____:_____
        .byte   $00     ;  59/3b _____;_____
        .byte   $00     ;  60/3c _____<_____
        .byte   $00     ;  61/3d _____=_____
        .byte   $00     ;  62/3e _____>_____
        .byte   $00     ;  63/3f _____?_____

        .byte   $00     ;  64/40 _____@_____
        .byte   $0A     ;  65/41 _____A_____
        .byte   $0A     ;  66/42 _____B_____
        .byte   $0A     ;  67/43 _____C_____
        .byte   $0A     ;  68/44 _____D_____
        .byte   $0A     ;  69/45 _____E_____
        .byte   $0A     ;  70/46 _____F_____
        .byte   $02     ;  71/47 _____G_____
        .byte   $02     ;  72/48 _____H_____
        .byte   $02     ;  73/49 _____I_____
        .byte   $02     ;  74/4a _____J_____
        .byte   $02     ;  75/4b _____K_____
        .byte   $02     ;  76/4c _____L_____
        .byte   $02     ;  77/4d _____M_____
        .byte   $02     ;  78/4e _____N_____
        .byte   $02     ;  79/4f _____O_____
        .byte   $02     ;  80/50 _____P_____
        .byte   $02     ;  81/51 _____Q_____
        .byte   $02     ;  82/52 _____R_____
        .byte   $02     ;  83/53 _____S_____
        .byte   $02     ;  84/54 _____T_____
        .byte   $02     ;  85/55 _____U_____
        .byte   $02     ;  86/56 _____V_____
        .byte   $02     ;  87/57 _____W_____
        .byte   $02     ;  88/58 _____X_____
        .byte   $02     ;  89/59 _____Y_____
        .byte   $02     ;  90/5a _____Z_____
        .byte   $00     ;  91/5b _____[_____
        .byte   $00     ;  92/5c _____\_____
        .byte   $00     ;  93/5d _____]_____
        .byte   $00     ;  94/5e _____^_____
        .byte   $00     ;  95/5f _UNDERLINE_
        .byte   $00     ;  96/60 ___grave___
        .byte   $09     ;  97/61 _____a_____
        .byte   $09     ;  98/62 _____b_____
        .byte   $09     ;  99/63 _____c_____
        .byte   $09     ; 100/64 _____d_____
        .byte   $09     ; 101/65 _____e_____
        .byte   $09     ; 102/66 _____f_____
        .byte   $01     ; 103/67 _____g_____
        .byte   $01     ; 104/68 _____h_____
        .byte   $01     ; 105/69 _____i_____
        .byte   $01     ; 106/6a _____j_____
        .byte   $01     ; 107/6b _____k_____
        .byte   $01     ; 108/6c _____l_____
        .byte   $01     ; 109/6d _____m_____
        .byte   $01     ; 110/6e _____n_____
        .byte   $01     ; 111/6f _____o_____
        .byte   $01     ; 112/70 _____p_____
        .byte   $01     ; 113/71 _____q_____
        .byte   $01     ; 114/72 _____r_____
        .byte   $01     ; 115/73 _____s_____
        .byte   $01     ; 116/74 _____t_____
        .byte   $01     ; 117/75 _____u_____
        .byte   $01     ; 118/76 _____v_____
        .byte   $01     ; 119/77 _____w_____
        .byte   $01     ; 120/78 _____x_____
        .byte   $01     ; 121/79 _____y_____
        .byte   $01     ; 122/7a _____z_____
        .byte   $00     ; 123/7b _____{_____
        .byte   $00     ; 124/7c _____|_____
        .byte   $00     ; 125/7d _____}_____
        .byte   $00     ; 126/7e _____~_____
        .byte   $40     ; 127/7f ____DEL____

.endrepeat
