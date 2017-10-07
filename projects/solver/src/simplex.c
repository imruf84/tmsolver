#include "simplex.h"

void simplex_init()
{
    srand(time(NULL));
}

void init_tableau(tableau_t *t, uint8_t matchesCount, uint8_t chancePerMatch)
{
    t->matchesCount = matchesCount;
    t->chancePerMatch = chancePerMatch;
    t->oddsCount = t->matchesCount * t->chancePerMatch;

    uint8_t n = 1;
    for (uint8_t i = 0; i < t->matchesCount; i++) n *= t->chancePerMatch;

    t->numVariables = n;
    t->numConstraints = n;
    t->numRows = t->numConstraints + 1;
    t->numCols = t->numRows + t->numVariables + 1;
}

void print_tableau(tableau_t *t)
{
    for (int i = 1; i <= t->numRows; i++)
    {
        for (int j = 1; j <= t->numCols; j++)
        {
            printf("%.2f ", t->tableau[i][j]);
        }
        printf("\n");
    }
}

void pivot(tableau_t *t, uint8_t theRow, uint8_t theCol)
{
    double lPivot = t->tableau[theRow][theCol];

    t->starred[theRow] = 0;

    for (uint8_t i = 1; i <= t->numCols; i++)
    {
        t->tableau[theRow][i] = t->tableau[theRow][i] / lPivot;
    } // i

    for (uint8_t i = 1; i <= t->numRows; i++)
    {

        if ( (i != theRow) && (t->tableau[i][theCol] != 0) )
        {

            double factr = t->tableau[i][theCol];
            for (uint8_t j = 1; j <= t->numCols; j++)
            {
                t->tableau[i][j] = t->tableau[i][j] - factr * t->tableau[theRow][j];
            } // j
        }
    } // i
}

void reset_tableau(tableau_t *t, odd_t *o1, odd_t *o2, odd_t *o3, odd_t *o4)
{
    //Szorzók tárolása.
    uint8_t oi = 0;
    t->odds[oi++] = o1->odds[0];t->odds[oi++] = o1->odds[1];t->odds[oi++] = o1->odds[2];
    t->odds[oi++] = o2->odds[0];t->odds[oi++] = o2->odds[1];t->odds[oi++] = o2->odds[2];
    t->odds[oi++] = o3->odds[0];t->odds[oi++] = o3->odds[1];t->odds[oi++] = o3->odds[2];
    t->odds[oi++] = o4->odds[0];t->odds[oi++] = o4->odds[1];t->odds[oi++] = o4->odds[2];

    t->TableauNumber = 1;
    t->phase1 = true;
    t->singular = false;


    // Kinullázzuk az egészet.
    for (uint8_t i = 1; i <= t->numRows; i++)
    {
        for (uint8_t j = 1; j <= t->numCols; j++)
        {
            t->tableau[i][j] = 0.0;
        }
    }
    // Bal felsõ negyed feltöltése -1-el
    for (uint8_t i = 1; i <= t->numConstraints; i++)
    {
        for (uint8_t j = 1; j <= t->numVariables; j++)
        {
            t->tableau[i][j] = -1.0;
        }
    }
    // Bal alsó sor feltöltése 1-esekkel.
    for (uint8_t j = 1; j <= t->numVariables; j++)
    {
        t->tableau[t->numRows][j] = 1.0;
    }
    // Fõátló feltöltése -1-el
    for (uint8_t i = 1; i <= t->numConstraints; i++)
    {
        for (uint8_t j = 1; j <= t->numVariables; j++)
        {
            if (i == j) t->tableau[i][j+t->numVariables] = -1.0;
        }
    }

    // Feltételek jobb oldalainak feltöltése.
    for (uint8_t i = 1; i <= t->numConstraints; i++)
    {
        t->tableau[i][t->numCols] = 1.0;
    }

    t->tableau[t->numRows][t->numCols-1] = 1;

    uint8_t ijk[t->matchesCount];
    for (uint8_t i = 0; i < t->matchesCount; i++) ijk[i] = 0;

    for (uint8_t k = 0; k < t->numConstraints; k++)
    {
        // Szorzók szorzata.
        double p = 1;
        for (uint8_t i = 0; i < t->matchesCount; i++)
        {
            p *= t->odds[i * t->chancePerMatch + ijk[i]];
        }
        t->tableau[k + 1][k + 1] = p - 1.0;

        // Léptetés.
        for (uint8_t i = t->matchesCount - 1; i >= 0; i--)
        {
            if (++ijk[i] >= t->chancePerMatch) ijk[i] = 0;
            else break;
        }
    }

    for (uint8_t i = 1; i <= t->numVariables; i++) t->starred[i] = 1;
}

