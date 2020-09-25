/* bug #1263 - erroneous error for K & R function declaration */

enum E { I = 0 };
extern int f(enum E);

int f(e)
  enum E e;
{
  return e;
}

int main(void)
{
    return f(I);
}
