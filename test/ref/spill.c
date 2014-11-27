/*
  !!DESCRIPTION!! register spilling
  !!ORIGIN!!      LCC 4.1 Testsuite
  !!LICENCE!!     own, freely distributeable for non-profit. read CPYRIGHT.LCC
*/

#include "common.h"
#include <stdio.h>

int main(void)
{
    printf("disassemble this program to check the generated code.\n");
    return 0;
}

#ifdef NO_EMPTY_FUNC_ARGS
        f(i){return i+i;}
        f2(i){return f(i)+(i?f(i):1);}
        f3(int i,int *p){
        register r1=0,r2=0,r3=0,r4=0,r5=0,r6=0,r7=0,r8=0,r9=0,r10=0;
                *p++=i?f(i):0;
        }
#else
        f(i){i=f()+f();}
        f2(i){i=f()+(i?f():1);}
        f3(int i,int *p){
        register r1=0,r2=0,r3=0,r4=0,r5=0,r6=0,r7=0,r8=0,r9=0,r10=0;
                *p++=i?f():0;
        }
#endif

#ifdef NO_FLOATS
        signed a[10],b[10];
#else
        double a[10],b[10];
#endif

int i;

f4(){
register r6=0,r7=0,r8=0,r9=0,r10=0,r11=0;
        i=a[i]+b[i] && i && a[i]-b[i];
}
/* f4 causes parent to spill child on vax when odd double regs are enabled */

int j, k, m, n;
#ifdef NO_FLOATS
        signed *A, *B, x;
#else
        double *A, *B, x;
#endif

f5(){
	x=A[k*m]*A[j*m]+B[k*n]*B[j*n];
	x=A[k*m]*B[j*n]-B[k*n]*A[j*m];
}
