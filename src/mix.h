#include "nn.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct mix_state_t
  {
  uint32_t nmodels;
  uint32_t nsymbols;

  ann_t *ann;

  float *hit;
  float *best;
  float *bits;
  float nnbits;
  uint8_t *symlog;

  float smean;
  float lmean;

  uint8_t symlogs1;
  uint8_t symlogs2;
  uint8_t symlogs3;
  }
mix_state_t;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

mix_state_t    *mix_init         (uint32_t, uint32_t, uint32_t);
float const    *mix              (mix_state_t *, float **);
void           mix_update_state  (mix_state_t *, float **, uint8_t, float);
void           mix_free          (mix_state_t *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
