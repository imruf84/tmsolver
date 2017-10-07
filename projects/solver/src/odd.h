#ifndef __ODD_H__
#define __ODD_H__

#include <stdio.h>
#include <math.h>
#include "stdint.h"

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

typedef struct
{
    uint32_t hash;
    double odds[3];
} odd_t;

uint32_t odds_to_hash(double *o);
void hash_to_odds(uint32_t hash, double (*odds)[3]);
void odd_print(odd_t *o);

#endif // __ODD_H__
