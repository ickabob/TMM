#ifndef __lnrsys_h__
#define __lnrsys_h__

#include <pthread.h>

#ifndef __NCORES__
#define __NCORES__ 1
#endif
/* linear system:
 * t = Ax + b
 */
typedef struct{
  double *a_p;   //A
  double *b_p;   //B 
  double *t_p;   //t
  double *t1_p;  //x
  int dimension; //dimension of the rowspace of t.
}linearSystem_t;

/*Structured meta information.
 *  A packaged reference to a linear system,
 *  and some reflective information for dispatched threads.
 */
typedef struct {
  linearSystem_t *system_p;
  int thread_max;
  int thread_number;
  double *return_storage_p;
}threadLocalState_t;

double step_linear_system(pthread_t *threads, linearSystem_t *system );
#endif
