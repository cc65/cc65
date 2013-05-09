;
; Source: The Hitchhiker's Guide To GEOS
; http://lyonlabs.org/commodore/onrequest/geos-manuals/The_Hitchhikers_Guide_to_GEOS.pdf
;
; Character specification table.
;

        .include "ctype.inc"

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

__ctype:
        .byte CT_CTRL           ;                         0/00 ____NULL___
        .byte CT_CTRL           ;                         1/01 ____N/A____
        .byte CT_CTRL           ;                         2/02 ____N/A____
        .byte CT_CTRL           ;                         3/03 ____N/A____
        .byte CT_CTRL           ;                         4/04 ____N/A____
        .byte CT_CTRL           ;                         5/05 ____N/A____
        .byte CT_CTRL           ;                         6/06 ____N/A____
        .byte CT_CTRL           ;                         7/07 ____N/A____
        .byte CT_CTRL           ;                         8/08 __BAKSPACE_
        .byte CT_CTRL | CT_OTHER_WS | CT_SPACE_TAB ;      9/09 __FWDSPACE_
        .byte CT_CTRL | CT_OTHER_WS ;                    10/0a _____LF____
        .byte CT_CTRL           ;                        11/0b ____HOME___
        .byte CT_CTRL           ;                        12/0c ___UPLINE__
        .byte CT_CTRL | CT_OTHER_WS ;                    13/0d _____CR____
        .byte CT_CTRL           ;                        14/0e __ULINEON__
        .byte CT_CTRL           ;                        15/0f __ULINEOFF_
        .byte CT_CTRL           ;                        16/10 _ESC_GRAPH_
        .byte CT_CTRL           ;                        17/11 ____N/A____
        .byte CT_CTRL           ;                        18/12 ___REVON___
        .byte CT_CTRL           ;                        19/13 ___REVOFF__
        .byte CT_CTRL           ;                        20/14 ___GOTOX___
        .byte CT_CTRL           ;                        21/15 ___GOTOY___
        .byte CT_CTRL           ;                        22/16 ___GOTOXY__
        .byte CT_CTRL           ;                        23/17 _NEWCRDSET_
        .byte CT_CTRL           ;                        24/18 ___BOLDON__
        .byte CT_CTRL           ;                        25/19 __ITALICON_
        .byte CT_CTRL           ;                        26/1a _OUTLINEON_
        .byte CT_CTRL           ;                        27/1b _PLAINTEXT_
        .byte CT_CTRL           ;                        28/1c ____N/A____
        .byte CT_CTRL           ;                        29/1d ____N/A____
        .byte CT_CTRL           ;                        30/1e ____N/A____
        .byte CT_CTRL           ;                        31/1f ____N/A____
        .byte CT_SPACE_TAB | CT_SPACE ;                  32/20 ___SPACE___
        .byte CT_NONE           ;                        33/21 _____!_____
        .byte CT_NONE           ;                        34/22 _____"_____
        .byte CT_NONE           ;                        35/23 _____#_____
        .byte CT_NONE           ;                        36/24 _____$_____
        .byte CT_NONE           ;                        37/25 _____%_____
        .byte CT_NONE           ;                        38/26 _____&_____
        .byte CT_NONE           ;                        39/27 _____'_____
        .byte CT_NONE           ;                        40/28 _____(_____
        .byte CT_NONE           ;                        41/29 _____)_____
        .byte CT_NONE           ;                        42/2a _____*_____
        .byte CT_NONE           ;                        43/2b _____+_____
        .byte CT_NONE           ;                        44/2c _____,_____
        .byte CT_NONE           ;                        45/2d _____-_____
        .byte CT_NONE           ;                        46/2e _____._____
        .byte CT_NONE           ;                        47/2f _____/_____
        .byte CT_DIGIT | CT_XDIGIT ;                     48/30 _____0_____
        .byte CT_DIGIT | CT_XDIGIT ;                     49/31 _____1_____
        .byte CT_DIGIT | CT_XDIGIT ;                     50/32 _____2_____
        .byte CT_DIGIT | CT_XDIGIT ;                     51/33 _____3_____
        .byte CT_DIGIT | CT_XDIGIT ;                     52/34 _____4_____
        .byte CT_DIGIT | CT_XDIGIT ;                     53/35 _____5_____
        .byte CT_DIGIT | CT_XDIGIT ;                     54/36 _____6_____
        .byte CT_DIGIT | CT_XDIGIT ;                     55/37 _____7_____
        .byte CT_DIGIT | CT_XDIGIT ;                     56/38 _____8_____
        .byte CT_DIGIT | CT_XDIGIT ;                     57/39 _____9_____
        .byte CT_NONE           ;                        58/3a _____:_____
        .byte CT_NONE           ;                        59/3b _____;_____
        .byte CT_NONE           ;                        60/3c _____<_____
        .byte CT_NONE           ;                        61/3d _____=_____
        .byte CT_NONE           ;                        62/3e _____>_____
        .byte CT_NONE           ;                        63/3f _____?_____
        .byte CT_NONE           ;                        64/40 _____@_____
        .byte CT_UPPER | CT_XDIGIT ;                     65/41 _____A_____
        .byte CT_UPPER | CT_XDIGIT ;                     66/42 _____B_____
        .byte CT_UPPER | CT_XDIGIT ;                     67/43 _____C_____
        .byte CT_UPPER | CT_XDIGIT ;                     68/44 _____D_____
        .byte CT_UPPER | CT_XDIGIT ;                     69/45 _____E_____
        .byte CT_UPPER | CT_XDIGIT ;                     70/46 _____F_____
        .byte CT_UPPER          ;                        71/47 _____G_____
        .byte CT_UPPER          ;                        72/48 _____H_____
        .byte CT_UPPER          ;                        73/49 _____I_____
        .byte CT_UPPER          ;                        74/4a _____J_____
        .byte CT_UPPER          ;                        75/4b _____K_____
        .byte CT_UPPER          ;                        76/4c _____L_____
        .byte CT_UPPER          ;                        77/4d _____M_____
        .byte CT_UPPER          ;                        78/4e _____N_____
        .byte CT_UPPER          ;                        79/4f _____O_____
        .byte CT_UPPER          ;                        80/50 _____P_____
        .byte CT_UPPER          ;                        81/51 _____Q_____
        .byte CT_UPPER          ;                        82/52 _____R_____
        .byte CT_UPPER          ;                        83/53 _____S_____
        .byte CT_UPPER          ;                        84/54 _____T_____
        .byte CT_UPPER          ;                        85/55 _____U_____
        .byte CT_UPPER          ;                        86/56 _____V_____
        .byte CT_UPPER          ;                        87/57 _____W_____
        .byte CT_UPPER          ;                        88/58 _____X_____
        .byte CT_UPPER          ;                        89/59 _____Y_____
        .byte CT_UPPER          ;                        90/5a _____Z_____
        .byte CT_NONE           ;                        91/5b _____[_____
        .byte CT_NONE           ;                        92/5c _____\_____
        .byte CT_NONE           ;                        93/5d _____]_____
        .byte CT_NONE           ;                        94/5e _____^_____
        .byte CT_NONE           ;                        95/5f _UNDERLINE_
        .byte CT_NONE           ;                        96/60 _____`_____
        .byte CT_LOWER | CT_XDIGIT ;                     97/61 _____a_____
        .byte CT_LOWER | CT_XDIGIT ;                     98/62 _____b_____
        .byte CT_LOWER | CT_XDIGIT ;                     99/63 _____c_____
        .byte CT_LOWER | CT_XDIGIT ;                    100/64 _____d_____
        .byte CT_LOWER | CT_XDIGIT ;                    101/65 _____e_____
        .byte CT_LOWER | CT_XDIGIT ;                    102/66 _____f_____
        .byte CT_LOWER          ;                       103/67 _____g_____
        .byte CT_LOWER          ;                       104/68 _____h_____
        .byte CT_LOWER          ;                       105/69 _____i_____
        .byte CT_LOWER          ;                       106/6a _____j_____
        .byte CT_LOWER          ;                       107/6b _____k_____
        .byte CT_LOWER          ;                       108/6c _____l_____
        .byte CT_LOWER          ;                       109/6d _____m_____
        .byte CT_LOWER          ;                       110/6e _____n_____
        .byte CT_LOWER          ;                       111/6f _____o_____
        .byte CT_LOWER          ;                       112/70 _____p_____
        .byte CT_LOWER          ;                       113/71 _____q_____
        .byte CT_LOWER          ;                       114/72 _____r_____
        .byte CT_LOWER          ;                       115/73 _____s_____
        .byte CT_LOWER          ;                       116/74 _____t_____
        .byte CT_LOWER          ;                       117/75 _____u_____
        .byte CT_LOWER          ;                       118/76 _____v_____
        .byte CT_LOWER          ;                       119/77 _____w_____
        .byte CT_LOWER          ;                       120/78 _____x_____
        .byte CT_LOWER          ;                       121/79 _____y_____
        .byte CT_LOWER          ;                       122/7a _____z_____
        .byte CT_NONE           ;                       123/7b _____{_____
        .byte CT_NONE           ;                       124/7c _____|_____
        .byte CT_NONE           ;                       125/7d _____}_____
        .byte CT_NONE           ;                       126/7e _____~_____
        .byte CT_CTRL           ;                       127/7f __USELAST__
        .byte CT_NONE           ;                       128/80 __SHORTCUT_
        .byte CT_NONE           ;                       129/81 ____N/A____
        .byte CT_NONE           ;                       130/82 ____N/A____
        .byte CT_NONE           ;                       131/83 ____N/A____
        .byte CT_NONE           ;                       132/84 ____N/A____
        .byte CT_NONE           ;                       133/85 ____N/A____
        .byte CT_NONE           ;                       134/86 ____N/A____
        .byte CT_NONE           ;                       135/87 ____N/A____
        .byte CT_NONE           ;                       136/88 ____N/A____
        .byte CT_NONE           ;                       137/89 ____N/A____
        .byte CT_NONE           ;                       138/8a ____N/A____
        .byte CT_NONE           ;                       139/8b ____N/A____
        .byte CT_NONE           ;                       140/8c ____N/A____
        .byte CT_NONE           ;                       141/8d ____N/A____
        .byte CT_NONE           ;                       142/8e ____N/A____
        .byte CT_NONE           ;                       143/8f ____N/A____
        .byte CT_NONE           ;                       144/90 ____N/A____
        .byte CT_NONE           ;                       145/91 ____N/A____
        .byte CT_NONE           ;                       146/92 ____N/A____
        .byte CT_NONE           ;                       147/93 ____N/A____
        .byte CT_NONE           ;                       148/94 ____N/A____
        .byte CT_NONE           ;                       149/95 ____N/A____
        .byte CT_NONE           ;                       150/96 ____N/A____
        .byte CT_NONE           ;                       151/97 ____N/A____
        .byte CT_NONE           ;                       152/98 ____N/A____
        .byte CT_NONE           ;                       153/99 ____N/A____
        .byte CT_NONE           ;                       154/9a ____N/A____
        .byte CT_NONE           ;                       155/9b ____N/A____
        .byte CT_NONE           ;                       156/9c ____N/A____
        .byte CT_NONE           ;                       157/9d ____N/A____
        .byte CT_NONE           ;                       158/9e ____N/A____
        .byte CT_NONE           ;                       159/9f ____N/A____
        .byte CT_NONE           ;                       160/a0 ____N/A____
        .byte CT_NONE           ;                       161/a1 ____N/A____
        .byte CT_NONE           ;                       162/a2 ____N/A____
        .byte CT_NONE           ;                       163/a3 ____N/A____
        .byte CT_NONE           ;                       164/a4 ____N/A____
        .byte CT_NONE           ;                       165/a5 ____N/A____
        .byte CT_NONE           ;                       166/a6 ____N/A____
        .byte CT_NONE           ;                       167/a7 ____N/A____
        .byte CT_NONE           ;                       168/a8 ____N/A____
        .byte CT_NONE           ;                       169/a9 ____N/A____
        .byte CT_NONE           ;                       170/aa ____N/A____
        .byte CT_NONE           ;                       171/ab ____N/A____
        .byte CT_NONE           ;                       172/ac ____N/A____
        .byte CT_NONE           ;                       173/ad ____N/A____
        .byte CT_NONE           ;                       174/ae ____N/A____
        .byte CT_NONE           ;                       175/af ____N/A____
        .byte CT_NONE           ;                       176/b0 ____N/A____
        .byte CT_NONE           ;                       177/b1 ____N/A____
        .byte CT_NONE           ;                       178/b2 ____N/A____
        .byte CT_NONE           ;                       179/b3 ____N/A____
        .byte CT_NONE           ;                       180/b4 ____N/A____
        .byte CT_NONE           ;                       181/b5 ____N/A____
        .byte CT_NONE           ;                       182/b6 ____N/A____
        .byte CT_NONE           ;                       183/b7 ____N/A____
        .byte CT_NONE           ;                       184/b8 ____N/A____
        .byte CT_NONE           ;                       185/b9 ____N/A____
        .byte CT_NONE           ;                       186/ba ____N/A____
        .byte CT_NONE           ;                       187/bb ____N/A____
        .byte CT_NONE           ;                       188/bc ____N/A____
        .byte CT_NONE           ;                       189/bd ____N/A____
        .byte CT_NONE           ;                       190/be ____N/A____
        .byte CT_NONE           ;                       191/bf ____N/A____
        .byte CT_NONE           ;                       192/c0 ____N/A____
        .byte CT_NONE           ;                       193/c1 ____N/A____
        .byte CT_NONE           ;                       194/c2 ____N/A____
        .byte CT_NONE           ;                       195/c3 ____N/A____
        .byte CT_NONE           ;                       196/c4 ____N/A____
        .byte CT_NONE           ;                       197/c5 ____N/A____
        .byte CT_NONE           ;                       198/c6 ____N/A____
        .byte CT_NONE           ;                       199/c7 ____N/A____
        .byte CT_NONE           ;                       200/c8 ____N/A____
        .byte CT_NONE           ;                       201/c9 ____N/A____
        .byte CT_NONE           ;                       202/ca ____N/A____
        .byte CT_NONE           ;                       203/cb ____N/A____
        .byte CT_NONE           ;                       204/cc ____N/A____
        .byte CT_NONE           ;                       205/cd ____N/A____
        .byte CT_NONE           ;                       206/ce ____N/A____
        .byte CT_NONE           ;                       207/cf ____N/A____
        .byte CT_NONE           ;                       208/d0 ____N/A____
        .byte CT_NONE           ;                       209/d1 ____N/A____
        .byte CT_NONE           ;                       210/d2 ____N/A____
        .byte CT_NONE           ;                       211/d3 ____N/A____
        .byte CT_NONE           ;                       212/d4 ____N/A____
        .byte CT_NONE           ;                       213/d5 ____N/A____
        .byte CT_NONE           ;                       214/d6 ____N/A____
        .byte CT_NONE           ;                       215/d7 ____N/A____
        .byte CT_NONE           ;                       216/d8 ____N/A____
        .byte CT_NONE           ;                       217/d9 ____N/A____
        .byte CT_NONE           ;                       218/da ____N/A____
        .byte CT_NONE           ;                       219/db ____N/A____
        .byte CT_NONE           ;                       220/dc ____N/A____
        .byte CT_NONE           ;                       221/dd ____N/A____
        .byte CT_NONE           ;                       222/de ____N/A____
        .byte CT_NONE           ;                       223/df ____N/A____
        .byte CT_NONE           ;                       224/e0 ____N/A____
        .byte CT_NONE           ;                       225/e1 ____N/A____
        .byte CT_NONE           ;                       226/e2 ____N/A____
        .byte CT_NONE           ;                       227/e3 ____N/A____
        .byte CT_NONE           ;                       228/e4 ____N/A____
        .byte CT_NONE           ;                       229/e5 ____N/A____
        .byte CT_NONE           ;                       230/e6 ____N/A____
        .byte CT_NONE           ;                       231/e7 ____N/A____
        .byte CT_NONE           ;                       232/e8 ____N/A____
        .byte CT_NONE           ;                       233/e9 ____N/A____
        .byte CT_NONE           ;                       234/ea ____N/A____
        .byte CT_NONE           ;                       235/eb ____N/A____
        .byte CT_NONE           ;                       236/ec ____N/A____
        .byte CT_NONE           ;                       237/ed ____N/A____
        .byte CT_NONE           ;                       238/ee ____N/A____
        .byte CT_NONE           ;                       239/ef ____N/A____
        .byte CT_NONE           ;                       240/f0 ____N/A____
        .byte CT_NONE           ;                       241/f1 ____N/A____
        .byte CT_NONE           ;                       242/f2 ____N/A____
        .byte CT_NONE           ;                       243/f3 ____N/A____
        .byte CT_NONE           ;                       244/f4 ____N/A____
        .byte CT_NONE           ;                       245/f5 ____N/A____
        .byte CT_NONE           ;                       246/f6 ____N/A____
        .byte CT_NONE           ;                       247/f7 ____N/A____
        .byte CT_NONE           ;                       248/f8 ____N/A____
        .byte CT_NONE           ;                       249/f9 ____N/A____
        .byte CT_NONE           ;                       250/fa ____N/A____
        .byte CT_NONE           ;                       251/fb ____N/A____
        .byte CT_NONE           ;                       252/fc ____N/A____
        .byte CT_NONE           ;                       253/fd ____N/A____
        .byte CT_NONE           ;                       254/fe ____N/A____
        .byte CT_NONE           ;                       255/ff ____N/A____
