/* Bug #2020 - ISO/IEC 9899:1999 (E), 6.9.1 footnote 137:
** "The intent is that the type category in a function definition cannot be inherited from a typedef"
 */

typedef void F(void);
F c { }                 /* Should fail */