void simplex_solve(tableau_t *t)
{
    bool negIndicator = false;
    double testRatio[t->numRows];
    uint8_t theRow = 0;
    uint8_t i,j;
    uint8_t theRowx;
    uint8_t theColx = 0;
    double numx;
    double rowmax;

    while ((t->phase1) && (t->TableauNumber <= MAX_STEPS))
    {

        bool checkingForZeros = true;
        bool foundAZero = false;

        while(checkingForZeros)
        {
            checkingForZeros = false;
            for (i = 1; i <= t->numRows-1; i++)
            {
                if (t->starred[i] == 1)  break;
            } // i
            theRowx = i;

            if ((t->tableau[theRowx][t->numCols] == 0) && (t->starred[theRowx] == 1))
            {
                checkingForZeros  = true;
                foundAZero = true;
                for (j = 1; j <= t->numCols-1; j++)
                {
                    t->tableau[theRowx][j] *= -1;
                } // j
                t->starred[theRowx] = 0;
                t->TableauNumber +=1;
            }
        }

        t->phase1 = false;
        for (i = 1; i <= t->numConstraints; i++)
        {
            if (t->starred[i] == 1)
            {
                t->phase1 = true;
                break;
            }
        } // i

        if (t->phase1)
        {
            if(!foundAZero)
            {
                rowmax = 0;

                for (i = 1; i <= t->numRows-1; i++)
                {
                    if (t->starred[i] == 1) break;
                } // i

                theRowx = i;

                for (j = 1; j <= t->numCols-2; j++)
                {

                    numx = t->tableau[i][j];
                    if (numx > rowmax)
                    {
                        rowmax = numx;
                        theColx = j;
                    }
                } // j

                if (rowmax == 0)
                {
                    t->singular = true;
                    simplex_check_solution(t);
                    return;
                }
                else
                {
                    for (i = 1; i <= t->numRows-1; i++)
                    {
                        testRatio[i] = -1;
                        if (t->tableau[i][theColx] >0)
                        {
                            if (dAbs(t->tableau[i][t->numCols]) < EPSILON) t->tableau[i][t->numCols] = 0;
                            testRatio[i] = t->tableau[i][t->numCols] / t->tableau[i][theColx];
                        }

                    } // i

                    double minRatio = 1000000000;
                    theRow = 0;
                    for (i = 1; i <= t->numRows - 1; i++)
                    {
                        if ((testRatio[i] >= 0) && (testRatio[i] < minRatio))
                        {
                            minRatio = testRatio[i];
                            theRow = i;
                        }
                        else if ((testRatio[i] >= 0) && (testRatio[i] == minRatio))
                        {
                            if (t->starred[i] == 1) theRow = i;
                            else if (MY_RANDOM>.5) theRow = i;
                        }
                    } // i

                    if (theRow == 0)
                    {
                        t->singular = true;
                        simplex_check_solution(t);
                        return;
                    }

                    pivot(t, theRow,theColx);
                }

                t->TableauNumber += 1;
            }
        }
    }

    double testnum = 0;

    for (i = 1; i <= t->numCols-1; i++)
    {
        testnum = t->tableau[t->numRows][i];

        if (testnum<0)
        {
            negIndicator = true;
        }
    } // i

    uint8_t theCol = 0;
    if (negIndicator)
    {
        double minval = 0;
        for (i = 1; i <= t->numCols-1; i++)
        {
            testnum = t->tableau[t->numRows][i];
            if (testnum < minval)
            {
                minval = testnum;
                theCol = i;
            }
        } // i
    }



    while  ((negIndicator) && (t->TableauNumber <= MAX_STEPS)) // phase 2
    {
        for (i = 1; i <= t->numRows-1; i++)
        {
            testRatio[i] = -1;
            if (t->tableau[i][theCol] > 0)
            {
                if (dAbs(t->tableau[i][t->numCols]) < EPSILON) t->tableau[i][t->numCols] = 0;
                testRatio[i] = t->tableau[i][t->numCols] / t->tableau[i][theCol];
            }
        } // i

        double minRatio = 1000000000;
        theRow = 0;
        for (i = 1; i <= t->numRows-1; i++)
        {
            if ((testRatio[i] >= 0) && (testRatio[i] < minRatio))
            {
                minRatio = testRatio[i];
                theRow = i;
            }
            else if ((testRatio[i] >= 0) && (testRatio[i] == minRatio))
            {
                if (MY_RANDOM>.5) theRow = i;
            }
        } // i

        if (theRow == 0)
        {
            t->singular = true;
            simplex_check_solution(t);
            return;
        }

        pivot(t, theRow,theCol);
        t->TableauNumber += 1;

        negIndicator = false;
        for (i = 1; i <= t->numCols-1; i++)
        {
            if (t->tableau[t->numRows][i] <0)
            {
                negIndicator = true;
            }
        } // i

        if (negIndicator)
        {
            double minval = 0;
            for (i = 1; i <= t->numCols-1; i++)
            {
                testnum = t->tableau[t->numRows][i];
                if (testnum < minval)
                {
                    minval = testnum;
                    theCol = i;
                }
            } // i
        }
    }

    simplex_check_solution(t);

} // simplexMethod

