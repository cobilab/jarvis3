#include <stdio.h>
#include <stdlib.h>
#include "mem.h"
#include "cm_hash.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline ENTRY *FindEntry(HASH *H, uint32_t hi, uint64_t key)
  {
  ENTRY *bucket = H->entries[hi];
  ENTMAX n = H->entrySize[hi];
  ENTMAX x;

  for(x = 0 ; x < n ; ++x)
    if(bucket[x].key == key)
      return &bucket[x];

  return NULL;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

HASH *CreateHashTable(uint32_t nSym)
  {
  HASH *H        = (HASH *) Calloc(1, sizeof(HASH));
  H->size        = HASH_SIZE;
  H->nSym        = nSym;
  H->entries     = (ENTRY **) Calloc(H->size, sizeof(ENTRY *));
  H->entrySize   = (ENTMAX *) Calloc(H->size, sizeof(ENTMAX));
  H->entryCap    = (ENTMAX *) Calloc(H->size, sizeof(ENTMAX));
  H->zeroCounters = (HCC *) Calloc(H->nSym, sizeof(HCC));
  return H;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void InsertKey(HASH *H, uint32_t hi, uint64_t key, uint8_t s)
  {
  ENTMAX used = H->entrySize[hi];
  ENTMAX cap  = H->entryCap[hi];
  ENTRY *bucket;

  if(used == cap)
    {
    ENTMAX newCap = (cap == 0) ? 2 : (ENTMAX)(cap << 1);
    H->entries[hi] = (ENTRY *) Realloc(H->entries[hi], newCap * sizeof(ENTRY));
    H->entryCap[hi] = newCap;
    }

  bucket = H->entries[hi];
  bucket[used].counters = (HCC *) Calloc(s, sizeof(HCC));
  bucket[used].key = key;
  H->entrySize[hi] = used + 1;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

HCC *GetHCCounters(HASH *H, uint64_t key)
  {
  uint64_t h = ZHASH(key);
  uint32_t hi = (uint32_t)(h % H->size);
  ENTRY *e = FindEntry(H, hi, h);

  if(e)
    return e->counters;

  return NULL;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateHashCounter(HASH *H, uint32_t sym, uint64_t im)
  {
  uint64_t idx = ZHASH(im);
  uint32_t hIndex = (uint32_t)(idx % H->size);
  ENTRY *e = FindEntry(H, hIndex, idx);

  if(e)
    {
    HCC *c = e->counters;

    if(++c[sym] == MAX_HASH_COUNTER)
      {
      if(H->nSym == 4)
        {
        c[0] >>= 1;
        c[1] >>= 1;
        c[2] >>= 1;
        c[3] >>= 1;
        }
      else
        {
        uint32_t s;
        for(s = 0 ; s < H->nSym ; ++s)
          c[s] >>= 1;
        }
      }
    return;
    }

  InsertKey(H, hIndex, idx, H->nSym);
  H->entries[hIndex][H->entrySize[hIndex] - 1].counters[sym] = 1;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveHashTable(HASH *H)
  {
  uint32_t x;

  for(x = 0 ; x < H->size ; ++x)
    {
    if(H->entrySize[x] != 0)
      {
      ENTMAX y;
      for(y = 0 ; y < H->entrySize[x] ; ++y)
        Free(H->entries[x][y].counters);
      Free(H->entries[x]);
      }
    }

  Free(H->entries);
  Free(H->entrySize);
  Free(H->entryCap);
  Free(H->zeroCounters);
  Free(H);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
