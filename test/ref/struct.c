/*
  !!DESCRIPTION!! structs
  !!ORIGIN!!      LCC 4.1 Testsuite
  !!LICENCE!!     own, freely distributeable for non-profit. read CPYRIGHT.LCC
*/

#include "common.h"

typedef struct point { int x,y; } point;
typedef struct rect { point pt1, pt2; } rect;

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#ifdef NO_FUNCS_RETURN_STRUCTS
# ifdef NO_FUNCS_TAKE_STRUCTS
/* canonicalize rectangle coordinates */
void canonrect(rect *d,rect *r) {
        d->pt1.x = min(r->pt1.x, r->pt2.x);
        d->pt1.y = min(r->pt1.y, r->pt2.y);
        d->pt2.x = max(r->pt1.x, r->pt2.x);
        d->pt2.y = max(r->pt1.y, r->pt2.y);
}
/* add two points */
void addpoint(point *p, point *p1, point *p2) {
        p->x= p1->x + p2->x;
        p->y= p1->y + p2->y;
}
/* make a point from x and y components */
void makepoint(point *p,int x, int y) {
        p->x = x;
        p->y = y;
}
/* make a rectangle from two points */
void makerect(rect *d,point *p1, point *p2) {
rect r;
        r.pt1.x = p1->x;
        r.pt1.y = p1->y;
        r.pt2.x = p2->x;
        r.pt2.y = p2->y;

        canonrect(d,&r);
}

#ifdef NO_SLOPPY_STRUCT_INIT
struct odd {char a[3]; } y = {{'a', 'b', 0 }};
#else
struct odd {char a[3]; } y = {'a', 'b', 0};
#endif

odd(struct odd *y) {
        struct odd *x = y;
        printf("%s\n\r", x->a);
}

# else  /* FUNCS_TAKE_STRUCTS */
/* canonicalize rectangle coordinates */
void canonrect(rect *d,rect r) {
        d->pt1.x = min(r.pt1.x, r.pt2.x);
        d->pt1.y = min(r.pt1.y, r.pt2.y);
        d->pt2.x = max(r.pt1.x, r.pt2.x);
        d->pt2.y = max(r.pt1.y, r.pt2.y);
}
/* add two points */
void addpoint(point *p, point p1, point p2) {
        p->x= p1.x + p2.x;
        p->y= p1.y + p2.y;
}
/* make a point from x and y components */
void makepoint(point *p,int x, int y) {
        p->x = x;
        p->y = y;
}
/* make a rectangle from two points */
void makerect(rect *d,point p1, point p2) {
rect r;
	r.pt1 = p1;
	r.pt2 = p2;

        canonrect(d,r);
}

#ifdef NO_SLOPPY_STRUCT_INIT
struct odd {char a[3]; } y = {{'a', 'b', 0}};
#else
struct odd {char a[3]; } y = {'a', 'b', 0};
#endif

odd(struct odd y) {
        struct odd x = y;
        printf("%s\n\r", x.a);
}

# endif /* FUNCS_TAKE_STRUCTS */

#else /* FUNCS_RETURN_STRUCTS */

/* add two points */
point addpoint(point p1, point p2) {
	p1.x += p2.x;
	p1.y += p2.y;
	return p1;
}
/* canonicalize rectangle coordinates */
rect canonrect(rect r) {
	rect temp;

	temp.pt1.x = min(r.pt1.x, r.pt2.x);
	temp.pt1.y = min(r.pt1.y, r.pt2.y);
	temp.pt2.x = max(r.pt1.x, r.pt2.x);
	temp.pt2.y = max(r.pt1.y, r.pt2.y);
	return temp;
}
/* make a point from x and y components */
point makepoint(int x, int y) {
	point p;

	p.x = x;
	p.y = y;
	return p;
}

/* make a rectangle from two points */
rect makerect(point p1, point p2) {
	rect r;

	r.pt1 = p1;
	r.pt2 = p2;
	return canonrect(r);
}

