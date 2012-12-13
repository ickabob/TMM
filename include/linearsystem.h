#ifndef __lnrsys_h__
#define __lnrsys_h__

#include <pthread.h>
#include <math.h>

#ifndef __NCORES__
#define __NCORES__ 1
#endif

#define NTHREADS __NCORES__

/* linear system:
 * t = Ax + b
 */

double *a;   //A
double *b;   //B 
double *t;   //t
double *t1;  //x
int dimension; //dimension of the rowspace of t.


//step the linear system count times.
void step_linear_system(int count);

//iteration errors.
double *errors;

#endif
