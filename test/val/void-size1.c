/*
  !!DESCRIPTION!! Getting the size of a void-type variable (cc65 extension)
  !!ORIGIN!!      cc65 regression tests
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Greg King
*/

static const void list1 = {
    (char)1,
    (char)2,
    (char)3,
    (char)4,
    (char)5,
    (char)6,
    (char)7,
    (char)8,
    (char)9,
    (char)0
};

static void list2 = {
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    0
};

void list3 = {
    (char)1,
    (char)2,
    (char)3,
    (char)4,
    &list1,
    (char)6,
    (char)7,
    (char)8,
    (char)9,
    &list2
};

/* We know that the expression is constant; don't tell us. */

#pragma warn (const-comparison, off)

int main (void)
{
    return sizeof list1 != 10
        || sizeof list2 != 20
        || sizeof list3 != 12;
}
