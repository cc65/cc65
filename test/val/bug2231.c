_Pragma("message/*Comment1*/ ( /*Comment2*/\"test message\" /*Comment3*/)")
/* We have no pragma without parenthesis but if there would be one, the
** following should also work:
*/
/* _Pragma("once// Comment") */

int main(void)
{
    return 0;
}
