#define N 25

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

int mem[N+1] = {1,1,1};

int fibonacci(int i) {
    int fibominus1, fibominus2;
    if(mem[i] != 0){
        return mem[i];
    }
    fibominus1 = fibonacci(i - 1);
    fibominus2 = fibonacci(i - 2);
    mem[i] = fibominus1 + fibominus2;
   return mem[i];
}

int main() {
    printf("%d\t\n", fibonacci(N));
    return 0;
}
