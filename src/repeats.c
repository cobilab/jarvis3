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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// GET NUMERICAL BASE FROM PACKED SEQUENCE BY ID. THE ID%4 IS GIVEN BY THE 2
// LESS SIGNIFICATIVE BITS (ID&3).
//
uint8_t GetNBase(uint8_t *b, uint64_t i){
  return (uint8_t) (((0x3<<((3-(i&0x3))<<1)) & b[i>>2])>>((3-(i&0x3))<<1));
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
      LT_C[lt_idx] = (1 - LT[lt_idx]) / 3;
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
  C->P->beta     = ((int)(b*65534))/65534.0;
  C->P->gamma    = ((int)(g*65534))/65534.0;
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
  C->T           = (RTABLE *) Calloc(1, sizeof(RTABLE));
  C->T->nPos     = s;
  if(C->T->nPos == 1)
    {
    C->T->size = pow(NSYM, C->P->ctx);
    }
  else
    {
    C->T->nPosAnd1 = C->T->nPos + 1;
    C->T->size     = pow(NSYM, C->P->ctx) * C->T->nPosAnd1;
    }
  C->T->array = (uint32_t *) Calloc(C->T->size + 1, sizeof(uint32_t));

  FillLT();

  return C;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// REVERSE COMPLEMENT INDEX BASED ON PAST SYMBOLS
//
uint64_t GetIdxRev(uint8_t *p, RCLASS *C){
  return (C->P->idxRev = (C->P->idxRev>>2)+CompNum(*p)*C->P->mult);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// INDEX CALC BASED ON PAST SYMBOLS
//
uint64_t GetIdx(uint8_t *p, RCLASS *C){
  return (C->P->idx = ((C->P->idx-*(p-C->P->ctx)*C->P->mult)<<2)+*p);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// START EACH REPEAT MODEL, CURRENTLY BY RANDOM STORED POSITION 
//
int32_t StartRM(RCLASS *C, uint32_t m, uint64_t i, uint8_t r)
  {
  RMODEL *RM = NULL;

  if(C->T->nPos == 1)
    {
    uint32_t idx = C->T->array[i];
    RM = &C->RM[m];
    if(r == 0) // REGULAR REPEAT
      {
      if(idx == 0) return 0;
      RM->pos = idx;
      }
    else
      {
      if(idx <= C->P->ctx+1)
        return 0;
      RM->pos = idx - C->P->ctx - 1;
      }
    }
  else
    {
    uint32_t *E = &C->T->array[i * C->T->nPosAnd1];
    uint32_t last = E[C->T->nPos];

    if(last == 0) return 0;

    RM = &C->RM[m];
    uint64_t idx = rand() % C->T->nPos;

    if(E[idx] <= C->P->ctx+1)
      return 0;
      
    if(r == 0) // REGULAR REPEAT
      {
      if(E[idx] == 0)
        RM->pos = E[last]; // IF POSITION 0 THEN USE LATEST  
      else 
        RM->pos = E[idx];
      }
    else // INVERTED REPEAT
      {
      RM->pos = E[idx] - C->P->ctx - 1;
      }
    }

  RM->nHits  = 0;
  RM->nTries = 0;
  RM->rev    = r;
  RM->acting = 0;
  RM->weight = C->P->iWeight;

  memset((void *)RM->probs, 0, NSYM * sizeof(double));

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
    }
  else
    {
    uint32_t *TC = &RT->array[key * RT->nPosAnd1];
    uint32_t nPos = RT->nPos;
    uint32_t idx = TC[nPos];

    if(idx == nPos)
      {
      TC[0   ] = pos;
      TC[nPos] = 0;
      }
    else
      {
      TC[++idx] = pos;
      TC[nPos ] = idx;
      }
    }

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// COMPUTE REPEAT MODEL PROBABILITIES
//
void ComputeRMProbs(RCLASS *C, RMODEL *R, uint8_t *b)
  {
  uint8_t s = (R->rev) ? CompNum(GetNBase(b, R->pos)) : GetNBase(b, R->pos);
  double comp_prob;	

  if(R->nTries < MAX_LT)
    {
    uint32_t lt_idx = R->nHits * MAX_LT + R->nTries;
    R->probs[s] = LT  [lt_idx];
    comp_prob   = LT_C[lt_idx];
    }
  else
    {
    R->probs[s] = (R->nHits+1.0) / (R->nTries+2.0); 
    comp_prob = (1-R->probs[s])/3;
    }
    
  if(0 != s) R->probs[0] = comp_prob;
  if(1 != s) R->probs[1] = comp_prob;
  if(2 != s) R->probs[2] = comp_prob;
  if(3 != s) R->probs[3] = comp_prob;
  
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// UPDATE REPEAT MODEL
//
void UpdateRM(RMODEL *R, uint8_t *b, uint8_t s)
  {
  R->lastHit = 1;

  if(R->rev == 0)
    {
    if(GetNBase(b, R->pos++) == s)
      {
      ++R->nHits;
      R->lastHit = 0;
      }
    }
  else
    {
    if(CompNum(GetNBase(b, R->pos--)) == s)
      {
      ++R->nHits;
      R->lastHit = 0;
      }
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
  double t = 0;
  for(n = 0 ; n < C->nRM ; ++n)
    t += C->RM[n].weight;
  for(n = 0 ; n < C->nRM ; ++n)
    C->RM[n].weight /= t;

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// STOP USELESS REPEAT MODELS
//
void StopRM(RCLASS *C)
  {
  uint32_t n, a;
  do
    {
    for(a = 0, n = 0 ; n < C->nRM ; ++n)
      {
      RMODEL *R = &C->RM[n];
      if((R->acting = R->acting * C->P->beta + R->lastHit) > C->P->limit || 
      R->pos == 0)
      //ALTERNATIVE: 
      //if(R->nTries - R->nHits > 10 || R->pos == 0)
        {
        if(n != C->nRM-1)
          C->RM[n] = C->RM[C->nRM-1];
        C->nRM--;
        a = 1;
        break;
        }
      }
    }
  while(a);

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// START NEW REPEAT MODELS IF THERE IS STILL SPACE
//                         
void StartMultipleRMs(RCLASS *C, uint8_t *b)
  {
  uint64_t idx = GetIdx(b, C); // DO NOT SET IT INSIDE THE NEXT IF!
			       
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
  double F[4] = {0,0,0,0};

  uint32_t r;
  for(r = 0 ; r < C->nRM ; ++r)
    {
    RMODEL *R = &C->RM[r];
    ComputeRMProbs(C, R, b);
    double rmw = R->weight;
    F[0] += R->probs[0] * rmw;
    F[1] += R->probs[1] * rmw;
    F[2] += R->probs[2] * rmw;
    F[3] += R->probs[3] * rmw;
    }

  M->sum  = (M->freqs[0] = 1 + (uint32_t)(F[0] * MAXC));
  M->sum += (M->freqs[1] = 1 + (uint32_t)(F[1] * MAXC));
  M->sum += (M->freqs[2] = 1 + (uint32_t)(F[2] * MAXC));
  M->sum += (M->freqs[3] = 1 + (uint32_t)(F[3] * MAXC));

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
