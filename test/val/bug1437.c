
/* bug #1437 enum declaration in a struct/union is invisible in the scope where the struct/union is declared */

struct nodelist1 {
    struct {
        enum { DEAD1, LIVE1, ONCE1, TWICE1 } live1;
    } s;
} firstnode1 = {ONCE1};

enum nodestate2 { DEAD2, LIVE2, ONCE2, TWICE2 } live2;

union nodelist2 {
    enum nodestate2 live2;
} firstnode2 = { {TWICE2} };

struct T {
    int I;
    int;
    enum E {
        I
    };
};

int failures = 0;

int main (void)
{
    if (firstnode1.s.live1 != ONCE1) {
        ++failures;
    }
    if (firstnode2.live2 != TWICE2) {
        ++failures;
    }
    return failures;
}
