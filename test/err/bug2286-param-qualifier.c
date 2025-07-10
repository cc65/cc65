/* Bug #2286 - Qualifiers of pointees of function parameters ignored for type compatibility check */

void woo(int* p);
void woo(const int* p); /* WRONG: Should be an error */
