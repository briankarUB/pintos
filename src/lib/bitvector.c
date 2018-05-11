#include "bitvector.h"
#include <debug.h>

void
bitvector_init (struct bitvector *vec)
{
  ASSERT (vec != NULL);

  int i;

  for (i = 0; i < BITVECTOR_SIZE; ++i)
    vec->value[i] = 0x0;
}

bool
bitvector_get (struct bitvector *vec, size_t index)
{
  ASSERT (vec != NULL);
  ASSERT (index < BITVECTOR_COUNT);

  size_t ch_index = (size_t) (index / sizeof (bv_cnt_t));
  size_t bit_index = index % sizeof (bv_cnt_t);
  bv_cnt_t ch = vec->value[ch_index];

  return (bool) ((ch >> bit_index) & 0x1);
}

void
bitvector_set (struct bitvector *vec, size_t index, bool value)
{
  ASSERT (vec != NULL);
  ASSERT (index < BITVECTOR_COUNT);

  size_t ch_index = (size_t) (index / sizeof (bv_cnt_t));
  size_t bit_index = index % sizeof (bv_cnt_t);
  bv_cnt_t val = 0x1 << bit_index;

  if (value)
    vec->value[ch_index] |= val;
  else
    vec->value[ch_index] &= ~val;
}
