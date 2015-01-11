;
; Character specification table.
;
; Ullrich von Bassewitz, 02.06.1998
; 2003-05-02, Greg King
;
; Copied from cbm/ctype.s

; The following 256-byte-wide table specifies attributes for the isxxx type
; of functions.  Doing it by a table means some overhead in space, but it
; has major advantages:
;
;   * It is fast.  If it weren't for the slow parameter-passing of cc65,
;     one even could define C-language macroes for the isxxx functions
;     (as it usually is done, on other platforms).
;
;   * It is highly portable.  The only unportable part is the table itself;
;     all real code goes into the common library.
;
;   * We save some code in the isxxx functions.

; This table is taken from Craig S. Bruce's technical docs. for the ACE OS.

        .include        "ctype.inc"

; The table is read-only, put it into the RODATA segment.

        .rodata

__ctype:
        .byte   CT_CTRL                                 ;   0/00 ___rvs_@___
        .byte   CT_CTRL                                 ;   1/01 ___rvs_a___
        .byte   CT_CTRL                                 ;   2/02 ___rvs_b___
        .byte   CT_CTRL                                 ;   3/03 ___rvs_c___
        .byte   CT_CTRL                                 ;   4/04 ___rvs_d___
        .byte   CT_CTRL                                 ;   5/05 ___rvs_e___
        .byte   CT_CTRL                                 ;   6/06 ___rvs_f___
        .byte   CT_CTRL                                 ;   7/07 _BEL/rvs_g_
        .byte   CT_CTRL                                 ;   8/08 ___rvs_h___
        .byte   CT_CTRL | CT_OTHER_WS | CT_SPACE_TAB    ;   9/09 _TAB/rvs_i_
        .byte   CT_CTRL | CT_OTHER_WS                   ;  10/0a _BOL/rvs_j_
        .byte   CT_CTRL                                 ;  11/0b ___rvs_k___
        .byte   CT_CTRL                                 ;  12/0c ___rvs_l___
        .byte   CT_CTRL | CT_OTHER_WS                   ;  13/0d _CR_/rvs_m_
        .byte   CT_CTRL                                 ;  14/0e ___rvs_n___
        .byte   CT_CTRL                                 ;  15/0f ___rvs_o___
        .byte   CT_CTRL                                 ;  16/10 ___rvs_p___
        .byte   CT_CTRL | CT_OTHER_WS                   ;  17/11 _VT_/rvs_q_
        .byte   CT_CTRL                                 ;  18/12 ___rvs_r___
        .byte   CT_CTRL | CT_OTHER_WS                   ;  19/13 HOME/rvs_s_
        .byte   CT_CTRL | CT_OTHER_WS                   ;  20/14 _BS_/rvs_t_
        .byte   CT_CTRL                                 ;  21/15 ___rvs_u___
        .byte   CT_CTRL                                 ;  22/16 ___rvs_v___
        .byte   CT_CTRL                                 ;  23/17 ___rvs_w___
        .byte   CT_CTRL                                 ;  24/18 ___rvs_x___
        .byte   CT_CTRL                                 ;  25/19 ___rvs_y___
        .byte   CT_CTRL                                 ;  26/1a ___rvs_z___
        .byte   CT_CTRL                                 ;  27/1b ___rvs_[___
        .byte   CT_CTRL                                 ;  28/1c ___rvs_\___
        .byte   CT_CTRL | CT_OTHER_WS                   ;  29/1d cursr-right
        .byte   CT_CTRL                                 ;  30/1e ___rvs_^___
        .byte   CT_CTRL                                 ;  31/1f _rvs_under_
        .byte   CT_SPACE | CT_SPACE_TAB                 ;  32/20 ___SPACE___
        .byte   $00                                     ;  33/21 _____!_____
        .byte   $00                                     ;  34/22 _____"_____
        .byte   $00                                     ;  35/23 _____#_____
        .byte   $00                                     ;  36/24 _____$_____
        .byte   $00                                     ;  37/25 _____%_____
        .byte   $00                                     ;  38/26 _____&_____
        .byte   $00                                     ;  39/27 _____'_____
        .byte   $00                                     ;  40/28 _____(_____
        .byte   $00                                     ;  41/29 _____)_____
        .byte   $00                                     ;  42/2a _____*_____
        .byte   $00                                     ;  43/2b _____+_____
        .byte   $00                                     ;  44/2c _____,_____
        .byte   $00                                     ;  45/2d _____-_____
        .byte   $00                                     ;  46/2e _____._____
        .byte   $00                                     ;  47/2f _____/_____
        .byte   CT_DIGIT | CT_XDIGIT                    ;  48/30 _____0_____
        .byte   CT_DIGIT | CT_XDIGIT                    ;  49/31 _____1_____
        .byte   CT_DIGIT | CT_XDIGIT                    ;  50/32 _____2_____
        .byte   CT_DIGIT | CT_XDIGIT                    ;  51/33 _____3_____
        .byte   CT_DIGIT | CT_XDIGIT                    ;  52/34 _____4_____
        .byte   CT_DIGIT | CT_XDIGIT                    ;  53/35 _____5_____
        .byte   CT_DIGIT | CT_XDIGIT                    ;  54/36 _____6_____
        .byte   CT_DIGIT | CT_XDIGIT                    ;  55/37 _____7_____
        .byte   CT_DIGIT | CT_XDIGIT                    ;  56/38 _____8_____
        .byte   CT_DIGIT | CT_XDIGIT                    ;  57/39 _____9_____
        .byte   $00                                     ;  58/3a _____:_____
        .byte   $00                                     ;  59/3b _____;_____
        .byte   $00                                     ;  60/3c _____<_____
        .byte   $00                                     ;  61/3d _____=_____
        .byte   $00                                     ;  62/3e _____>_____
        .byte   $00                                     ;  63/3f _____?_____

        .byte   $00                                     ;  64/40 _____@_____
        .byte   CT_LOWER | CT_XDIGIT                    ;  65/41 _____a_____
        .byte   CT_LOWER | CT_XDIGIT                    ;  66/42 _____b_____
        .byte   CT_LOWER | CT_XDIGIT                    ;  67/43 _____c_____
        .byte   CT_LOWER | CT_XDIGIT                    ;  68/44 _____d_____
        .byte   CT_LOWER | CT_XDIGIT                    ;  69/45 _____e_____
        .byte   CT_LOWER | CT_XDIGIT                    ;  70/46 _____f_____
        .byte   CT_LOWER                                ;  71/47 _____g_____
        .byte   CT_LOWER                                ;  72/48 _____h_____
        .byte   CT_LOWER                                ;  73/49 _____i_____
        .byte   CT_LOWER                                ;  74/4a _____j_____
        .byte   CT_LOWER                                ;  75/4b _____k_____
        .byte   CT_LOWER                                ;  76/4c _____l_____
        .byte   CT_LOWER                                ;  77/4d _____m_____
        .byte   CT_LOWER                                ;  78/4e _____n_____
        .byte   CT_LOWER                                ;  79/4f _____o_____
        .byte   CT_LOWER                                ;  80/50 _____p_____
        .byte   CT_LOWER                                ;  81/51 _____q_____
        .byte   CT_LOWER                                ;  82/52 _____r_____
        .byte   CT_LOWER                                ;  83/53 _____s_____
        .byte   CT_LOWER                                ;  84/54 _____t_____
        .byte   CT_LOWER                                ;  85/55 _____u_____
        .byte   CT_LOWER                                ;  86/56 _____v_____
        .byte   CT_LOWER                                ;  87/57 _____w_____
        .byte   CT_LOWER                                ;  88/58 _____x_____
        .byte   CT_LOWER                                ;  89/59 _____y_____
        .byte   CT_LOWER                                ;  90/5a _____z_____
        .byte   $00                                     ;  91/5b _____[_____
        .byte   $00                                     ;  92/5c _____\_____
        .byte   $00                                     ;  93/5d _____]_____
        .byte   $00                                     ;  94/5e _____^_____
        .byte   $00                                     ;  95/5f _UNDERLINE_
        .byte   $00                                     ;  96/60 _A`_grave__
        .byte   $00                                     ;  97/61 _A'_acute__
        .byte   $00                                     ;  98/62 _A^_circum_
        .byte   $00                                     ;  99/63 _A~_tilde__
        .byte   $00                                     ; 100/64 _A"_dieres_
        .byte   $00                                     ; 101/65 _A__ring___
        .byte   $00                                     ; 102/66 _AE________
        .byte   $00                                     ; 103/67 _C,cedilla_
        .byte   $00                                     ; 104/68 _E`_grave__
        .byte   $00                                     ; 105/69 _E'_acute__
        .byte   $00                                     ; 106/6a _E^_circum_
        .byte   $00                                     ; 107/6b _E"_dieres_
        .byte   $00                                     ; 108/6c _I`_grave__
        .byte   $00                                     ; 109/6d _I'_acute__
        .byte   $00                                     ; 110/6e _I^_circum_
        .byte   $00                                     ; 111/6f _I"_dieres_
        .byte   $00                                     ; 112/70 _D-_Eth_lr_
        .byte   $00                                     ; 113/71 _N~_tilde__
        .byte   $00                                     ; 114/72 _O`_grave__
        .byte   $00                                     ; 115/73 _O'_acute__
        .byte   $00                                     ; 116/74 _O^_circum_
        .byte   $00                                     ; 117/75 _O~_tilde__
        .byte   $00                                     ; 118/76 _O"_dieres_
        .byte   $00                                     ; 119/77 __multiply_
        .byte   $00                                     ; 120/78 _O/_slash__
        .byte   $00                                     ; 121/79 _U`_grave__
        .byte   $00                                     ; 122/7a _U'_acute__
        .byte   $00                                     ; 123/7b _U^_circum_
        .byte   $00                                     ; 124/7c _U"_dieres_
        .byte   $00                                     ; 125/7d _Y'_acute__
        .byte   $00                                     ; 126/7e _cap_thorn_
        .byte   $00                                     ; 127/7f _Es-sed_B__

        .byte   CT_CTRL                                 ; 128/80 __bullet___
        .byte   CT_CTRL                                 ; 129/81 __v_line___
        .byte   CT_CTRL                                 ; 130/82 __h_line___
        .byte   CT_CTRL                                 ; 131/83 ___cross___
        .byte   CT_CTRL                                 ; 132/84 _tl_corner_
        .byte   CT_CTRL                                 ; 133/85 _tr_corner_
        .byte   CT_CTRL                                 ; 134/86 _bl_corner_
        .byte   CT_CTRL                                 ; 135/87 _br_corner_
        .byte   CT_CTRL                                 ; 136/88 ___l_tee___
        .byte   CT_CTRL                                 ; 137/89 ___r_tee___
        .byte   CT_CTRL                                 ; 138/8a ___t_tee___
        .byte   CT_CTRL                                 ; 139/8b ___b_tee___
        .byte   CT_CTRL                                 ; 140/8c ___heart___
        .byte   CT_CTRL | CT_OTHER_WS                   ; 141/8d _CR/diamond
        .byte   CT_CTRL                                 ; 142/8e ___club____
        .byte   CT_CTRL                                 ; 143/8f ___spade___
        .byte   CT_CTRL                                 ; 144/90 _s_circle__
        .byte   CT_CTRL | CT_OTHER_WS                   ; 145/91 _cursor-up_
        .byte   CT_CTRL                                 ; 146/92 ___pound___
        .byte   CT_CTRL | CT_OTHER_WS                   ; 147/93 _CLS/check_
        .byte   CT_CTRL | CT_OTHER_WS                   ; 148/94 __INSert___
        .byte   CT_CTRL                                 ; 149/95 ____+/-____
        .byte   CT_CTRL                                 ; 150/96 __divide___
        .byte   CT_CTRL                                 ; 151/97 __degree___
        .byte   CT_CTRL                                 ; 152/98 _c_checker_
        .byte   CT_CTRL                                 ; 153/99 _f_checker_
        .byte   CT_CTRL                                 ; 154/9a _solid_sq__
        .byte   CT_CTRL                                 ; 155/9b __cr_char__
        .byte   CT_CTRL                                 ; 156/9c _up_arrow__
        .byte   CT_CTRL | CT_OTHER_WS                   ; 157/9d cursor-left
        .byte   CT_CTRL                                 ; 158/9e _left_arro_
        .byte   CT_CTRL                                 ; 159/9f _right_arr_
        .byte   CT_SPACE | CT_SPACE_TAB                 ; 160/a0 _req space_
        .byte   $00                                     ; 161/a1 _!_invertd_
        .byte   $00                                     ; 162/a2 ___cent____
        .byte   $00                                     ; 163/a3 ___pound___
        .byte   $00                                     ; 164/a4 __currency_
        .byte   $00                                     ; 165/a5 ____yen____
        .byte   $00                                     ; 166/a6 _|_broken__
        .byte   $00                                     ; 167/a7 __section__
        .byte   $00                                     ; 168/a8 __umulaut__
        .byte   $00                                     ; 169/a9 _copyright_
        .byte   $00                                     ; 170/aa __fem_ord__
        .byte   $00                                     ; 171/ab _l_ang_quo_
        .byte   $00                                     ; 172/ac ____not____
        .byte   $00                                     ; 173/ad _syl_hyphn_
        .byte   $00                                     ; 174/ae _registerd_
        .byte   $00                                     ; 175/af _overline__
        .byte   $00                                     ; 176/b0 __degrees__
        .byte   $00                                     ; 177/b1 ____+/-____
        .byte   $00                                     ; 178/b2 _2_supersc_
        .byte   $00                                     ; 179/b3 _3_supersc_
        .byte   $00                                     ; 180/b4 ___acute___
        .byte   $00                                     ; 181/b5 ____mu_____
        .byte   $00                                     ; 182/b6 _paragraph_
        .byte   $00                                     ; 183/b7 __mid_dot__
        .byte   $00                                     ; 184/b8 __cedilla__
        .byte   $00                                     ; 185/b9 _1_supersc_
        .byte   $00                                     ; 186/ba __mas_ord__
        .byte   $00                                     ; 187/bb _r_ang_quo_
        .byte   $00                                     ; 188/bc ____1/4____
        .byte   $00                                     ; 189/bd ____1/2____
        .byte   $00                                     ; 190/be ____3/4____
        .byte   $00                                     ; 191/bf _?_invertd_

        .byte   $00                                     ; 192/c0 _____`_____
        .byte   CT_UPPER | CT_XDIGIT                    ; 193/c1 _____A_____
        .byte   CT_UPPER | CT_XDIGIT                    ; 194/c2 _____B_____
        .byte   CT_UPPER | CT_XDIGIT                    ; 195/c3 _____C_____
        .byte   CT_UPPER | CT_XDIGIT                    ; 196/c4 _____D_____
        .byte   CT_UPPER | CT_XDIGIT                    ; 197/c5 _____E_____
        .byte   CT_UPPER | CT_XDIGIT                    ; 198/c6 _____F_____
        .byte   CT_UPPER                                ; 199/c7 _____G_____
        .byte   CT_UPPER                                ; 200/c8 _____H_____
        .byte   CT_UPPER                                ; 201/c9 _____I_____
        .byte   CT_UPPER                                ; 202/ca _____J_____
        .byte   CT_UPPER                                ; 203/cb _____K_____
        .byte   CT_UPPER                                ; 204/cc _____L_____
        .byte   CT_UPPER                                ; 205/cd _____M_____
        .byte   CT_UPPER                                ; 206/ce _____N_____
        .byte   CT_UPPER                                ; 207/cf _____O_____
        .byte   CT_UPPER                                ; 208/d0 _____P_____
        .byte   CT_UPPER                                ; 209/d1 _____Q_____
        .byte   CT_UPPER                                ; 210/d2 _____R_____
        .byte   CT_UPPER                                ; 211/d3 _____S_____
        .byte   CT_UPPER                                ; 212/d4 _____T_____
        .byte   CT_UPPER                                ; 213/d5 _____U_____
        .byte   CT_UPPER                                ; 214/d6 _____V_____
        .byte   CT_UPPER                                ; 215/d7 _____W_____
        .byte   CT_UPPER                                ; 216/d8 _____X_____
        .byte   CT_UPPER                                ; 217/d9 _____Y_____
        .byte   CT_UPPER                                ; 218/da _____Z_____
        .byte   $00                                     ; 219/db _____{_____
        .byte   $00                                     ; 220/dc _____|_____
        .byte   $00                                     ; 221/dd _____}_____
        .byte   $00                                     ; 222/de _____~_____
        .byte   $00                                     ; 223/df ___HOUSE___
        .byte   $00                                     ; 224/e0 _a`_grave__
        .byte   $00                                     ; 225/e1 _a'_acute__
        .byte   $00                                     ; 226/e2 _a^_circum_
        .byte   $00                                     ; 227/e3 _a~_tilde__
        .byte   $00                                     ; 228/e4 _a"_dieres_
        .byte   $00                                     ; 229/e5 _a__ring___
        .byte   $00                                     ; 230/e6 _ae________
        .byte   $00                                     ; 231/e7 _c,cedilla_
        .byte   $00                                     ; 232/e8 _e`_grave__
        .byte   $00                                     ; 233/e9 _e'_acute__
        .byte   $00                                     ; 234/ea _e^_circum_
        .byte   $00                                     ; 235/eb _e"_dieres_
        .byte   $00                                     ; 236/ec _i`_grave__
        .byte   $00                                     ; 237/ed _i'_acute__
        .byte   $00                                     ; 238/ee _i^_circum_
        .byte   $00                                     ; 239/ef _i"_dieres_
        .byte   $00                                     ; 240/f0 _o^x_Eth_s_
        .byte   $00                                     ; 241/f1 _n~_tilda__
        .byte   $00                                     ; 242/f2 _o`_grave__
        .byte   $00                                     ; 243/f3 _o'_acute__
        .byte   $00                                     ; 244/f4 _o^_circum_
        .byte   $00                                     ; 245/f5 _o~_tilde__
        .byte   $00                                     ; 246/f6 _o"_dieres_
        .byte   $00                                     ; 247/f7 __divide___
        .byte   $00                                     ; 248/f8 _o/_slash__
        .byte   $00                                     ; 249/f9 _u`_grave__
        .byte   $00                                     ; 250/fa _u'_acute__
        .byte   $00                                     ; 251/fb _u^_circum_
        .byte   $00                                     ; 252/fc _u"_dieres_
        .byte   $00                                     ; 253/fd _y'_acute__
        .byte   $00                                     ; 254/fe _sm_thorn__
        .byte   $00                                     ; 255/ff _y"_dieres_
