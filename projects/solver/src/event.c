#include "event.h"


void event_print(event_t *e)
{
    printf("%s %.2lf %.2lf %.2lf %u\n", e->no, e->odds[0], e->odds[1], e->odds[2], e->hash);
}

