
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#define NUM_THREADS 1001

#define CCR_DECLARE(label) \
  pthread_mutex_t region_##label##_mtx; \
  \
  pthread_cond_t region_##label##_queue_fail; \
  pthread_cond_t region_##label##_queue_try; \
  pthread_cond_t region_##label##_queue_entry; \
  \
  int region_##label##_is_trying; \
  int region_##label##_num_failed; \
  int region_##label##_num_trying; \

#define CCR_INIT(label) \
  pthread_mutex_init(&region_##label##_mtx, NULL); \
  \
  pthread_cond_init(&region_##label##_queue_fail, NULL); \
  pthread_cond_init(&region_##label##_queue_try, NULL); \
  pthread_cond_init(&region_##label##_queue_entry, NULL); \
  \
  region_##label##_is_trying = 0; \
  region_##label##_num_failed = 0; \
  region_##label##_num_trying = 0; \

// NOTE: Consult accompanying pseudocode
#define CCR_EXEC(label, cond, body) \
  \
  pthread_mutex_lock(&region_##label##_mtx); \
  if (region_##label##_is_trying) pthread_cond_wait(&region_##label##_queue_entry, &region_##label##_mtx); \
  \
  while(!(cond)) { \
    \
    region_##label##_num_failed++; \
    \
    if (region_##label##_num_trying > 0) { \
      region_##label##_num_trying--; \
      pthread_cond_signal(&region_##label##_queue_try); \
    } \
    else { \
      region_##label##_is_trying = 0; \
      pthread_cond_signal(&region_##label##_queue_entry); \
    } \
    \
    pthread_cond_wait(&region_##label##_queue_fail, &region_##label##_mtx); \
    \
    region_##label##_is_trying = 1; \
    region_##label##_num_trying++; \
    \
    if(region_##label##_num_failed > 0) { \
      region_##label##_num_failed--; \
      pthread_cond_signal(&region_##label##_queue_fail); \
    } \
    else { \
      region_##label##_num_trying--; \
      pthread_cond_signal(&region_##label##_queue_try); \
    } \
    if(region_##label##_num_trying > 0) pthread_cond_wait(&region_##label##_queue_try, &region_##label##_mtx); \
    \
  } \
  \
  body; \
  \
  if(region_##label##_num_failed > 0) { \
    region_##label##_num_failed--; \
    region_##label##_is_trying = 1; \
    pthread_cond_signal(&region_##label##_queue_fail); \
  } \
  else if (region_##label##_num_trying > 0) { \
    region_##label##_num_trying--;\
    pthread_cond_signal(&region_##label##_queue_try); \
  } \
  else { \
    region_##label##_is_trying = 0; \
    pthread_cond_signal(&region_##label##_queue_entry); \
  } \
  pthread_mutex_unlock(&region_##label##_mtx); \


int cs = 1;
CCR_DECLARE(makis);


// foo: A simple demo function
void* foo(void* arg) {
  int id = *((int *) arg);
  free(arg);

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
