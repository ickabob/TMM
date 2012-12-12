#include <stdlib.h>
#include <stdio.h>
#include "dbg.h"
#include "linearsystem.h"

//file local functions.
static void *thread_transformation(void *);

//mutex to protext global error.
pthread_mutex_t error_mutex = PTHREAD_MUTEX_INITIALIZER;
//file global error and iteration error.
double error = 0;
double errori = 0;

void
step_linear_system(int max_iterations)
{
  /*Purpose: one iterative step of an affine linear transformation
   *represented by the the function argument: system_p.
   *ie: iterates a linear transformation followed by a translation.
   *
   *returns:
   */
  //locals
  int iteration;                   //iteration index
  int i;                           //generic index
  int rv;                          //return value
  double *ttmp_p;                  //swap space
  extern double *errors;          //error per iteration, space to be allocated.
  extern double error;             //thread error

  //exit with nonsensical params.
  if (max_iterations <= 0) return;
#ifdef DEBUG
  debug("Entered step_linear_system: iteration = %d",max_iterations);
#endif

  //Create proper amount of threads
  pthread_t threads[NTHREADS];

  //allocate error array.
  errors = (double *)malloc(sizeof(double) * max_iterations);
  if (errors == NULL) {
  perror("failed to allocate memory");
  exit(EXIT_FAILURE);
}
  for (iteration = 0; iteration < max_iterations; iteration++) {
  error = 0.0;
  //thread dispatch loop
  for (i=0; i < NTHREADS; i++){
#ifdef DEBUG
  debug("Dispatching thread %d.",i);
  fflush(stderr);
#endif
  //send thread out to work @function.
  rv = pthread_create(&threads[i], NULL, thread_transformation, (void *)i);
  //check thread was created.
  if (rv < 0) {
  perror("error creating thread.");
  exit(EXIT_FAILURE);
}
}

  //join threads and compute max error.
  for (i=0; i < NTHREADS; i++){
  rv = pthread_join(threads[i], NULL);
  if (rv != 0){
  log_err("error on thread join %d\n", i);
  exit(EXIT_FAILURE);
}
#ifdef DEBUG
  debug("Joined thread %d.", i);
#endif
}
  //swap t1 & t
#ifdef DEBUG
  debug("Swapping t1:%p with t:%p.", (void*)t1, (void*)t);
#endif
  ttmp_p = t;
  t1 = t;
  t = ttmp_p;

  //Store iterations error
  errors[iteration] = error;
#ifdef DEBUG
  debug("Returning from step_linear_system:\t %14.6e", error);
#endif
}
  return;
}

static void*
   thread_transformation(void *args) {
  int r, c;     //indecies into matrix.
  int t_no;      //thread number.
  double sum;  //Inner product of A * t;
  extern double *a;

  t_no = (int)args;
#ifdef DEBUG
  debug("T[%d] entered thread_transformation.", t_no);
#endif

#ifdef DEBUG
  debug("T[%d]:\n\tParameters:  &a: %p, &b: %p, &t: %p, &t1: %p}", t_no, (void*)a, (void*)b, (void*)t, (void*)t1);
#endif
  //do work
  for (r = t_no; r < dimension; r = r + t_no) {

  sum = 0.0;
  //inner product loop,
  for (c = 0; c < dimension; c++)
    sum += *(a+r*dimension+c) * t[c];

  //add dot product to translation vector b.
  t1[r] = sum + b[r];

  pthread_mutex_lock(&error_mutex);
  errori = fabs(t1[r] - t[r]);
  if(errori > error)
    error = errori;
  pthread_mutex_unlock(&error_mutex);
}
#ifdef DEBUG
  debug("T[%d] leaving thread_transformation.\n",t_no);
#endif
  //done doing work.
  pthread_exit(NULL);
}
