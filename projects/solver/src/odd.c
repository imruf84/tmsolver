#include "odd.h"

void odd_print(odd_t *o)
{
    printf("%u %.2lf %.2lf %.2lf\n", o->hash, o->odds[0], o->odds[1], o->odds[2]);
}

uint32_t odds_to_hash(double *o)
{
    // Szorzók sorba rendezése.
    uint32_t oi0 = round(o[0]*100);
    uint32_t oi1 = round(o[1]*100);
    uint32_t oi2 = round(o[2]*100);

    uint32_t o0 = max(max(oi0, oi1), oi2);
    uint32_t o2 = min(min(oi0, oi1), oi2);
    uint32_t o1 = oi0 + oi1 + oi2 - o0 - o2;

    return o0 * 1000000 + o1 * 1000 + o2;
}

void hash_to_odds(uint32_t hash, double (*odds)[3])
{
    (*odds)[0] = (double)((uint32_t)(hash / 1000000)) / 100.0;
    (*odds)[1] = (double)(((uint32_t)(hash / 1000)) - (((uint32_t)(hash / 1000000)) * 1000)) / 100.0;
    (*odds)[2] = (double)(((uint32_t)(hash)) - (((uint32_t)(hash / 1000)) * 1000)) / 100.0;
}
