static unsigned char val;

static void foo(void) {
	val = 5;
}

static void wrap() {

	asm("lda #<%v", foo);
	asm("ldx #>%v", foo);
	asm("jmp callax");

}

int main() {

	val = 0;
	wrap();

	return val == 5 ? 0 : 1;
}
