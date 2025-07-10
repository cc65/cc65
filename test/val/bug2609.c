/* Bug #2609 - charmap translation violates C specification 6.4.4.4 Character constant */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#pragma charmap (0x07, 0x62) /* map \a to b */
static_assert('\a' == 0x62);
static_assert('\07' == 0x07);
static_assert('\x07' == 0x07);

#pragma charmap (0x07, 0x63) /* map \a to c */
static_assert('\a' == 0x63);
static_assert('\07' == 0x07);
static_assert('\x07' == 0x07);

#pragma charmap (0x07, 0x07) /* map \a back to x07 */
static_assert('\a' == 0x07);
static_assert('\07' == 0x07);
static_assert('\x07' == 0x07);

#pragma charmap (0x07, 0x61) /* map \a to a */

char *s = "\07\a\x07";
char t[] = { 7, 0x61, 7, 0 };

static_assert('\a' == 0x61);
static_assert('\07' == 0x07);
static_assert('\x07' == 0x07);

char c_back_a = '\a';
char c_hex_07 = '\x07';
char c_oct_07 = '\07';
int  i_back_a = '\a';
int  i_hex_07 = '\x07';
int  i_oct_07 = '\07';

#define TEST(a,b) \
    if (a != b) { printf("\n\n !FAIL! %s = %04x not %04x\n\n", #a, a, b); return EXIT_FAILURE; }

int main (void) {
    int i;

    TEST(c_back_a, 0x61)
    TEST(c_hex_07, 0x07)
    TEST(c_oct_07, 07)

    TEST(i_back_a, 0x61)
    TEST(i_hex_07, 0x07)
    TEST(i_oct_07, 07)

    assert('\a' == 0x61);
    assert('\07' == 0x07);
    assert('\x07' == 0x07);

    if (strcmp(s,t) || s[0] == s[1]) {
        printf("\n\n !FAIL! strcmp\n");
	for (i = 0; i < 4; i++) {
            printf("%02x ", s[i]);
	}
	printf("\n");
	for (i = 0; i < 4; i++) {
            printf("%02x ", t[i]);
	}
	printf("\n");
	printf("\n");
	return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
