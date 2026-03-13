#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "arith.h"
#include "bitio.h"

/*
 * Byte-oriented range coder.
 *
 * This is a classic carry-safe range coder layout:
 *   - encoder state: low/range + delayed carry cache
 *   - decoder state: code/range
 *   - renormalize whenever range shrinks below RC_TOP
 *
 * The public API intentionally remains unchanged so the rest of JARVIS3 can
 * continue to call arithmetic_encode(), arithmetic_decode_target(),
 * arithmetic_decode(), start_encode(), finish_encode(), etc.
 */

#define RC_TOP   ((uint32_t)1u << 24)
#define RC_INIT_RANGE ((uint32_t)0xFFFFFFFFu)

/* encoder state */
static uint64_t rc_low;
static uint32_t rc_range;
static uint8_t  rc_cache;
static uint64_t rc_cache_size;

/* decoder state */
static uint32_t rc_code;
static uint32_t rc_drange;
static uint32_t rc_div;

static inline void rc_output_byte(uint8_t b, FILE *s)
{
  OUTPUT_BYTE(b, s);
}

static inline int rc_input_byte(FILE *s)
{
  int c = INPUT_BYTE(s);
  return c == EOF ? 0 : (c & 0xFF);
}

static inline void rc_shift_low(FILE *s)
{
  if ((uint32_t)rc_low < 0xFF000000u || (uint32_t)(rc_low >> 32) != 0) {
    uint8_t temp = rc_cache;
    do {
      rc_output_byte((uint8_t)(temp + (uint8_t)(rc_low >> 32)), s);
      temp = 0xFF;
    } while (--rc_cache_size != 0);

    rc_cache = (uint8_t)((rc_low >> 24) & 0xFF);
  }

  ++rc_cache_size;
  rc_low = ((uint32_t)rc_low) << 8;
}

static inline void rc_encode_normalize(FILE *s)
{
  while (rc_range < RC_TOP) {
    rc_range <<= 8;
    rc_shift_low(s);
  }
}

static inline void rc_decode_normalize(FILE *s)
{
  while (rc_drange < RC_TOP) {
    rc_drange <<= 8;
    rc_code = (rc_code << 8) | (uint32_t)rc_input_byte(s);
  }
}

void arithmetic_encode(freq_value low, freq_value high, freq_value total,
  FILE *s)
{
  uint32_t r;
  uint64_t temp;

  if (total == 0) {
    fprintf(stderr, "Invalid total frequency 0 in arithmetic_encode()\n");
    exit(1);
  }

  r = rc_range / (uint32_t) total;
  if (r == 0) {
    fprintf(stderr, "Range coder underflow in arithmetic_encode()\n");
    exit(1);
  }

  temp = (uint64_t) r * (uint32_t) low;
  rc_low += temp;

  if (high < total)
    rc_range = r * (uint32_t)(high - low);
  else
    rc_range -= (uint32_t) temp;

  rc_encode_normalize(s);
}

freq_value arithmetic_decode_target(freq_value total)
{
  uint32_t target;

  if (total == 0) {
    fprintf(stderr, "Invalid total frequency 0 in arithmetic_decode_target()\n");
    exit(1);
  }

  rc_div = rc_drange / (uint32_t) total;
  if (rc_div == 0) {
    fprintf(stderr, "Range coder underflow in arithmetic_decode_target()\n");
    exit(1);
  }

  target = rc_code / rc_div;
  return (target >= total) ? (total - 1) : target;
}

void arithmetic_decode(freq_value low, freq_value high, freq_value total,
  FILE *s)
{
  uint32_t temp;

  (void) total;

  temp = rc_div * (uint32_t) low;
  rc_code -= temp;

  if (high < total)
    rc_drange = rc_div * (uint32_t)(high - low);
  else
    rc_drange -= temp;

  rc_decode_normalize(s);
}

void start_encode(void)
{
  rc_low = 0;
  rc_range = RC_INIT_RANGE;
  rc_cache = 0;
  rc_cache_size = 1;
}

void finish_encode(FILE *s)
{
  int i;
  for (i = 0; i < 5; ++i)
    rc_shift_low(s);
}

void start_decode(FILE *s)
{
  int i;

  rc_code = 0;
  rc_drange = RC_INIT_RANGE;

  for (i = 0; i < 5; ++i)
    rc_code = (rc_code << 8) | (uint32_t) rc_input_byte(s);
}

void finish_decode(void)
{
  /* No action required. */
}

void binary_arithmetic_encode(freq_value c0, freq_value c1, int bit, FILE *s)
{
  freq_value total = c0 + c1;
  if (bit)
    arithmetic_encode(c0, total, total, s);
  else
    arithmetic_encode(0, c0, total, s);
}

int binary_arithmetic_decode(freq_value c0, freq_value c1, FILE *s)
{
  freq_value total = c0 + c1;
  freq_value target = arithmetic_decode_target(total);

  if (target < c0) {
    arithmetic_decode(0, c0, total, s);
    return 0;
  }

  arithmetic_decode(c0, total, total, s);
  return 1;
}
