#include "unittest.h"

struct Object
{
    int a;
    int data[10];
};

struct Object object_data = { 0x0102, {0x0304, 0x0506, 
                             0x0708, 0x090A, 0x0B0C,
                             0x0D0E, 0x0F10, 0x1112,
                             0x1314, 0x1516}};

TEST
{
    struct Object *o = &object_data;
    ASSERT_IsTrue(o->a == 0x0102, "Wrong value for a");
    ASSERT_IsTrue(o->data[2] == 0x0708, "Wrong value for data[2]");
}
ENDTEST
