static unsigned char val;

static void foo(void) {
        val = 5;
}

static void wrap(void) {

        asm("lda #<%v", foo);
        asm("ldx #>%v", foo);
        asm("jmp callax");

}

int main(void) {

        val = 0;
        wrap();

        return val == 5 ? 0 : 1;
}
