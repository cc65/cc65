/* Bug #2020 - Right cases */

typedef int F(void);          // type F is "function with no parameters returning int"

F f, g;                       // f and g both have type compatible with F
int f(void) { return 0; }     // RIGHT: f has type compatible with F
int g() { return 0; }         // RIGHT: g has type compatible with F
F *e(void) { return 0; }      // e returns a pointer to a function
F *((h))(void) { return 0; }  // similar: parentheses irrelevant
int (*fp)(void);              // fp points to a function that has type F
F *Fp;                        // Fp points to a function that has type

int main(void)
{
    return 0;
}
