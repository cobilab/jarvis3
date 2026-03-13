#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "mem.h"
#include "cm_array.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ARRAY *CreateArrayTable(uint64_t nSym, uint64_t length)
  {
  ARRAY *A    = (ARRAY *) Calloc(1, sizeof(ARRAY));
  A->nSym     = nSym;
  A->length   = length;
  A->counters = (ACC *) Calloc(A->length * A->nSym, sizeof(ACC));
  return A;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateArrayCounter(ARRAY *A, uint32_t sym, uint64_t idx)
  {
  ACC *AC = &A->counters[idx * A->nSym];

  if(++AC[sym] == MAX_ARRAY_COUNTER)
    {
    if(A->nSym == 4)
      {
      AC[0] >>= 1;
      AC[1] >>= 1;
      AC[2] >>= 1;
      AC[3] >>= 1;
      }
    else
      {
      uint32_t x;
      for(x = 0 ; x < A->nSym ; ++x)
        AC[x] >>= 1;
      }
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveArrayTable(ARRAY *A)
  {
  Free(A->counters);
  Free(A);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
