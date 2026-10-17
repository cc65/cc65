/* val_types.c: Test C types, storage classes, and symbol information */

int global_int = 42;
static char static_char = 'A';
const char* const_string = "Hello";

enum Color {
    RED,
    GREEN,
    BLUE
};
enum Color my_color = GREEN;

struct Point {
    int x;
    int y;
};
struct Point p1 = { 10, 20 };

/* Forward declaration */
extern void external_func(void);

int main(void)
{
    /* Local variables (sc=auto, offs) */
    int local_int = 100;
    
    /* Register variable (sc=register) */
    register int i;

    /* Static local (sc=static) */
    static int calls = 0;

    for (i = 0; i < 10; ++i) {
        local_int += i;
    }
    calls++;
    static_char++;
    return 0;
}
