#define DO_TEST(actual, expected) \
    if (actual != expected) { \
        return 1; \
    }

int main() {
    DO_TEST(1'2'3'4'5, 12345);
    DO_TEST(0x1'2'3'4, 0x1234);
    DO_TEST(0'1'2'3'4, 01234);
    DO_TEST(0b1'0'1'1'0'1'0'0, 0b10110100);

    return 0;
}