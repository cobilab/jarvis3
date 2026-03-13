#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "common.h"
#include "mix.h"

static inline int feq(float a, float b)
  {
  return fabsf(a - b) < 1e-7f;
  }

static inline float stretch(float p)
  {
  const float eps = 1e-6f;

  if(p < eps)
    p = eps;
  else if(p > 1.0f - eps)
    p = 1.0f - eps;

  return fasterlog2(p / (1.0f - p));
  }

mix_state_t* mix_init(uint32_t nmodels, uint32_t nsymbols, uint32_t hs)
  {
  mix_state_t *mxs = malloc(sizeof(mix_state_t));

  mxs->nmodels  = nmodels;
  mxs->nsymbols = nsymbols;

  {
  int model_derived = 3;
  int seq_derived   = 3;
  int xs = (nmodels * (nsymbols + model_derived)) + (seq_derived * nsymbols) + 1;
  xs++;

  mxs->ann = ann_init(xs, hs, nsymbols);
  mxs->ann->x[xs - 1] = 1.0f;
  }

  mxs->symlogs1 = 8;
  mxs->symlogs2 = 16;
  mxs->symlogs3 = 64;

  mxs->symlog = calloc(mxs->symlogs3, sizeof(uint8_t));
  mxs->symlog_pos = 0;

  mxs->sf1 = calloc(nsymbols, sizeof(uint32_t));
  mxs->sf2 = calloc(nsymbols, sizeof(uint32_t));
  mxs->sf3 = calloc(nsymbols, sizeof(uint32_t));

  mxs->sf1[0] = mxs->symlogs1;
  mxs->sf2[0] = mxs->symlogs2 - mxs->symlogs1;
  mxs->sf3[0] = mxs->symlogs3;

  mxs->hit  = calloc(nmodels, sizeof(float));
  mxs->best = calloc(nmodels, sizeof(float));
  mxs->bits = calloc(nmodels, sizeof(float));

  mxs->tdata = calloc(nsymbols, sizeof(float));
  mxs->last_target = -1;

  mxs->ignorant = calloc(nmodels, sizeof(int));
  mxs->hitmask  = calloc(nmodels, sizeof(int));

  mxs->nnbits = 0.0f;

  {
  float mean = 1.0f / mxs->nsymbols;
  mxs->smean = stretch(mean);
  mxs->lmean = fasterlog2(mean);
  }

  return mxs;
  }

float const* mix(mix_state_t* mxs, float **probs)
  {
  float *ret = mxs->ann->y;
  uint32_t i, j, k = 0;

  const float smean = mxs->smean;
  const uint32_t nmodels = mxs->nmodels;
  const uint32_t nsymbols = mxs->nsymbols;

  for(i = 0; i < nmodels; ++i)
    {
    for(j = 0; j < nsymbols; ++j)
      mxs->ann->x[k++] = stretch(probs[i][j]) - smean;

    mxs->ann->x[k++] = mxs->hit[i];
    mxs->ann->x[k++] = mxs->best[i];
    mxs->ann->x[k++] = mxs->bits[i];
    }

  mxs->ann->x[k++] = mxs->nnbits;

  for(i = 0; i < nsymbols; ++i)
    {
    mxs->ann->x[k++] = (((float)mxs->sf1[i] / mxs->symlogs1) - 0.5f) * 2.0f;
    mxs->ann->x[k++] = (((float)mxs->sf2[i] / (mxs->symlogs2 - mxs->symlogs1)) - 0.5f) * 2.0f;
    mxs->ann->x[k++] = (((float)mxs->sf3[i] / mxs->symlogs3) - 0.5f) * 2.0f;
    }

  ann_apply(mxs->ann);

  return ret;
  }

void calc_aggregates(mix_state_t* mxs, float **probs, uint8_t sym)
  {
  const float lmean = mxs->lmean;
  const float a = 0.15f;
  const float na = 1.0f - a;

  const float nnb = -fasterlog2(mxs->ann->y[sym]) + lmean;
  mxs->nnbits = (a * nnb) + (na * mxs->nnbits);

  {
  const uint32_t pos = mxs->symlog_pos;
  const uint32_t n3  = mxs->symlogs3;

  const uint8_t old63 = mxs->symlog[pos];
  const uint8_t old15 = mxs->symlog[(pos + n3 - mxs->symlogs2 + 1) % n3];
  const uint8_t old7  = mxs->symlog[(pos + n3 - mxs->symlogs1 + 1) % n3];

  mxs->sf3[old63]--;
  mxs->sf3[sym]++;

  mxs->sf1[old7]--;
  mxs->sf1[sym]++;

  mxs->sf2[old15]--;
  mxs->sf2[old7]++;

  mxs->symlog[pos] = sym;
  mxs->symlog_pos = (pos + 1) % n3;
  }

  {
  const float alpha = 0.5f;
  const float nalpha = 1.0f - alpha;
  const uint32_t nmodels = mxs->nmodels;
  const uint32_t nsymbols = mxs->nsymbols;

  uint32_t i, j;
  float bestp = probs[0][sym];
  int *ignorant = mxs->ignorant;
  int *hit = mxs->hitmask;

  for(i = 0; i < nmodels; ++i)
    {
    const float psym = probs[i][sym];
    const float p0 = probs[i][0];

    if(bestp < psym)
      bestp = psym;

    ignorant[i] = 1;
    hit[i] = (psym >= p0);

    for(j = 1; j < nsymbols; ++j)
      {
      ignorant[i] = ignorant[i] && feq(p0, probs[i][j]);
      hit[i] = hit[i] && (psym >= probs[i][j]);
      }
    }

  for(i = 0; i < nmodels; ++i)
    {
    const float psym = probs[i][sym];
    const float safe_psym = (psym < 1e-6f) ? 1e-6f : psym;
    const float t = -fasterlog2(safe_psym) + lmean;
    mxs->bits[i] = (alpha * t) + (nalpha * mxs->bits[i]);

    if(!ignorant[i])
      {
      mxs->best[i] += feq(bestp, psym) ? 0.1f : -0.1f;
      mxs->best[i] = fminf(1.0f,  mxs->best[i]);
      mxs->best[i] = fmaxf(-1.0f, mxs->best[i]);

      mxs->hit[i] += hit[i] ? 0.1f : -0.1f;
      mxs->hit[i] = fminf(1.0f,  mxs->hit[i]);
      mxs->hit[i] = fmaxf(-1.0f, mxs->hit[i]);
      }
    }
  }
  }

void mix_update_state(mix_state_t* mxs, float **probs, uint8_t sym, float lr)
  {
  calc_aggregates(mxs, probs, sym);

  if(mxs->last_target >= 0)
    mxs->tdata[mxs->last_target] = 0.0f;

  mxs->tdata[sym] = 1.0f;
  mxs->last_target = sym;

  ann_train(mxs->ann, mxs->tdata, lr);
  }

void mix_free(mix_state_t* mxs)
  {
  ann_free(mxs->ann);

  free(mxs->hit);
  free(mxs->best);
  free(mxs->bits);

  free(mxs->symlog);
  free(mxs->sf1);
  free(mxs->sf2);
  free(mxs->sf3);

  free(mxs->tdata);

  free(mxs->ignorant);
  free(mxs->hitmask);

  free(mxs);
  }

