/*
$ cl65 -Osir --codesize 180 -S -o main.s main.c
main.c(9): Internal compiler error:
Code generation messed up: StackPtr is -2, should be -4

Input: if (wcnt > btw) {

$ git bisect bad
aa6fdf58b8a17b747090fb521f3d9106e0c56d1c is the first bad commit
commit aa6fdf58b8a17b747090fb521f3d9106e0c56d1c
Author: acqn <acqn163@outlook.com>
Date:   Mon Feb 8 09:03:19 2021 +0800

    Addresses in constant subtraction expressions now work.
    Fixed codegen for cast type subtraction in constant expressions.
*/

unsigned long fptr = 0x40001;

int main(void)
{
    unsigned int btw = 500;
    unsigned int wcnt;

    wcnt = 512U - (fptr % 512U);

    if (wcnt > btw) {
         wcnt = btw;
    }

    return wcnt == 500 ? 0 : 1;
}

