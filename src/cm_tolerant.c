#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cm_tolerant.h"
#include "strings.h"
#include "mem.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

TOLERANT *CreateTolerantModel(uint32_t e, uint32_t d, uint32_t c, uint32_t n,
uint32_t i)
  {
  TOLERANT *T  = (TOLERANT *) Calloc(1, sizeof(TOLERANT));

  T->mask      = (uint8_t *) Calloc(BGUARD, sizeof(uint8_t));
  T->seq       = CreateCBuffer(BUFFER_SIZE, BGUARD);
  T->in        = 0;
  T->fails     = 0;
  T->idx       = 0;
  T->threshold = e;
  T->ctx       = c;
  T->nSym      = n;
  T->den       = d;
  T->ir        = i;

  return T;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int32_t BestId(uint32_t *f, uint32_t sum, uint32_t nSym)
  {
  if(sum == nSym)
    return -2; // ZERO COUNTERS

  if(nSym == 4)
    {
    const uint32_t f0 = f[0];
    const uint32_t f1 = f[1];
    const uint32_t f2 = f[2];
    const uint32_t f3 = f[3];

    uint32_t best = 0;
    uint32_t max = f0;

    if(f1 > max){ max = f1; best = 1; }
    if(f2 > max){ max = f2; best = 2; }
    if(f3 > max){ max = f3; best = 3; }

    if((best != 0 && max == f0) ||
       (best != 1 && max == f1) ||
       (best != 2 && max == f2) ||
       (best != 3 && max == f3))
      return -1;

    return (int32_t) best;
    }

  {
  uint32_t x, best = 0, max = f[0];

  for(x = 1 ; x < nSym ; ++x)
    if(f[x] > max)
      {
      max = f[x];
      best = x;
      }

  for(x = 0 ; x < nSym ; ++x)
    if(best != x && max == f[x])
      return -1;

  return (int32_t) best;
  }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Fail(TOLERANT *T)
  {
  if(T->fails <= T->threshold)
    {
    const uint8_t old = T->mask[0];
    ShiftBuffer(T->mask, T->ctx, 1);

    if(old == 0)
      ++T->fails;
    }
  else
    {
    T->in = 0;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Hit(TOLERANT *T)
  {
  const uint8_t old = T->mask[0];
  ShiftBuffer(T->mask, T->ctx, 0);

  if(old != 0)
    --T->fails;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateTolerantModel(TOLERANT *T, PMODEL *PM, uint8_t sym)
  {
  int32_t best = BestId(PM->freqs, PM->sum, T->nSym);

  switch(best)
    {
    case -2:  // IT IS A ZERO COUNTER [NOT SEEN BEFORE]
      if(T->in != 0)
        Hit(T);
    break;

    case -1:  // IT HAS AT LEAST TWO MAXIMUM FREQS [CONFUSION FREQS]
      if(T->in != 0)
        Fail(T);
    break;

    default:  // IT HAS ONE MAXIMUM FREQ
      if(T->in == 0)
        {
        T->in = 1;
        T->fails = 0;
        memset(T->mask, 0, T->ctx);
        }
      else
        {
        if((uint8_t) best == sym)
          {
          Hit(T);
          }
        else
          {
          Fail(T);
          T->seq->buf[T->seq->idx] = (uint8_t) best;
          }
        }
    break;
    }

  UpdateCBuffer(T->seq);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveTolerantModel(TOLERANT *T)
  {
  RemoveCBuffer(T->seq);
  Free(T->mask);
  Free(T);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
