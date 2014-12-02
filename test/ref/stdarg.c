/*
  !!DESCRIPTION!! variable argument lists
  !!ORIGIN!!      LCC 4.1 Testsuite
  !!LICENCE!!     own, freely distributeable for non-profit. read CPYRIGHT.LCC
*/

#include "common.h"
#include <stdarg.h>

#ifndef NO_FUNCS_TAKE_STRUCTS
struct node
{
	int a[4];
} x =
{
#ifdef NO_SLOPPY_STRUCT_INIT
	{
#endif
		1,2,3,4
#ifdef NO_SLOPPY_STRUCT_INIT
	}
#endif
};
#endif

print(char *fmt, ...);

main()
{
	print("test 1\n");
	print("test %s\n", "2");
	print("test %d%c", 3, '\n');
	print("%s%s %w%c", "te", "st", 4, '\n');
    #ifdef NO_FLOATS
		print("%s%s %f%c", "te", "st", (signed long) 5, '\n');
	#else
		print("%s%s %f%c", "te", "st", 5.0, '\n');
    #endif
	#ifndef NO_FUNCS_TAKE_STRUCTS
        print("%b %b %b %b %b %b\n", x, x, x, x, x, x);
	#endif
	return 0;
}

print(char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	for (; *fmt; fmt++)
	{
		if (*fmt == '%')
			switch (*++fmt) {
            case 'b': {
					#ifdef NO_FUNCS_TAKE_STRUCTS
                    	printf("(1 2 3 4)");
					#else
                    	struct node x =
							va_arg(
								ap,
								struct node
								);
                    	printf("(%d %d %d %d)", x.a[0], x.a[1], x.a[2], x.a[3]);
					#endif
                    break;
                    }
			case 'c':
				/* printf("%c", va_arg(ap, char)); */
				printf("%c", va_arg(ap, int));
				break;
			case 'd':
				printf("%d", va_arg(ap, int));
				break;
			case 'w':
				/* printf("%x", va_arg(ap, short)); */
				printf("%x", va_arg(ap, int));
				break;
			case 's':
				printf("%s", va_arg(ap, char *));
				break;
			case 'f':
            	#ifdef NO_FLOATS
					printf("%ld.000000", va_arg(ap, signed long));
				#else
					printf("%f", va_arg(ap, double));
            	#endif
				break;
			default:
				printf("%c", *fmt);
				break;
			}
		 else
			printf("%c", *fmt);
	}
	va_end(ap);
}