struct odd {char a[3]; } y =
{
#ifdef NO_SLOPPY_STRUCT_INIT
	{
#endif
	'a', 'b', 0
#ifdef NO_SLOPPY_STRUCT_INIT
	}
#endif
};

odd(struct odd y)
{
	struct odd x
		= y;
        printf("%s\n\r", x.a);
}

#endif

/* is p in r? */
# ifdef NO_FUNCS_TAKE_STRUCTS
int ptinrect(point *p, rect *r) {
        return p->x >= r->pt1.x && p->x < r->pt2.x
                && p->y >= r->pt1.y && p->y < r->pt2.y;
}
#else
int ptinrect(point p, rect r) {
	return p.x >= r.pt1.x && p.x < r.pt2.x
		&& p.y >= r.pt1.y && p.y < r.pt2.y;
}
#endif

#ifdef NO_FUNCS_RETURN_STRUCTS

#ifdef NO_LOCAL_STRUCT_INIT
#ifdef NO_SLOPPY_STRUCT_INIT
point pts[] = { {-1, -1},{ 1, 1},{ 20, 300},{ 500, 400 } };
#else
point pts[] = { -1, -1, 1, 1, 20, 300, 500, 400 };
#endif
point origin = { 0, 0 };
point maxpt = { 320, 320 };
#endif

main() {
int i;
point x;
rect screen;
#ifndef NO_LOCAL_STRUCT_INIT
point origin = { 0, 0 };
point maxpt = { 320, 320 };
#ifdef NO_SLOPPY_STRUCT_INIT
point pts[] = { {-1, -1},{ 1, 1},{ 20, 300},{ 500, 400 } };
#else
point pts[] = { -1, -1, 1, 1, 20, 300, 500, 400 };
#endif
#endif

          makepoint (     &x,    -10,    -10);
#ifdef NO_FUNCS_TAKE_STRUCTS
          addpoint  ( &maxpt, &maxpt,     &x);
#else
          addpoint  ( &maxpt, maxpt,     x);
#endif
          makepoint (     &x,     10,     10);

#ifdef NO_FUNCS_TAKE_STRUCTS
          addpoint  (&origin,&origin,     &x);
          makerect  (&screen, &maxpt,&origin);
#else
          addpoint  (&origin,origin,     x);
          makerect  (&screen, maxpt,origin);
#endif

        for (i = 0; i < sizeof pts/sizeof pts[0]; i++) {
                makepoint(&x,pts[i].x, pts[i].y);
                printf("(%d,%d) is ", pts[i].x, x.y);
#ifdef NO_FUNCS_TAKE_STRUCTS
                if (ptinrect(&x, &screen) == 0)
#else
                if (ptinrect(x, screen) == 0)
#endif
		{
                        printf("not ");
		}
                printf("within (%d,%d; %d,%d)\n\r", screen.pt1.x, screen.pt1.y,
                        screen.pt2.x, screen.pt2.y);
        }
#ifdef NO_FUNCS_TAKE_STRUCTS
        odd(&y);
#else
        odd(y);
#endif

        return 0;
}

#else /* FUNCS_RETURN_STRUCTS */

main() {
int i;
point x, origin = { 0, 0 }, maxpt = { 320, 320 };

#ifdef NO_SLOPPY_STRUCT_INIT
point pts[] = { {-1, -1}, {1, 1}, {20, 300}, {500, 400} };
#else
point pts[] = { -1, -1, 1, 1, 20, 300, 500, 400 };
#endif

rect screen =
	makerect(
		addpoint(maxpt, makepoint(-10, -10)),
		addpoint(origin, makepoint(10, 10))
		);

	test1();
	
	for (i = 0; i < sizeof pts/sizeof pts[0]; i++) {
		printf("(%d,%d) is ", pts[i].x,
			(x = makepoint(pts[i].x, pts[i].y)).y);
		if (ptinrect(x, screen) == 0)
			printf("not ");
                printf("within (%d,%d; %d,%d)\n\r", screen.pt1.x, screen.pt1.y,
			screen.pt2.x, screen.pt2.y);
	}
	odd(y);

	return 0;
}

#endif /* FUNCS_RETURN_STRUCTS */
