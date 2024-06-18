#if 1
static float powerOfTen(int num)
{
    int i;
//    float rst = 1.0f;
    float rst;
    rst = 1.0f;
    if(num >= 0) {
        for(i = 0; i < num ; ++i) {
            // rst *= 10.0;
            rst = rst * 10.0f;
        }
    } else {
        for(i = 0; i < (0 - num); ++i) {
            // rst *= 0.1;
            rst = rst * 0.1f;
        }
    }

    return rst;
}

#define MAXDIGITS   8
float sqrtf(float a)
{
//    float rst = 0.0f;
    float rst;
//    float z = a;
    float z;
    signed int i;
//    float j = 1.0f;
    float j;
    float power;
    rst = 0.0f;
    z = a;
    j = 1.0f;
    for(i = MAXDIGITS ; i > 0 ; i--) {
        power = powerOfTen(i);
        // value must be bigger then 0
        if(z - (( 2.0f * rst ) + ( j * power)) * ( j * power) >= 0) {
            while( z - (( 2.0f * rst ) + ( j * power)) * ( j * power) >= 0) {
                //j++;
                j = j + 1.0f;
                if(j >= 10.0f) {
                    break;
                }
            }
            //j--; //correct the extra value by minus one to j
            j = j - 1.0f;
            //z -= (( 2.0f * rst ) + ( j * power)) * ( j * power); //find value of z
            z = z - (( 2.0f * rst ) + ( j * power)) * ( j * power); //find value of z

            //rst += j * power;    // find sum of a
            rst = rst + (j * power);    // find sum of a
            j = 1.0f;
        }
    }

    for(i = 0 ; i >= 0 - MAXDIGITS ; i--) {
        power = powerOfTen(i);
        if(z - (( 2.0f * rst ) + ( j * power))*( j * power) >= 0) {
            while( z - (( 2.0f * rst ) + ( j * power))*( j * power) >= 0) {
                //j++;
                j = j + 1.0f;
                if(j >= 10.0f) {
                    break;
                }
            }
            //j--; //correct the extra value by minus one to j
            j = j - 1.0f;
            //z -= (( 2.0f * rst ) + ( j * power))*( j * power); //find value of z
            z = z - (( 2.0f * rst ) + ( j * power)) * ( j * power); //find value of z
            //rst += j * power;    // find sum of a
            rst = rst + (j * power);    // find sum of a
            j = 1.0f;
        }
    }

    // find the number on each digit
    return rst;
}
#endif

#if 0
float sqrtf(float a)
{
    a = a;
    return 0.0f;
}
#endif
