#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#include "nn.h"
#include "common.h"

static inline float Sig(float x)
  {
  if(x < -45.0f)
    x = -45.0f;

  return 1.0f / (1.0f + Power(2, 1.442695040f * (-x)));
  }

ann_t* ann_init(uint64_t xs, uint64_t hs, uint64_t ys)
  {
  ann_t *ann = (ann_t *) malloc(sizeof(ann_t));

  ann->xs = xs;
  ann->hs = hs;
  ann->ys = ys;

  ann->x = (float *) calloc(xs, sizeof(float));
  ann->h = (float *) calloc(hs, sizeof(float));
  ann->y = (float *) calloc(ys, sizeof(float));

  ann->wxh = (float *) calloc(xs * hs, sizeof(float));
  ann->why = (float *) calloc((hs + 1) * ys, sizeof(float));

  ann->d1 = (float *) calloc(ys, sizeof(float));
  ann->d2 = (float *) calloc(hs, sizeof(float));

  {
  float *w1 = ann->wxh;
  float *w2 = ann->why;
  uint64_t i;
  float r;

  const float s1 = sqrtf(6.0f / (xs + hs));
  const float s2 = sqrtf(6.0f / (ys + hs + 1));

  for(i = 0; i < xs * hs; ++i)
    {
    r = ((float) drand48() * 2.0f) - 1.0f;
    *w1++ = r * s1;
    }

  for(i = 0; i < (hs + 1) * ys; ++i)
    {
    r = ((float) drand48() * 2.0f) - 1.0f;
    *w2++ = r * s2;
    }
  }

  return ann;
  }

void ann_apply(ann_t *ann)
  {
  uint64_t i, j;
  const uint64_t xs = ann->xs;
  const uint64_t hs = ann->hs;
  const uint64_t ys = ann->ys;

  float *h = ann->h;
  float *y = ann->y;
  float *w1 = ann->wxh;

  for(i = 0; i < xs; ++i)
    {
    const float xi = ann->x[i];
    for(j = 0; j < hs; ++j)
      h[j] += (*w1++) * xi;
    }

  {
  float *w2 = ann->why;

  for(i = 0; i < hs; ++i)
    {
    const float hi = Sig(h[i]);
    h[i] = hi;

    if(ys == 4)
      {
      y[0] += (*w2++) * hi;
      y[1] += (*w2++) * hi;
      y[2] += (*w2++) * hi;
      y[3] += (*w2++) * hi;
      }
    else
      {
      for(j = 0; j < ys; ++j)
        y[j] += (*w2++) * hi;
      }
    }

  if(ys == 4)
    {
    y[0] = Sig(y[0] + (*w2++));
    y[1] = Sig(y[1] + (*w2++));
    y[2] = Sig(y[2] + (*w2++));
    y[3] = Sig(y[3] + (*w2++));
    }
  else
    {
    for(i = 0; i < ys; ++i)
      y[i] = Sig(y[i] + (*w2++));
    }
  }
  }

void ann_train(ann_t *ann, float *t, float learning_rate)
  {
  uint64_t i, j;
  const uint64_t xs = ann->xs;
  const uint64_t hs = ann->hs;
  const uint64_t ys = ann->ys;

  float *d1 = ann->d1;
  float *d2 = ann->d2;
  float *y = ann->y;
  float *h = ann->h;

  if(ys == 4)
    {
    d1[0] = y[0] * (1.0f - y[0]) * (t[0] - y[0]);
    d1[1] = y[1] * (1.0f - y[1]) * (t[1] - y[1]);
    d1[2] = y[2] * (1.0f - y[2]) * (t[2] - y[2]);
    d1[3] = y[3] * (1.0f - y[3]) * (t[3] - y[3]);
    }
  else
    {
    for(i = 0; i < ys; ++i)
      d1[i] = y[i] * (1.0f - y[i]) * (t[i] - y[i]);
    }

  {
  float *w2 = ann->why;

  for(i = 0; i < hs; ++i)
    {
    float acc = 0.0f;
    const float hi = h[i];

    if(ys == 4)
      {
      acc += w2[0] * d1[0]; w2[0] += learning_rate * hi * d1[0];
      acc += w2[1] * d1[1]; w2[1] += learning_rate * hi * d1[1];
      acc += w2[2] * d1[2]; w2[2] += learning_rate * hi * d1[2];
      acc += w2[3] * d1[3]; w2[3] += learning_rate * hi * d1[3];
      w2 += 4;
      }
    else
      {
      for(j = 0; j < ys; ++j)
        {
        acc += (*w2) * d1[j];
        *w2++ += learning_rate * hi * d1[j];
        }
      }

    d2[i] = acc * hi * (1.0f - hi);
    h[i] = 0.0f;
    }

  if(ys == 4)
    {
    w2[0] += learning_rate * d1[0];
    w2[1] += learning_rate * d1[1];
    w2[2] += learning_rate * d1[2];
    w2[3] += learning_rate * d1[3];

    y[0] = 0.0f;
    y[1] = 0.0f;
    y[2] = 0.0f;
    y[3] = 0.0f;
    }
  else
    {
    for(i = 0; i < ys; ++i)
      {
      *w2++ += learning_rate * d1[i];
      y[i] = 0.0f;
      }
    }
  }

  {
  float *w1 = ann->wxh;

  for(i = 0; i < xs; ++i)
    {
    const float xi = ann->x[i];
    for(j = 0; j < hs; ++j)
      *w1++ += learning_rate * xi * d2[j];
    }
  }
  }

void ann_free(ann_t *ann)
  {
  free(ann->wxh);
  free(ann->why);
  free(ann->x);
  free(ann->h);
  free(ann->y);
  free(ann->d1);
  free(ann->d2);
  free(ann);
  }
