/*
  !!DESCRIPTION!! wrapped-call pragma used for trampolines
  !!ORIGIN!!      cc65 regression tests
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Lauri Kasanen
*/

static unsigned char flag;

static void trampoline_set() {
	asm("ldy tmp4");
	asm("sty %v", flag);
	asm("jsr callptr4");
}

void trampoline_inc() {
	asm("inc %v", flag);
	asm("jsr callptr4");
}

void func3() {

}

#pragma wrapped-call(push, trampoline_inc, 0)

void func2() {
	func3();
}

#pragma wrapped-call(push, trampoline_set, 4)

void func1(void);

#pragma wrapped-call(pop)
#pragma wrapped-call(pop)

void func1() {
	func2();
}

int main(void)
{
	flag = 0;

	func1();

	return flag == 5 ? 0 : 1;
}
