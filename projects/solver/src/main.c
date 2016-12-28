#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

extern int omp_get_thread_num();
extern int omp_get_num_threads();

int main(int argc, const char** argv) {

int nthreads, tid;

/* Fork a team of threads giving them their own copies of variables */
#pragma omp parallel private(nthreads, tid)
  {

  /* Obtain thread number */
  tid = omp_get_thread_num();
  printf("Hello World from thread = %d\n", tid);

  /* Only master thread does this */
  if (tid == 0)
    {
    nthreads = omp_get_num_threads();
    printf("Number of threads = %d\n", nthreads);
    }

  }  /* All threads join master thread and disband */


  return EXIT_SUCCESS;

}