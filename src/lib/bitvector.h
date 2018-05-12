#ifndef __LIB_BITVECTOR_H
#define __LIB_BITVECTOR_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef uint32_t bv_cnt_t;

#define BITVECTOR_SIZE 8
#define BITVECTOR_COUNT ((int) (sizeof (bv_cnt_t) * BITVECTOR_SIZE))


struct bitvector
  {
    bv_cnt_t value[BITVECTOR_SIZE];
  };

void bitvector_init (struct bitvector *vec);
bool bitvector_get (struct bitvector *vec, size_t index);
void bitvector_set (struct bitvector *vec, size_t index, bool value);

#endif
