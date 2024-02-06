; ctype.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; Character specification table, matching serveral consoles.
;

        .include        "ctypetable.inc"
        .export         __ctypeidx

; The tables are readonly, put them into the rodata segment

.rodata

__ctypeidx:

.repeat 2       ; 2 times for normal and inverted

        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;   0/00 ___ctrl_@___,  1/01 ___ctrl_A___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;   2/02 ___ctrl_B___,  3/03 ___ctrl_C___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;   4/04 ___ctrl_D___,  5/05 ___ctrl_E___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;   6/06 ___ctrl_F___,  7/07 ___ctrl_G___
        ct_mix  CT_CTRL_IDX, CT_CTRL_WS_SPACETAB_IDX     ;   8/08 ___ctrl_H___,  9/09 ___ctrl_I___
        ct_mix  CT_CTRL_WS_IDX, CT_CTRL_WS_IDX           ;  10/0a ___ctrl_J___, 11/0b ___ctrl_K___
        ct_mix  CT_CTRL_WS_IDX, CT_CTRL_WS_IDX           ;  12/0c ___ctrl_L___, 13/0d ___ctrl_M___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;  14/0e ___ctrl_N___, 15/0f ___ctrl_O___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;  16/10 ___ctrl_P___, 17/11 ___ctrl_Q___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;  18/12 ___ctrl_R___, 19/13 ___ctrl_S___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;  20/14 ___ctrl_T___, 21/15 ___ctrl_U___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;  22/16 ___ctrl_V___, 23/17 ___ctrl_W___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;  24/18 ___ctrl_X___, 25/19 ___ctrl_Y___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;  26/1a ___ctrl_Z___, 27/1b ___ctrl_[___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;  28/1c ___ctrl_\___, 29/1d ___ctrl_]___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;  30/1e ___ctrl_^___, 31/1f ___ctrl_____

        ct_mix  CT_SPACE_SPACETAB_IDX, CT_NONE_IDX       ;  32/20 ___SPACE___,  33/21 _____!_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  34/22 _____"_____,  35/23 _____#_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  36/24 _____$_____,  37/25 _____%_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  38/26 _____&_____,  39/27 _____'_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  40/28 _____(_____,  41/29 _____)_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  42/2a _____*_____,  43/2b _____+_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  44/2c _____,_____,  45/2d _____-_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  46/2e _____._____,  47/2f _____/_____
        ct_mix  CT_DIGIT_XDIGIT_IDX, CT_DIGIT_XDIGIT_IDX ;  48/30 _____0_____,  49/31 _____1_____
        ct_mix  CT_DIGIT_XDIGIT_IDX, CT_DIGIT_XDIGIT_IDX ;  50/32 _____2_____,  51/33 _____3_____
        ct_mix  CT_DIGIT_XDIGIT_IDX, CT_DIGIT_XDIGIT_IDX ;  52/34 _____4_____,  53/35 _____5_____
        ct_mix  CT_DIGIT_XDIGIT_IDX, CT_DIGIT_XDIGIT_IDX ;  54/36 _____6_____,  55/37 _____7_____
        ct_mix  CT_DIGIT_XDIGIT_IDX, CT_DIGIT_XDIGIT_IDX ;  56/38 _____8_____,  57/39 _____9_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  58/3a _____:_____,  59/3b _____;_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  60/3c _____<_____,  61/3d _____=_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  62/3e _____>_____,  63/3f _____?_____

        ct_mix  CT_NONE_IDX, CT_UPPER_XDIGIT_IDX         ;  64/40 _____@_____,  65/41 _____A_____
        ct_mix  CT_UPPER_XDIGIT_IDX, CT_UPPER_XDIGIT_IDX ;  66/42 _____B_____,  67/43 _____C_____
        ct_mix  CT_UPPER_XDIGIT_IDX, CT_UPPER_XDIGIT_IDX ;  68/44 _____D_____,  69/45 _____E_____
        ct_mix  CT_UPPER_XDIGIT_IDX, CT_UPPER_IDX        ;  70/46 _____F_____,  71/47 _____G_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ;  72/48 _____H_____,  73/49 _____I_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ;  74/4a _____J_____,  75/4b _____K_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ;  76/4c _____L_____,  77/4d _____M_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ;  78/4e _____N_____,  79/4f _____O_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ;  80/50 _____P_____,  81/51 _____Q_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ;  82/52 _____R_____,  83/53 _____S_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ;  84/54 _____T_____,  85/55 _____U_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ;  86/56 _____V_____,  87/57 _____W_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ;  88/58 _____X_____,  89/59 _____Y_____
        ct_mix  CT_UPPER_IDX, CT_NONE_IDX                ;  90/5a _____Z_____,  91/5b _____[_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  92/5c _____\_____,  93/5d _____]_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  94/5e _____^_____,  95/5f _UNDERLINE_

        ct_mix  CT_NONE_IDX, CT_LOWER_XDIGIT_IDX         ;  96/60 ___grave___,  97/61 _____a_____
        ct_mix  CT_LOWER_XDIGIT_IDX, CT_LOWER_XDIGIT_IDX ;  98/62 _____b_____,  99/63 _____c_____
        ct_mix  CT_LOWER_XDIGIT_IDX, CT_LOWER_XDIGIT_IDX ; 100/64 _____d_____, 101/65 _____e_____
        ct_mix  CT_LOWER_XDIGIT_IDX, CT_LOWER_IDX        ; 102/66 _____f_____, 103/67 _____g_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 104/68 _____h_____, 105/69 _____i_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 106/6a _____j_____, 107/6b _____k_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 108/6c _____l_____, 109/6d _____m_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 110/6e _____n_____, 111/6f _____o_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 112/70 _____p_____, 113/71 _____q_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 114/72 _____r_____, 115/73 _____s_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 116/74 _____t_____, 117/75 _____u_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 118/76 _____v_____, 119/77 _____w_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 120/78 _____x_____, 121/79 _____y_____
        ct_mix  CT_LOWER_IDX, CT_NONE_IDX                ; 122/7a _____z_____, 123/7b _____{_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 124/7c _____|_____, 125/7d _____}_____
        ct_mix  CT_NONE_IDX, CT_WS_IDX                   ; 126/7e _____~_____, 127/7f ____DEL____

.endrepeat
