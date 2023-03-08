#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "repeats.h"
#include "common.h"
#include "dna.h"
#include "mem.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// FAST HASH
//
uint64_t FHASH(uint64_t z){
  z = (~z) + (z << 21);
  z = z    ^ (z >> 24);
  z = (z   + (z << 3)) + (z << 8);
  z = z    ^ (z >> 14);
  z = (z   + (z << 2)) + (z << 4);
  z = z    ^ (z >> 28);
  z = z    + (z << 31);
  return z;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// SHIFT BUFFER TO RIGHT. PERHAPS A CIRCULAR BUFFER IS A BETTER APPROACH...
//
void ShiftRBuf(uint8_t *b, int32_t s, uint8_t n){
  memmove(b, b+1, s * sizeof(uint8_t));
  b[s-1] = n;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// GET NUMERICAL BASE FROM PACKED SEQUENCE BY ID. THE ID%4 IS GIVEN BY THE 2
// LESS SIGNIFICATIVE BITS (ID&3).
//
uint8_t GetNBase(uint8_t *b, uint64_t i){
  return (uint8_t) (((0x3<<((3-(i&0x3))<<1)) & b[i>>2])>>((3-(i&0x3))<<1));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// CREATES THE RCLASS BASIC STRUCTURE 
//
RCLASS *CreateRC(uint32_t m, double a, double b, uint32_t l, uint32_t c,
double g, uint8_t i, double w, uint64_t s){
  RCLASS *C      = (RCLASS   *) Calloc(1,     sizeof(RCLASS  ));
  C->hash        = (RHASH    *) Calloc(1,     sizeof(RHASH   ));
  C->P           = (RPARAM   *) Calloc(1,     sizeof(RPARAM  ));
  C->hash->ent   = (RENTRY  **) Calloc(HSIZE, sizeof(RENTRY *));
  C->hash->size  = (uint32_t *) Calloc(HSIZE, sizeof(uint32_t));
  C->RM          = (RMODEL   *) Calloc(m,     sizeof(RMODEL  ));
  C->hash->max_c = MAX_HASH_COL;
  C->mRM         = m;
  C->P->rev      = i;
  C->P->alpha    = ((int)(a*65534))/65534.0;
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
// THRESHOLD REVERSE COMPLEMENT INDEX BASED ON PAST SYMBOLS
//
uint64_t GetTIdxRev(RCLASS *C, uint8_t p_current){
  return (C->P->c_idxRev = (C->P->c_idxRev>>2)+CompNum(p_current)*C->P->mult);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// THRESHOLD INDEX CALC BASED ON PAST SYMBOLS
//
uint64_t GetTIdx(RCLASS *C, uint8_t p_current, uint8_t p_current_less_k){
  return (C->P->c_idx = ((C->P->c_idx-p_current_less_k*C->P->mult)<<2)+p_current);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// GET REPEAT MODEL RHASH RENTRY
//
RENTRY *GetHEnt(RCLASS *C, uint64_t key){

  uint32_t n, h = (FHASH(key) % HSIZE);
  uint16_t b = key & 0xffff;

  for(n = 0 ; n < C->hash->size[h] ; ++n)
    if(C->hash->ent[h][n].key == b)
      return &C->hash->ent[h][n];

  return NULL;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// START EACH REPEAT MODEL, CURRENTLY BY RANDOM STORED POSITION 
//
int32_t StartRM(RCLASS *C, uint32_t m, uint64_t i, uint8_t r){
  uint32_t s;
  RENTRY *E;

  if((E = GetHEnt(C, i)) == NULL)
    return 0;

  uint64_t idx;
  if(E->nPos > 0)
    idx = rand() % E->nPos;
  else
    idx = 0;

  if(r == 0) // REGULAR REPEAT
    C->RM[m].pos = E->pos[ idx ];
  else{ // INVERTED REPEAT
    if(E->pos[ idx ] <= C->P->ctx+1) 
      return 0;
    C->RM[m].pos = E->pos[ idx ] - C->P->ctx - 1;
    }

  C->RM[m].nHits  = 0;
  C->RM[m].nTries = 0;
  C->RM[m].rev    = r;
  C->RM[m].acting = 0;
  C->RM[m].weight = C->P->iWeight;
  for(s = 0 ; s < NSYM ; ++s)
    C->RM[m].probs[s] = 0;

  return 1;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// REMOVE KMER POSITION OF RHASH TABLE
//
void RemoveKmerPos(RCLASS *C, uint8_t *block){

  int64_t previous = 0;
  if(C->P->c_pos > C->P->ctx)
    previous = C->P->c_pos-C->P->ctx;
  uint64_t key = GetTIdx(C, GetNBase(block, C->P->c_pos), GetNBase(block, previous));

  if(C->P->c_pos++ > C->P->c_max){
 
    uint64_t n, x, h = (FHASH(key) % HSIZE);
    uint16_t b = key & 0xffff;

    for(n = 0 ; n < C->hash->size[h] ; ++n)
      if(C->hash->ent[h][n].key == b){

        if(C->hash->ent[h][n].nPos < 1){
          fprintf(stderr, "Error: removing kmer exception found!\n");
	  fprintf(stderr, "Error: kmer was never inserted in RHASH!\n");
          exit(1);
          }
        else if(C->hash->ent[h][n].nPos == 1){
	  return;
	  }	
	else if(C->hash->ent[h][n].nPos > 1){
	  for(x = C->hash->ent[h][n].nPos - 1 ; x > 0 ; x--)
            C->hash->ent[h][n].pos[x-1] = C->hash->ent[h][n].pos[x];    
	      
          C->hash->ent[h][n].nPos--;
          C->hash->ent[h][n].pos = (POS_PREC *) Realloc(C->hash->ent[h][n].pos,
          (C->hash->ent[h][n].nPos) * sizeof(POS_PREC));
	  }

        return;
        }
    }

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// INSERT KMER POSITION INTO RHASH TABLE 
//
void InsertKmerPos(RCLASS *C, uint64_t key, POS_PREC pos){

  uint64_t n, h = (FHASH(key) % HSIZE);
  uint16_t b = key & 0xffff;

  for(n = 0 ; n < C->hash->size[h] ; ++n)
    if(C->hash->ent[h][n].key == b){

      if(C->hash->ent[h][n].nPos >= C->hash->max_c){  // PROTECTION FOR MAX BITS
	C->hash->ent[h][n].nPos = 128; // RESET TO 128 ON MAX
        C->hash->ent[h][n].pos = (POS_PREC *) Realloc(C->hash->ent[h][n].pos,
                                 (C->hash->ent[h][n].nPos) * sizeof(POS_PREC));
        C->hash->ent[h][n].pos[0] = pos;
        C->hash->ent[h][n].key    = (uint16_t) (key & 0xffff);
        // fprintf(stderr, "Warning: reached MAX! of C->hash->max_c!\n");
	return;
        }

      C->hash->ent[h][n].pos = (POS_PREC *) Realloc(C->hash->ent[h][n].pos, 
      (C->hash->ent[h][n].nPos + 1) * sizeof(POS_PREC));
      C->hash->ent[h][n].pos[C->hash->ent[h][n].nPos] = pos; 
      C->hash->ent[h][n].nPos++;
      // STORE THE LAST K-MER POSITION
      return;
      }

  // CREATE A NEW RENTRY
  C->hash->ent[h] = (RENTRY *) Realloc(C->hash->ent[h],  
                    (C->hash->size[h]+1) * sizeof(RENTRY));
  
  // CREATE A NEW POSITION
  C->hash->ent[h][C->hash->size[h]].pos    = (POS_PREC *) Calloc(1, 
		                             sizeof(POS_PREC));
  C->hash->ent[h][C->hash->size[h]].nPos   = 1;
  C->hash->ent[h][C->hash->size[h]].pos[0] = pos;
  C->hash->ent[h][C->hash->size[h]].key    = (uint16_t) (key & 0xffff);
  C->hash->size[h]++;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// COMPUTE REPEAT MODEL PROBABILITIES
//
void ComputeRMProbs(RCLASS *C, RMODEL *R, uint8_t *b){
  
  uint8_t n, s;
  
  s = (R->rev != 0) ? CompNum(GetNBase(b, R->pos)) : GetNBase(b, R->pos);

  R->probs[s] = (R->nHits+C->P->alpha) / (R->nTries+2*C->P->alpha);
  for(n = 0 ; n < NSYM ; ++n)
    if(n != s){
      R->probs[n] = (1-R->probs[s])/3;
      }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// UPDATE REPEAT MODEL
//
void UpdateRM(RMODEL *R, uint8_t *b, uint8_t s){

  R->lastHit = 1;

  if(R->rev == 0){
    if(GetNBase(b, R->pos++) == s){
      R->nHits++;
      R->lastHit = 0;
      }
    }
  else{
    if(CompNum(GetNBase(b, R->pos--)) == s){
      R->nHits++;
      R->lastHit = 0;
      }
    }

  R->nTries++;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// RENORMALIZE REPEAT WEIGHTS
//
void RenormWeights(RCLASS *C){
  uint32_t n;
  double   t = 0;
  for(n = 0 ; n < C->nRM ; ++n)
    t += C->RM[n].weight;
  for(n = 0 ; n < C->nRM ; ++n)
    C->RM[n].weight /= t;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// STOP USELESS REPEAT MODELS
//
void StopRM(RCLASS *C){
  uint32_t n, a;
  do{
    a = 0;
    for(n = 0 ; n < C->nRM ; ++n)
      if((C->RM[n].acting = C->P->beta * C->RM[n].acting + C->RM[n].lastHit) > 
      C->P->limit * 1.0 || C->RM[n].pos == 0)
        {
        if(n != C->nRM-1)
          C->RM[n] = C->RM[C->nRM-1];
        C->nRM--;
        a = 1;
        break;
        }
    }
  while(a);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// START NEW REPEAT MODELS IF THERE IS STILL SPACE
//                         
void StartMultipleRMs(RCLASS *C, uint8_t *b){
  
  uint64_t idx = GetIdx(b, C); // DO NOT SET IT INSIDE THE NEXT IF!
  if(C->P->rev != 2)
    if(C->nRM < C->mRM && StartRM(C, C->nRM, idx, 0))
      C->nRM++;

  if(C->P->rev != 0){
    uint64_t idx_rev = GetIdxRev(b, C);
    if(C->nRM < C->mRM && StartRM(C, C->nRM, idx_rev, 1))
      C->nRM++;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// COMPUTE AND EXTRACT MIXTURED PROBABILITIES
//
void ComputeMixture(RCLASS *C, PMODEL *M, uint8_t *b){
  uint32_t r, s; 
  double F[NSYM] = {0,0,0,0};
  
  for(r = 0 ; r < C->nRM ; ++r){
    ComputeRMProbs(C, &C->RM[r], b);
    for(s = 0 ; s < NSYM ; ++s)
      F[s] += C->RM[r].probs[s] * C->RM[r].weight;
    }

  M->sum = 0;
  for(s = 0 ; s < NSYM ; ++s){
    M->freqs[s] = 1 + (uint32_t)(F[s] * MAXC);
    M->sum += M->freqs[s];
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// UPDATE WEIGHTS ACCORDING TO THE PERFORMANCE OF EACH REPEAT MODEL
//
void UpdateWeights(RCLASS *C, uint8_t *b, uint8_t s){
  uint32_t r;
  for(r = 0 ; r < C->nRM ; ++r){
    C->RM[r].weight = Power(C->RM[r].weight, C->P->gamma) * C->RM[r].probs[s];
    UpdateRM(&C->RM[r], b, s);
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
