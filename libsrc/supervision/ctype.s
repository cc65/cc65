;
; Ullrich von Bassewitz, 2003-10-10
;
; Character specification table.
;

	.include	"ctype.inc"

; The tables are readonly, put them into the rodata segment

.rodata

; The following 256 byte wide table specifies attributes for the isxxx type
; of functions. Doing it by a table means some overhead in space, but it
; has major advantages:
;
;   * It is fast. If it weren't for the slow parameter passing of cc65, one
;     could even define macros for the isxxx functions (this is usually
;     done on other platforms).
;
;   * It is highly portable. The only unportable part is the table itself,
;     all real code goes into the common library.
;
;   * We save some code in the isxxx functions.


__ctype:
       	.byte  	CT_CTRL			;   0/00 ___ctrl_@___
	.byte	CT_CTRL			;   1/01 ___ctrl_A___
	.byte	CT_CTRL			;   2/02 ___ctrl_B___
	.byte	CT_CTRL			;   3/03 ___ctrl_C___
	.byte	CT_CTRL			;   4/04 ___ctrl_D___
	.byte	CT_CTRL			;   5/05 ___ctrl_E___
	.byte	CT_CTRL			;   6/06 ___ctrl_F___
	.byte	CT_CTRL			;   7/07 ___ctrl_G___
	.byte	CT_CTRL			;   8/08 ___ctrl_H___
	.byte	CT_CTRL | CT_OTHER_WS | CT_SPACE_TAB
					;   9/09 ___ctrl_I___
	.byte	CT_CTRL | CT_OTHER_WS	;  10/0a ___ctrl_J___
	.byte	CT_CTRL | CT_OTHER_WS	;  11/0b ___ctrl_K___
	.byte	CT_CTRL | CT_OTHER_WS	;  12/0c ___ctrl_L___
	.byte	CT_CTRL | CT_OTHER_WS	;  13/0d ___ctrl_M___
	.byte	CT_CTRL			;  14/0e ___ctrl_N___
	.byte	CT_CTRL			;  15/0f ___ctrl_O___
	.byte	CT_CTRL			;  16/10 ___ctrl_P___
	.byte	CT_CTRL			;  17/11 ___ctrl_Q___
	.byte	CT_CTRL			;  18/12 ___ctrl_R___
	.byte	CT_CTRL			;  19/13 ___ctrl_S___
	.byte	CT_CTRL			;  20/14 ___ctrl_T___
	.byte	CT_CTRL			;  21/15 ___ctrl_U___
	.byte	CT_CTRL			;  22/16 ___ctrl_V___
	.byte	CT_CTRL			;  23/17 ___ctrl_W___
	.byte	CT_CTRL			;  24/18 ___ctrl_X___
	.byte	CT_CTRL			;  25/19 ___ctrl_Y___
	.byte	CT_CTRL			;  26/1a ___ctrl_Z___
	.byte	CT_CTRL			;  27/1b ___ctrl_[___
	.byte	CT_CTRL			;  28/1c ___ctrl_\___
	.byte	CT_CTRL			;  29/1d ___ctrl_]___
	.byte	CT_CTRL			;  30/1e ___ctrl_^___
	.byte	CT_CTRL			;  31/1f ___ctrl_____
	.byte	CT_SPACE | CT_SPACE_TAB	;  32/20 ___SPACE___
       	.byte  	CT_NONE                 ;  33/21 _____!_____
       	.byte  	CT_NONE                 ;  34/22 _____"_____
       	.byte  	CT_NONE                 ;  35/23 _____#_____
       	.byte  	CT_NONE                 ;  36/24 _____$_____
       	.byte  	CT_NONE                 ;  37/25 _____%_____
       	.byte  	CT_NONE                 ;  38/26 _____&_____
       	.byte  	CT_NONE                 ;  39/27 _____'_____
       	.byte  	CT_NONE                 ;  40/28 _____(_____
       	.byte  	CT_NONE                 ;  41/29 _____)_____
       	.byte  	CT_NONE                 ;  42/2a _____*_____
       	.byte  	CT_NONE                 ;  43/2b _____+_____
       	.byte  	CT_NONE                 ;  44/2c _____,_____
       	.byte  	CT_NONE                 ;  45/2d _____-_____
       	.byte  	CT_NONE                 ;  46/2e _____._____
       	.byte  	CT_NONE                 ;  47/2f _____/_____
	.byte	CT_DIGIT | CT_XDIGIT	;  48/30 _____0_____
	.byte	CT_DIGIT | CT_XDIGIT	;  49/31 _____1_____
	.byte	CT_DIGIT | CT_XDIGIT	;  50/32 _____2_____
	.byte	CT_DIGIT | CT_XDIGIT	;  51/33 _____3_____
	.byte	CT_DIGIT | CT_XDIGIT	;  52/34 _____4_____
	.byte	CT_DIGIT | CT_XDIGIT	;  53/35 _____5_____
	.byte	CT_DIGIT | CT_XDIGIT	;  54/36 _____6_____
	.byte	CT_DIGIT | CT_XDIGIT	;  55/37 _____7_____
	.byte	CT_DIGIT | CT_XDIGIT	;  56/38 _____8_____
	.byte	CT_DIGIT | CT_XDIGIT	;  57/39 _____9_____
       	.byte  	CT_NONE                 ;  58/3a _____:_____
       	.byte  	CT_NONE                 ;  59/3b _____;_____
       	.byte  	CT_NONE                 ;  60/3c _____<_____
       	.byte  	CT_NONE                 ;  61/3d _____=_____
       	.byte  	CT_NONE                 ;  62/3e _____>_____
       	.byte  	CT_NONE                 ;  63/3f _____?_____

       	.byte  	CT_NONE                 ;  64/40 _____@_____
	.byte	CT_UPPER | CT_XDIGIT	;  65/41 _____A_____
	.byte	CT_UPPER | CT_XDIGIT	;  66/42 _____B_____
	.byte	CT_UPPER | CT_XDIGIT	;  67/43 _____C_____
	.byte	CT_UPPER | CT_XDIGIT	;  68/44 _____D_____
	.byte	CT_UPPER | CT_XDIGIT	;  69/45 _____E_____
	.byte	CT_UPPER | CT_XDIGIT	;  70/46 _____F_____
	.byte	CT_UPPER		;  71/47 _____G_____
	.byte	CT_UPPER		;  72/48 _____H_____
	.byte	CT_UPPER		;  73/49 _____I_____
	.byte	CT_UPPER		;  74/4a _____J_____
	.byte	CT_UPPER		;  75/4b _____K_____
	.byte	CT_UPPER		;  76/4c _____L_____
	.byte	CT_UPPER		;  77/4d _____M_____
	.byte	CT_UPPER		;  78/4e _____N_____
	.byte	CT_UPPER		;  79/4f _____O_____
	.byte	CT_UPPER		;  80/50 _____P_____
	.byte	CT_UPPER		;  81/51 _____Q_____
	.byte	CT_UPPER		;  82/52 _____R_____
	.byte	CT_UPPER		;  83/53 _____S_____
	.byte	CT_UPPER		;  84/54 _____T_____
	.byte	CT_UPPER		;  85/55 _____U_____
	.byte	CT_UPPER		;  86/56 _____V_____
	.byte	CT_UPPER		;  87/57 _____W_____
	.byte	CT_UPPER		;  88/58 _____X_____
	.byte	CT_UPPER		;  89/59 _____Y_____
	.byte	CT_UPPER		;  90/5a _____Z_____
       	.byte  	CT_NONE                 ;  91/5b _____[_____
       	.byte  	CT_NONE                 ;  92/5c _____\_____
       	.byte  	CT_NONE                 ;  93/5d _____]_____
       	.byte  	CT_NONE                 ;  94/5e _____^_____
       	.byte  	CT_NONE                 ;  95/5f _UNDERLINE_
       	.byte  	CT_NONE                 ;  96/60 ___grave___
	.byte	CT_LOWER | CT_XDIGIT	;  97/61 _____a_____
	.byte	CT_LOWER | CT_XDIGIT	;  98/62 _____b_____
	.byte	CT_LOWER | CT_XDIGIT	;  99/63 _____c_____
	.byte	CT_LOWER | CT_XDIGIT	; 100/64 _____d_____
	.byte	CT_LOWER | CT_XDIGIT	; 101/65 _____e_____
	.byte	CT_LOWER | CT_XDIGIT	; 102/66 _____f_____
	.byte	CT_LOWER		; 103/67 _____g_____
	.byte	CT_LOWER		; 104/68 _____h_____
	.byte	CT_LOWER		; 105/69 _____i_____
	.byte	CT_LOWER		; 106/6a _____j_____
	.byte	CT_LOWER		; 107/6b _____k_____
	.byte	CT_LOWER		; 108/6c _____l_____
	.byte	CT_LOWER		; 109/6d _____m_____
	.byte	CT_LOWER		; 110/6e _____n_____
	.byte	CT_LOWER		; 111/6f _____o_____
	.byte	CT_LOWER		; 112/70 _____p_____
	.byte	CT_LOWER		; 113/71 _____q_____
	.byte	CT_LOWER		; 114/72 _____r_____
	.byte	CT_LOWER		; 115/73 _____s_____
	.byte	CT_LOWER		; 116/74 _____t_____
	.byte	CT_LOWER		; 117/75 _____u_____
	.byte	CT_LOWER		; 118/76 _____v_____
	.byte	CT_LOWER		; 119/77 _____w_____
	.byte	CT_LOWER		; 120/78 _____x_____
	.byte	CT_LOWER		; 121/79 _____y_____
	.byte	CT_LOWER		; 122/7a _____z_____
       	.byte  	CT_NONE                 ; 123/7b _____{_____
       	.byte  	CT_NONE                 ; 124/7c _____|_____
       	.byte  	CT_NONE                 ; 125/7d _____}_____
       	.byte  	CT_NONE                 ; 126/7e _____~_____
	.byte	CT_OTHER_WS		; 127/7f ____DEL____

        .res    128, CT_NONE            ; 128-255



