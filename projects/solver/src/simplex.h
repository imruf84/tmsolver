#ifndef __SIMPLEX_H__
#define __SIMPLEX_H__

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "odd.h"

#define EPSILON .00000000000001
#define MAX_STEPS 150
#define MAX_ROWS 85
#define MAX_COLS 170
#define MAX_ODDS 15

#define dAbs(a) (a < 0 ? -a : a)

#define MY_RANDOM ((double)rand() / (RAND_MAX))

typedef enum {NONE, TRIVIAL, OPTIMAL} solution_e;

typedef struct
{
    bool singular;
    uint8_t numRows;
    uint8_t numCols;
    uint8_t numConstraints;
    bool phase1;
    uint8_t numVariables;
    double tableau[MAX_ROWS][MAX_COLS];
    double starred[MAX_ROWS];
    double odds[MAX_ODDS];
    uint8_t TableauNumber;
    uint8_t chancePerMatch;
    uint8_t oddsCount;
    uint8_t matchesCount;
    solution_e solution;
} tableau_t;


void simplex_init();
void simplex_solve(tableau_t *t);
void simplex_check_solution(tableau_t *t);
void simplex_print_solution(tableau_t *t, bool full);

void init_tableau(tableau_t *t, uint8_t matchesCount, uint8_t chancePerMatch);
void reset_tableau(tableau_t *t, odd_t *o1, odd_t *o2, odd_t *o3, odd_t *o4);
void print_tableau(tableau_t *t);

#endif // __SIMPLEX_H__