void simplex_check_solution(tableau_t *t)
{
    if  (t->TableauNumber > MAX_STEPS)
    {
        //printf("No optimal solution found after 50 steps. Aborted.");
        t->solution = NONE;
        return;
    }

    if (t->singular)
    {
        //printf("No optimal solution exists for this problem.");
        t->solution = NONE;
        return;
    }


    // Triviális megoldás tesztelése.
    bool trivial = true;
    for (uint8_t j = 1; j <= t->numVariables; j++)
    {
        if (0.0 != t->tableau[j][t->numCols])
        {
            trivial = false;
            break;
        }
    }

    if (trivial)
    {
        //printf("Trivial solution found.");
        t->solution = TRIVIAL;
        return;
    }

    //printf("Optimal Solution:\n");

    double objectiveVal = t->tableau[t->numRows][t->numCols];
    objectiveVal = - objectiveVal;

    //printf("%f\n", objectiveVal);

    for (uint8_t j = 1; j <= t->numVariables; j++)
    {
        //printf("%f\n", t->tableau[j][t->numCols]);
    } // j

    t->solution = OPTIMAL;
    //return OPTIMAL;
}

void simplex_print_solution(tableau_t *t, bool full)
{
    uint8_t ijk[t->matchesCount];
    for (uint8_t i = 0; i < t->matchesCount; i++) ijk[i] = 0;

    printf("Szorzok:\n");
    for (uint8_t i = 0; i < t->oddsCount; i++)
    {
        printf("%.2f ", t->odds[i]);
        if ((i % t->chancePerMatch) == (t->chancePerMatch - 1))
        {
            printf("\n");
        }
    }

    if (false == full)
    {
        return;
    }

    printf("Tetek:\n");
    double d = 0;
    for (uint8_t k = 0; k < t->numConstraints; k++)
    {
        // Szorzók szorzata.
        double p = 1;
        for (uint8_t i = 0; i < t->matchesCount; i++)
        {
            p *= t->odds[i * t->chancePerMatch + ijk[i]];
            printf("%.2f*", t->odds[i * t->chancePerMatch + ijk[i]]);
        }
        d = p*t->tableau[k+1][t->numCols];
        printf("%f=%f\n", t->tableau[k+1][t->numCols], d);

        // Léptetés.
        for (uint8_t i = t->matchesCount - 1; i >= 0; i--)
        {
            if (++ijk[i] >= t->chancePerMatch) ijk[i] = 0;
            else break;
        }
    }

    printf("Kiadas:\n");
    for (uint8_t j = 1; j <= t->numVariables; j++)
    {
        printf("%f", t->tableau[j][t->numCols]);
        if (j < t->numVariables)
        {
            printf("+");
        }
    }
    printf("=%f\n", (-t->tableau[t->numRows][t->numCols]));
    printf("Nyereseg:\n%f-%f=%f\n", d, (-t->tableau[t->numRows][t->numCols]), (d+t->tableau[t->numRows][t->numCols]));
}
