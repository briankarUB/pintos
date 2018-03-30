#include "threads/fixed-point.h"

/* converts an int to a fixed point number */

struct fixed_point fp_to_fp(int n)
{
  //multiply by 2^14
  struct fixed_point ff;
  ff.num = n * (1 << 14);
  return ff;
}

/* converts a fixed_point number to an int rounding to zero */

int fp_to_int_zero (struct fixed_point *fp)
{
  int n;
  n = fp->num/ (1 << 14);
  return n;
}

/* converts a fixed_point number to an int rounding to nearest */

int fp_to_int_nearest (struct fixed_point *fp)
{
  /*
    (x + f / 2) / f if x >= 0,
    (x - f / 2) / f if x <= 0.
  */

  int f = (1 << 14);
  if ( fp->num > 0 )
  {
    return (fp->num + f/2)/f;
  }
  else if ( fp->num < 0 )
  {
    return (fp->num - f/2)/f;
  }
  else
  return 0;

}

/* add x+y */

struct fixed_point fp_add (struct fixed_point *x, struct fixed_point *y)
{
  struct fixed_point z;
  z.num = x->num + y->num;
  return z;
}

/* subtract x-y */

struct fixed_point fp_sub (struct fixed_point *x, struct fixed_point *y)
{
  struct fixed_point z;
  z.num = x->num - y->num;
  return z;
}
/* add x+n */

struct fixed_point fp_add_n (struct fixed_point *x, int n)
{
  struct fixed_point y;
  y = fp_to_fp(n);
  struct fixed_point z;
  z.num = x->num + y.num;
  return z;
}
/* sub x-n */

struct fixed_point fp_sub_xn (struct fixed_point *x, int n)
{
  struct fixed_point y;
  y = fp_to_fp(n);
  struct fixed_point z;
  z.num = x->num - y.num;
  return z;
}

/* sub n-x */

struct fixed_point fp_sub_nx (int n, struct fixed_point *x)
{
  struct fixed_point y;
  y = fp_to_fp(n);
  struct fixed_point z;
  z.num = y.num - x->num;
  return z;

}
/* multiply x*y */

struct fixed_point fp_mul (struct fixed_point *x, struct fixed_point *y)
{
  struct fixed_point z;
  z.num = (x->num * y->num)/(1 << 14);
  return z;
}
/* divide x/y */

struct fixed_point fp_div (struct fixed_point *x, struct fixed_point *y)
{
  struct fixed_point z;
  z.num = (x->num / y->num)*(1 << 14);
  return  z;
}
/* multiply x*n */

struct fixed_point fp_mul_n (struct fixed_point *x, int n)
{
  struct fixed_point y;
  struct fixed_point z;
  y = fp_to_fp(n);
  z.num = (x->num * y.num)/(1 << 14);
  return z;
}
/* divide x/n */

struct fixed_point fp_div_xn (struct fixed_point *x, int n)
{
  struct fixed_point y;
  struct fixed_point z;
  y = fp_to_fp(n);
  z.num = (x->num / y.num)*(1 << 14);
  return z;
}
/* divide n/x */

struct fixed_point fp_div_nx (int n, struct fixed_point *x)
{
  struct fixed_point y;
  struct fixed_point z;
  y = fp_to_fp(n);
  z.num = (y.num / x->num)*(1 << 14);
  return z;
}

/* fixed_point from fraction */

struct fixed_point fp_from_fraction (int x, int y)
{
  struct fixed_point a = fp_to_fp(x);
  struct fixed_point b = fp_to_fp(y);
  return fp_div(&a,&b);
}
