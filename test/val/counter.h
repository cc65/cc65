/* Tests for predefined macro __COUNTER__ */

#line GET_COUNTER()  /* 1st: 8; 2nd: 9 */
int CONCAT(ident,GET_LINE()) = GET_LINE();
