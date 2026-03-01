/* val_func.c: Test functions, scopes, and line information */

int add(int a, int b) {
    return a + b;
}

void nested_scopes(void) {
    int x = 1;
    {
        int y = 2;
        /* Nested scope */
        {
            int z = 3;
            x = x + y + z;
        }
    }
}

int main(void) {
    int result;
    
    result = add(10, 20);
    
    nested_scopes();
    
    return 0;
}
