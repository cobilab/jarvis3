#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "repeats.h"
#include "common.h"
#include "dna.h"
#include "mem.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// LOOKUP TABLES FOR SPEED ENHANCEMENT
//
double LT  [MAX_LT * MAX_LT];
double LT_C[MAX_LT * MAX_LT];
static int LT_READY = 0;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline uint8_t Comp2(uint8_t x)
  {
  static const uint8_t comp2[4] = {3, 2, 1, 0};
  return comp2[x & 0x3];
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// FILL LOOKUP TABLE
//
void FillLT(void)
  {
  uint32_t x, y;
  for(x = 0 ; x < MAX_LT ; ++x)
    for(y = 0 ; y < MAX_LT ; ++y)
      {
      uint32_t lt_idx = y * MAX_LT + x;
      LT  [lt_idx] = (y + 1.0) / (x + 2.0);
      LT_C[lt_idx] = (1.0 - LT[lt_idx]) / 3.0;
      }

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// CREATES THE RCLASS BASIC STRUCTURE
//
RCLASS *CreateRC(uint32_t m, double b, uint32_t l, uint32_t c,
double g, uint8_t i, double w, uint64_t s)
  {
  RCLASS *C      = (RCLASS *) Calloc(1, sizeof(RCLASS));
  C->P           = (RPARAM *) Calloc(1, sizeof(RPARAM));
  C->RM          = (RMODEL *) Calloc(m, sizeof(RMODEL));
  C->mRM         = m;
  C->P->rev      = i;
  C->P->beta     = ((int)(b * 65534)) / 65534.0;
  C->P->gamma    = ((int)(g * 65534)) / 65534.0;
  C->P->limit    = l;
  C->P->ctx      = c;
  C->P->mult     = CalcMult(c);
  C->P->idx      = 0;
  C->P->idxRev   = 0;
  C->P->c_pos    = 0;
  C->P->c_max    = s;
  C->P->c_idx    = 0;
  C->P->c_idxRev = 0;
  C->P->iWeight  = w;

  // CACHE TABLE
  C->T       = (RTABLE *) Calloc(1, sizeof(RTABLE));
  C->T->nPos = s;

  if(C->T->nPos == 1)
    {
    C->T->size = 1ULL << (2 * C->P->ctx);
    }
  else
    {
    C->T->nPosAnd1 = C->T->nPos + 1;
    C->T->size     = (1ULL << (2 * C->P->ctx)) * C->T->nPosAnd1;
    }

  C->T->array = (uint32_t *) Calloc(C->T->size + 1, sizeof(uint32_t));

  if(!LT_READY)
    {
    FillLT();
    LT_READY = 1;
    }

  return C;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// REVERSE COMPLEMENT INDEX BASED ON PAST SYMBOLS
//
uint64_t GetIdxRev(uint8_t *p, RCLASS *C)
  {
  return (C->P->idxRev = (C->P->idxRev >> 2) + CompNum(*p) * C->P->mult);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// INDEX CALC BASED ON PAST SYMBOLS
//
uint64_t GetIdx(uint8_t *p, RCLASS *C)
  {
  return (C->P->idx = ((C->P->idx - *(p - C->P->ctx) * C->P->mult) << 2) + *p);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// START EACH REPEAT MODEL, CURRENTLY BY RANDOM STORED POSITION
//
int32_t StartRM(RCLASS *C, uint32_t m, uint64_t i, uint8_t r)
  {
  RMODEL *RM = &C->RM[m];
  uint32_t pos;

  if(C->T->nPos == 1)
    {
    pos = C->T->array[i];

    if(r == 0)
      {
      if(pos == 0)
        return 0;
      }
    else
      {
      if(pos <= C->P->ctx + 1)
        return 0;
      pos -= C->P->ctx + 1;
      }
    }
  else
    {
    uint32_t *E = &C->T->array[i * C->T->nPosAnd1];
    uint32_t last = E[C->T->nPos];
    static uint32_t rng = 2463534242u;
    uint32_t idx;

    if(last == 0)
      return 0;

    rng ^= rng << 13;
    rng ^= rng >> 17;
    rng ^= rng << 5;

    idx = rng % C->T->nPos;
    pos = E[idx];

    if(pos == 0)
      pos = E[last];

    if(pos <= C->P->ctx + 1)
      return 0;

    if(r != 0)
      pos -= C->P->ctx + 1;
    }

  RM->pos    = pos;
  RM->nHits  = 0;
  RM->nTries = 0;
  RM->rev    = r;
  RM->acting = 0;
  RM->weight = C->P->iWeight;

  RM->probs[0] = 0.0;
  RM->probs[1] = 0.0;
  RM->probs[2] = 0.0;
  RM->probs[3] = 0.0;

  return 1;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// ADD KMER POSITION INTO RTABLE
//
void AddKmerPos(RTABLE *RT, uint64_t key, uint32_t pos)
  {
  if(RT->nPos == 1)
    {
    RT->array[key] = pos;
    return;
    }

  uint32_t *TC = &RT->array[key * RT->nPosAnd1];
  uint32_t nPos = RT->nPos;
  uint32_t idx = TC[nPos];

  if(idx == nPos)
    {
    TC[0]    = pos;
    TC[nPos] = 0;
    }
  else
    {
    ++idx;
    TC[idx]  = pos;
    TC[nPos] = idx;
    }

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// COMPUTE REPEAT MODEL PROBABILITIES
//
void ComputeRMProbs(RCLASS *C, RMODEL *R, uint8_t *b)
  {
  uint8_t s;
  double p_hit, p_other;

  (void) C;

  if(R->rev == 0)
    s = GetNBase(b, R->pos);
  else
    s = Comp2(GetNBase(b, R->pos));

  if(R->nTries < MAX_LT)
    {
    uint32_t lt_idx = R->nHits * MAX_LT + R->nTries;
    p_hit   = LT[lt_idx];
    p_other = LT_C[lt_idx];
    }
  else
    {
    p_hit   = (R->nHits + 1.0) / (R->nTries + 2.0);
    p_other = (1.0 - p_hit) / 3.0;
    }

  R->probs[0] = p_other;
  R->probs[1] = p_other;
  R->probs[2] = p_other;
  R->probs[3] = p_other;
  R->probs[s] = p_hit;

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// UPDATE REPEAT MODEL
//
void UpdateRM(RMODEL *R, uint8_t *b, uint8_t s)
  {
  uint8_t rsym;

  R->lastHit = 1;

  if(R->rev == 0)
    {
    rsym = GetNBase(b, R->pos);
    ++R->pos;
    }
  else
    {
    rsym = Comp2(GetNBase(b, R->pos));
    --R->pos;
    }

  if(rsym == s)
    {
    ++R->nHits;
    R->lastHit = 0;
    }

  ++R->nTries;
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// RENORMALIZE REPEAT WEIGHTS
//
void RenormWeights(RCLASS *C)
  {
  uint32_t n;
  double t = 0.0;

  if(C->nRM <= 1)
    return;

  for(n = 0 ; n < C->nRM ; ++n)
    t += C->RM[n].weight;

  if(t == 0.0)
    return;

  t = 1.0 / t;
  for(n = 0 ; n < C->nRM ; ++n)
    C->RM[n].weight *= t;

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// STOP USELESS REPEAT MODELS
//
void StopRM(RCLASS *C)
  {
  uint32_t src, dst = 0;

  for(src = 0 ; src < C->nRM ; ++src)
    {
    RMODEL *R = &C->RM[src];
    R->acting = R->acting * C->P->beta + R->lastHit;

    if(R->acting <= C->P->limit && R->pos != 0)
      {
      if(dst != src)
        C->RM[dst] = C->RM[src];
      ++dst;
      }
    }

  C->nRM = dst;

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// START NEW REPEAT MODELS IF THERE IS STILL SPACE
//
void StartMultipleRMs(RCLASS *C, uint8_t *b)
  {
  uint64_t idx = GetIdx(b, C);

  if(C->P->rev != 2)
    if(C->nRM < C->mRM && StartRM(C, C->nRM, idx, 0))
      ++C->nRM;

  if(C->P->rev != 0)
    {
    uint64_t idx_rev = GetIdxRev(b, C);
    if(C->nRM < C->mRM && StartRM(C, C->nRM, idx_rev, 1))
      ++C->nRM;
    }

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// COMPUTE AND EXTRACT MIXTURED PROBABILITIES
//
void ComputeMixture(RCLASS *C, PMODEL *M, uint8_t *b)
  {
  double f0 = 0.0, f1 = 0.0, f2 = 0.0, f3 = 0.0;
  uint32_t r;

  for(r = 0 ; r < C->nRM ; ++r)
    {
    RMODEL *R = &C->RM[r];
    const double rmw = R->weight;

    ComputeRMProbs(C, R, b);

    f0 += R->probs[0] * rmw;
    f1 += R->probs[1] * rmw;
    f2 += R->probs[2] * rmw;
    f3 += R->probs[3] * rmw;
    }

  M->freqs[0] = 1 + (uint32_t)(f0 * MAXC);
  M->freqs[1] = 1 + (uint32_t)(f1 * MAXC);
  M->freqs[2] = 1 + (uint32_t)(f2 * MAXC);
  M->freqs[3] = 1 + (uint32_t)(f3 * MAXC);

  M->sum = M->freqs[0] + M->freqs[1] + M->freqs[2] + M->freqs[3];

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// UPDATE WEIGHTS ACCORDING TO THE PERFORMANCE OF EACH REPEAT MODEL
//
void UpdateWeights(RCLASS *C, uint8_t *b, uint8_t s)
  {
  uint32_t r;
  for(r = 0 ; r < C->nRM ; ++r)
    {
    RMODEL *R = &C->RM[r];
    R->weight = Power(R->weight, C->P->gamma) * R->probs[s];
    UpdateRM(R, b, s);
    }

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
