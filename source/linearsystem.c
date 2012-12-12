#include <math.h>
#include "dbg.h"
#include "linearsystem.h"

int
step_linear_system(pthread_t *threads, const int NThreads, linearSystem_t *system){
  int i;   //iterator
  int rv;  //return check for thread creation.
  //  double errors[NThreads];
  //validate parameters naively.
  if ((!threads) || (system == NULL)) return -1;
  //dispatch thread to do its work.  
  for (i=0; i < NThreads; i++)
    {
      threadLocalState_t ts = { .system = system, \
                                .thread_max = NThreads, \
                                .thread_number = i};
      
      rv = pthread_create( (pthread_t *)threads[i],  //thread
			   NULL,                     //params
			   thread_transformation,     //function to execute
			   (void *)&ts);              //function arguments.
      if (rv < 0){
        log_err("error on thread creation %d.\n",i);
        return -1;
      }
    }
  //collect threads and compute max error.
  for (i=0; i < NThreads; i++)
    {
      rv = pthread_join(threads[i], NULL);
      if (rv != 0){
	log_err("error on thread join %d\n", i);
	return -1;
      }
    }
  return 0;
}

void *
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
  
  if (threadstate == NULL){
    log_err("Invalid arguments: %p\n",(void*)L);
    return NULL;
  }

  //breaking the work into p chunks.
  /* 1/pth amount of work, in a multiplication is 
   *defined to (rows/p) dot products, plus
   *translational component from b.
   */
  a = L->system->a;
  b = L->system->b;
  t = L->system->t;
  t1 = L->system->t1;
  cols = (L->system->dimension);
  rows = cols / (L->thread_max);
  entry_row = L->thread_number * rows;
#ifdef DEBUG
  debug("thread[%d]: range of rows [%d - %d]\n",L->thread_number, entry_row, entry_row + rows);
#endif
  error = errori = 0;
  pthread_exit(NULL);
  for(r = entry_row; r < entry_row + rows; ++r)
    {
      //inner product loop,
      sum = 0.0;
      for (c = 0; c < cols; ++c)
	sum += *(a+rows*r+c) * t[c];
    }
  t1[r] = sum + b[r];
  errori = fabs(t1[r] - t[r]);
  if(errori > error)
    error = errori;
  //done doing work.
  //callee will have to determine the max error returned by the n threads.
  pthread_exit(NULL);
}
