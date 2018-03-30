/*fixed point number holds an int that represents a fixed point number*/
struct fixed_point
{
  int num;
};

/* converts an int to a fixed point number */

struct fixed_point fp_to_fp (int n);

/* converts a fixed_point number to an int rounding to zero */

int fp_to_int_zero (struct fixed_point *fp);

/* converts a fixed_point number to an int rounding to nearest */

int fp_to_int_nearest (struct fixed_point *fp);

/* add x+y */

struct fixed_point fp_add (struct fixed_point *x, struct fixed_point *y);

/* subtract x-y */

struct fixed_point fp_sub (struct fixed_point *x, struct fixed_point *y);

/* add x+n */

struct fixed_point fp_add_n (struct fixed_point *x, int n);

/* sub x-n */

struct fixed_point fp_sub_xn (struct fixed_point *x, int n);

/* sub n-x */

struct fixed_point fp_sub_nx (int n,struct fixed_point *x);

/* multiply x*y */

struct fixed_point fp_mul (struct fixed_point *x, struct fixed_point *y);

/* divide x/y */

struct fixed_point fp_div (struct fixed_point *x, struct fixed_point *y);

/* multiply x*n */

struct fixed_point fp_mul_n (struct fixed_point *x, int n);

/* divide x/n */

struct fixed_point fp_div_xn (struct fixed_point *x, int n);

/* divide n/x */

struct fixed_point fp_div_nx (int n, struct fixed_point *x);

/* fixed_point from fraction */

struct fixed_point fp_from_fraction (int x, int y);
