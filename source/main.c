// thrded_matmul_heap.c
// CS4540 Fall 2012
// Templated off Kapenga's matmul_heap.c
// Ian Kane
//

// The following itteretion can be used to solve linear systems
//   t_{i+1} = A t_i + b
// If the itteration converges to t, then t == t_{i+1} == t_i
// So t = A t + b
//   or  (I-a) t = b
//   where, I is the n*n idenity matrix
// There are several important applied problems where convergence
// will take place. One such case is when for
// each row of A ( rows 0 <= i < n)
//             sum(j=0 ... n-1) abs(a[i][j])  < 1.0
// Then the itteration will converge, assuming no roundoff or overflow.
// Example
// % ./matmul_heap 4 10 5

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "dbg.h"
#include "linearsystem.h"

// These two function are not ansi C so they do not appear from the
// libstd.h  header if the gcc option -std=c99 option is used.
// I should see if there is a safe way to include them from stdlib.h
// and not place them explicitly here, which is bad style.
void srand48(long int seedval);
double drand48(void);

int
main(int argc, char *argv[]){
  int   n=4;    // problenm size
  int   seed=10;// seed for srand48() / drand48()
  int i,j;      // iterators
  
  int   itt_max=5;// number of itterations to preform
  char  ch;     // for error checking on command line args.

  if( argc == 4 ) {
    if( (sscanf(argv[1],"%d %[^ /t]", &n, &ch) != 1) ||
        (sscanf(argv[2],"%d %[^ /t]", &seed, &ch) != 1) ||
        (sscanf(argv[3],"%d %[^ /t]", &itt_max, &ch) != 1) ) {
      fprintf(stderr," ERROR : useage: %s [ <n> <seed> <itt_max>]\n", argv[0]);
      return(1);
    }
  } else if(argc != 1 ) {
    fprintf(stderr," ERROR : useage: %s [ <n> <seed> <itt_max>]\n", argv[0]);
    return(1);
  }
  if( n<1 ) {
    fprintf(stderr," ERROR : n must be positive\n");
    return(1);
  }
  if( (a=(double *)malloc(sizeof(double)*n*n)) == NULL) {
    fprintf(stderr," ERROR : malloc for a failed\n");
    return(1);
  }
  if( (b=(double *)malloc(sizeof(double)*n)) == NULL) {
    fprintf(stderr," ERROR : malloc for b failed\n");
    return(1);
  }
  if( (t=(double *)malloc(sizeof(double)*n)) == NULL) {
    fprintf(stderr," ERROR : malloc for t failed\n");
    return(1);
  }
  if( (t1=(double *)malloc(sizeof(double)*n)) == NULL) {
    fprintf(stderr," ERROR : malloc for t1 failed\n");
    return(1);
  }
  
  dimension = n; //dimension of linear space.

  // Generate matrix a with | eigenvalues | < 1
  srand48((long int)seed);
  for(i=0; i< n; i++) {
    for(j=0; j< n; j++) {
      *(a+n*i+j) = 1.999 * (drand48() - 0.5) / n;
    }
  }
  // Generate vector b
  for(i=0; i< n; i++) {
    b[i] = 10.0 * drand48();
  }
  
  // Initialize t
  for(i=0; i< n; i++) {
    t[i] = b[i];
  }

  printf("\n  itt  error\n");
  //main work
  step_linear_system(itt_max);
 
  return 0;
}
