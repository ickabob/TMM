#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "dbg.h"
#include "linearsystem.h"

//file local functions.
static void *thread_transformation(void *);
static double *max_err(double *, const int);

//defined elsewhere.
#define NT __NCORES__  //Maximum architecture supported 'real' threads.
// ie: core count.
//local to file.
static double Thread_errs[NT];            //each thread will have its computed error.
static threadLocalState_t Ts[NT];        //meta state information for each thread.

double
step_linear_system(pthread_t *threads_p, linearSystem_t *system_p){
  /*Purpose: one iterative step of an affine linear transformation
   *represented by the the function argument: system_p.
   *ie: iterates a linear transformation followed by a translation.
   *
   *returns:
   */
  extern double Thread_errs[];
  extern threadLocalState_t Ts[];

  //locals
  int i;                           //iterator
  int rv;                          //return check for thread creation.
  double *ttmp_p;                  //swap pointer.

  //grossly validate parameters.
  if (!threads_p) return -1.0;
#ifdef DEBUG
  debug("Entered: %s", "step_linear_system");
#endif
  //thread dispatch loop
  for (i=0; i < NT; i++)
    {
      //initialize thread introspection info.
      Ts[i].system_p = system_p;         //the linear system on which to operate.
      Ts[i].thread_max = NT;   //thread count over which work is shared.
      Ts[i].thread_number = i;       //local thread identifier, range [0,thread_max].
      Ts[i].return_storage_p = &Thread_errs[i];  //address where to place return value;

#ifdef DEBUG
      debug("Dispatching thread %d.\n\tParameters: {%p,%d,%d}\n",i,(void*)Ts[i].system_p,Ts[i].thread_max,Ts[i].thread_number);
      fflush(stderr);
#endif
      //send thread out to work @function.
      rv = pthread_create( &threads_p[i],            //thread
                           NULL,                     //params
                           thread_transformation,    //function to execute
                           (void *)&Ts[i]);          //function arguments.
      //creation check
      if (rv < 0){
        log_err("error on thread creation %d.\n",i);
        exit(EXIT_FAILURE);
      }
    }
  //collect threads and compute max error.
  for (i=0; i < NT; i++)
    {
      rv = pthread_join(threads_p[i], NULL);
      if (rv != 0){
        log_err("error on thread join %d\n", i);
        exit(EXIT_FAILURE);
      }
#ifdef DEBUG
      debug("Collected thread %d.", i);
#endif
    }
  //swap t1 & t
#ifdef DEBUG
  debug("Swapping t1:%p with t:%p.", (void*)system_p->t1_p, (void*)system_p->t_p);
#endif
  ttmp_p = system_p->t1_p;
  system_p->t1_p = system_p->t_p;
  system_p->t_p = ttmp_p;

  //iteration error is maximum threads' errors.
  //use ttmp_p to reference the max error in Thread_errs.
  ttmp_p = max_err((double *)&Thread_errs, NT);
#ifdef DEBUG
  debug("Returning from step_linear_system:\t %14.6e", *ttmp_p);
#endif
  return *ttmp_p;
}

static void *
thread_transformation(void * threadstate){
  int r,c;     //indecies into matrix.
  int rows,cols; //bounds of matrix product to compute.
  int entry_row; //first row vector.
  double sum;  //Inner product of A * t;
  double error;//max | t1[i] - t[i] |
  double errori;// | t1[i] - t[i] |
  double *a, *b, *t, *t1;

  //linearSystem *S, int thread_max, int thread_no}
  threadLocalState_t *L = (threadLocalState_t *) threadstate;

  if (L == NULL){
    log_err("Invalid arguments: %p\n",(void*)L);
    return NULL;
  }
#ifdef DEBUG
  debug("T[%d] entered thread_transformation.",L->thread_number);
#endif
  //breaking the work into p chunks.
  /* 1/pth amount of work, in a multiplication is
   *defined to (rows/p) dot products, plus
   *translational component from b.
   */

  a = L->system_p->a_p;
  b = L->system_p->b_p;
  t = L->system_p->t_p;
  t1 = L->system_p->t1_p;
  cols = (L->system_p->dimension);
  rows = cols / (L->thread_max);
  entry_row = L->thread_number * rows;
  #ifdef DEBUG
  debug("T[%d]:\n\tParameters:  &a: %p, &b: %p, &t: %p, &t1: %p}", L->thread_number, (void*)a, (void*)b, (void*)t, (void*)t1);
  debug("T[%d]: row operating range [%d - %d]",L->thread_number, entry_row, entry_row + rows);
  #endif
  error = errori = 0;
  for(r = entry_row; r < entry_row + rows; ++r)
    {
      //inner product loop,
      sum = 0.0;
      for (c = 0; c < cols; ++c)
        sum += *(a+rows*r+c) * t[c];
      t1[r] = sum + b[r];
      errori = fabs(t1[r] - t[r]);
      if(errori > error)
        error = errori;
    }
  //store error
  //error is thread local max error,
  //callee will have to determine the max error returned by the n threads.
  *(L->return_storage_p) = error;
#ifdef DEBUG
  debug("T[%d] leaving critical section of thread_transformation.\n",L->thread_number);
#endif
  //done doing work.
  pthread_exit(NULL);
}

  static double *
    max_err(double *Errs, const int bound){
  int i;
  double *tmp,*max;
  tmp = max = Errs;
#ifdef DEBUG
  debug("Entered max_err\n\tParmeters:  {%p, %d}", (void*)Errs, bound);
#endif
  for (i = 0; i < bound; i++){
    //I think this fails, but need it.  FIX LATER.
    tmp = Errs + i;
#ifdef DEBUG
    debug("Comparing %14.6e against %14.6e",*tmp, *max);
#endif
    if( *tmp > *max ){
#ifdef DEBUG
      debug("New max error: %14.6e", *max);
#endif
      max = tmp;
    }
  }
#ifdef DEBUG
  debug("Returning from max_err:\t %14.6e @ address %p", *max, (void*) max);
#endif
  return max;
}
