/*------------------------------------------------------------------------------

Copyright 2010 IEETA / University of Aveiro, All Rights Reserved.

These programs are supplied free of charge for research purposes only,
and may not be sold or incorporated into any commercial product. There is
ABSOLUTELY NO WARRANTY of any sort, nor any undertaking that they are
fit for ANY PURPOSE WHATSOEVER. Use them at your own risk. If you do
happen to find a bug, or have modifications to suggest, please report
the same to Armando J. Pinho, ap@ua.pt. The copyright notice above
and this statement of conditions must remain an integral part of each
and every copy made of these files.

Adapted by D. Pratas, 2026

------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "arith.h"
#include "bitio.h"
#include "arith_aux.h"

/*----------------------------------------------------------------------------*/

static inline int PrefixSumToSymbol(const int *count, int symbol)
	{
	int sum = 0;
	const int *p = count;
	int n = symbol;

	while(n >= 4)
		{
		sum += p[0] + p[1] + p[2] + p[3];
		p += 4;
		n -= 4;
		}

	while(n--)
		sum += *p++;

	return sum;
	}

/*----------------------------------------------------------------------------*/

static inline int FindSymbolLinear(const int *count, int target, int nSymbols,
  int *low, int *high)
	{
	int base = 0;
	int n = 0;
	const int *p = count;

	assert(nSymbols > 0);

	while(n + 4 <= nSymbols)
		{
		int h0 = base + p[0];
		if(h0 > target)
			{
			*low = base;
			*high = h0;
			return n;
			}

		int h1 = h0 + p[1];
		if(h1 > target)
			{
			*low = h0;
			*high = h1;
			return n + 1;
			}

		int h2 = h1 + p[2];
		if(h2 > target)
			{
			*low = h1;
			*high = h2;
			return n + 2;
			}

		int h3 = h2 + p[3];
		if(h3 > target)
			{
			*low = h2;
			*high = h3;
			return n + 3;
			}

		base = h3;
		p += 4;
		n += 4;
		}

	while(n < nSymbols)
		{
		int h = base + *p++;
		if(h > target)
			{
			*low = base;
			*high = h;
			return n;
			}

		base = h;
		++n;
		}

	assert(0 && "FindSymbolLinear: target out of range");
	*low = 0;
	*high = 0;
	return 0;
	}

/*----------------------------------------------------------------------------*/

void GetInterval(int *low, int *high, int *count, int symbol)
	{
	int l;

	assert(symbol >= 0);

	if(symbol == 0)
		{
		*low = 0;
		*high = count[0];
		return;
		}

	if(symbol == 1)
		{
		l = count[0];
		*low = l;
		*high = l + count[1];
		return;
		}

	l = PrefixSumToSymbol(count, symbol);
	*low = l;
	*high = l + count[symbol];
	}

/*----------------------------------------------------------------------------*/

int GetSymbol(int *low, int *high, int *count, int target, int nSymbols)
	{
	assert(nSymbols > 0);

	if(nSymbols == 2)
		{
		int c0 = count[0];
		if(target < c0)
			{
			*low = 0;
			*high = c0;
			return 0;
			}

		*low = c0;
		*high = c0 + count[1];
		return 1;
		}

	return FindSymbolLinear(count, target, nSymbols, low, high);
	}

/*----------------------------------------------------------------------------*/

void WriteNBits(uint64_t bits, int nBits, FILE *oFp)
	{
	while(nBits--)
		{
		int bit = (int)((bits >> nBits) & 0x1u);
		arithmetic_encode(bit, bit + 1, 2, oFp);
		}
	}

/*----------------------------------------------------------------------------*/

uint64_t ReadNBits(int nBits, FILE *iFp)
	{
	uint64_t bits = 0;

	while(nBits--)
		{
		int bit = arithmetic_decode_target(2);
		bits = (bits << 1) | (uint64_t) bit;
		arithmetic_decode(bit, bit + 1, 2, iFp);
		}

	return bits;
	}

/*----------------------------------------------------------------------------*/

void AESym(int symbol, int *counters, int totalCount, FILE *oFp)
	{
	int low, high;

	GetInterval(&low, &high, counters, symbol);
	arithmetic_encode(low, high, totalCount, oFp);
	}

/*----------------------------------------------------------------------------*/

int ArithDecodeSymbol(int nSymbols, int *counters, int totalCount, FILE *iFp)
	{
	int low, high;
	int target = arithmetic_decode_target(totalCount);
	int symbol = GetSymbol(&low, &high, counters, target, nSymbols);

	arithmetic_decode(low, high, totalCount, iFp);
	return symbol;
	}
