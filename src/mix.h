#ifndef MIX_H_INCLUDED
#define MIX_H_INCLUDED

#include <stdint.h>
#include "nn.h"

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
  uint32_t symlog_pos;

  uint32_t *sf1;
  uint32_t *sf2;
  uint32_t *sf3;

  float *tdata;
  int last_target;

  int *ignorant;
  int *hitmask;

  float smean;
  float lmean;

  uint8_t symlogs1;
  uint8_t symlogs2;
  uint8_t symlogs3;
  }
mix_state_t;

mix_state_t* mix_init(uint32_t nmodels, uint32_t nsymbols, uint32_t hs);
float const* mix(mix_state_t* mxs, float **probs);
void calc_aggregates(mix_state_t* mxs, float **probs, uint8_t sym);
void mix_update_state(mix_state_t* mxs, float **probs, uint8_t sym, float lr);
void mix_free(mix_state_t* mxs);

#endif
