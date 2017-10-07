#ifndef __EVENT_H__
#define __EVENT_H__

#include <stdint.h>
#include <stdio.h>

typedef struct
{
    char no[10];
    double odds[3];
    uint32_t hash;
} event_t;

void event_print(event_t *e);

#endif // __EVENT_H__
