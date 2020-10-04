
/* Simple use example of the CCR macros
 * Apostolos Karasmanoglou akarasman@uth.gr
 * Danae Dimitriadi dandimit@uth.gr
 */

#include "ccr.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#define NUM_THREADS 1001

int cs = 1;
CCR_DECLARE(makis);

// foo: A simple demo function
void* foo(void* arg) {
  int id = *((int *) arg);
  free(arg);

 /* Try replacing this line with :
  * if(cs == id) printf("%d == %d\n", id, cs++)
  * and see what happens...
  */
  CCR_EXEC(makis, cs == id, printf("%d == %d\n", id, cs++))
  return NULL;
}

int main(int argc, char *argv[]) {
  int i;
  int *thread_args[NUM_THREADS];
  pthread_t thread_num;
  CCR_INIT(makis);

  // Init thread arguments
  for(i = 0; i < NUM_THREADS; i++) {
    thread_args[i] = malloc(sizeof(int *));
    *thread_args[i] = i;
  }

  // Create threads
  for(i = 1; i < NUM_THREADS; i++)
    pthread_create(&thread_num, NULL, foo, (int *)thread_args[i]);

  getchar();


  return(1);
}
