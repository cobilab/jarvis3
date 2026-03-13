#ifndef REPEATS_H_INCLUDED
#define REPEATS_H_INCLUDED

#include <stdint.h>
#include "defs.h"
#include "pmodels.h"

#define MAX_BUF          2000000
#define NSYM             4
#define MAXC             ((1<<(sizeof(uint16_t)*8))-1)
#define DEF_MRM          50
#define DEF_KMER         11
#define DEF_ALPHA        1
#define DEF_GAMMA        0.2
#define DEF_BETA         0.90
#define DEF_LIMIT        7
#define DEF_REV          0
#define DEF_MODE         0
#define INIWEIGHT        0.01
#define MAX_LT           250

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct
  {
  double   beta;
  double   gamma;
  double   iWeight;
  uint32_t limit;
  uint32_t ctx;
  uint64_t mult;
  uint64_t idx;
  uint64_t idxRev;
  uint64_t c_pos;
  uint64_t c_max;
  uint64_t c_idx;
  uint64_t c_idxRev;
  uint8_t  rev;
  }
RPARAM;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct
  {
  uint32_t *array;
  uint64_t size;
  uint32_t nPos;
  uint32_t nPosAnd1;
  }
RTABLE;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct
  {
  uint32_t pos;
  uint32_t nHits;
  uint32_t nTries;
  double   probs[NSYM];
  double   weight;
  double   acting;
  uint8_t  rev;
  uint8_t  lastHit;
  }
RMODEL;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct
  {
  RPARAM  *P;
  RTABLE  *T;
  RMODEL  *RM;
  uint32_t nRM;
  uint32_t mRM;
  }
RCLASS;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline uint8_t GetNBase(const uint8_t *b, uint64_t i)
  {
  return (uint8_t) ((b[i >> 2] >> (((3 - (i & 0x3)) << 1))) & 0x3);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void     FillLT          (void);
RCLASS  *CreateRC        (uint32_t, double, uint32_t, uint32_t, double,
                          uint8_t, double, uint64_t);
uint64_t GetIdxRev       (uint8_t *, RCLASS *);
uint64_t GetIdx          (uint8_t *, RCLASS *);
int32_t  StartRM         (RCLASS *, uint32_t, uint64_t, uint8_t);
void     AddKmerPos      (RTABLE *, uint64_t, uint32_t);
void     ComputeRMProbs  (RCLASS *, RMODEL *, uint8_t *);
void     UpdateRM        (RMODEL *, uint8_t *, uint8_t);
void     RenormWeights   (RCLASS *);
void     StopRM          (RCLASS *);
void     StartMultipleRMs(RCLASS *, uint8_t *);
void     ComputeMixture  (RCLASS *, PMODEL *, uint8_t *);
void     UpdateWeights   (RCLASS *, uint8_t *, uint8_t);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
