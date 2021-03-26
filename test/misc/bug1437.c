
/* bug #1437 enum declaration in struct does not compile */

struct nodelist1 {
    enum { DEAD1, LIVE1, ONCE1, TWICE1 } live1;
} firstnode1 = {ONCE1};

enum nodestate2 { DEAD2, LIVE2, ONCE2, TWICE2 } live2;

struct nodelist2 {
    enum nodestate2 live2;
} firstnode2 = {TWICE2};

int main (void)
{
    if (firstnode1.live1 != ONCE1) {
        return 1;
    }
    if (firstnode2.live2 != TWICE2) {
        return 1;
    }
    return 0;
}
