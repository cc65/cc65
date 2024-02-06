static unsigned char val, val2;

static void act(const unsigned char op) {

        static const void * const arr[] = {
                &&op0,
                &&op1,
                &&op2,
                &&op3,
                &&op4,
                &&op5,
                &&op6,
        };

        goto *arr[op];

        op0:
        val += 1;
        return;

        op1:
        val += 2;
        return;

        op2:
        val += 3;
        return;

        op3:
        val2 += 1;
        return;

        op4:
        val2 += 5;
        return;

        op5:
        val2 += 7;
        return;

        op6:
        val2 += 9;
        return;
}

int main(void) {

        val = val2 = 0;

        act(1);
        act(3);
        act(5);

        return val == 2 && val2 == 8 ? 0 : 1;
}
