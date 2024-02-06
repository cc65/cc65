/*
  !!DESCRIPTION!! char-sized post-increment and -decrement
  !!ORIGIN!!      cc65 regression tests
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Lauri Kasanen
*/


static unsigned char val, array[2];

int main() {

        val = 0;
        array[0] = array[1] = 10;

        array[val++] = 2;
        array[val++] = 2;
        --val;
        array[val--] = 0;
        array[val--] = 0;

        return (array[0] == array[1] && array[0] == 0 && val == 0xff) ? 0 : 1;
}
