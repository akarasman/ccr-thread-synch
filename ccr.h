
/* A set of macros to use in thread synchronization implementing a Conditional
 * critical region using pthread library mutexes.
 *
 * Apostolos Karasmanoglou akarasman@uth.gr
 * Danae Dimitriadi dandimit@uth.gr
 */

#ifndef __CCR_H__
#define __CCR_H__

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
  if (region_##label##_is_trying) \
    pthread_cond_wait(&region_##label##_queue_entry, &region_##label##_mtx); \
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
    if(region_##label##_num_trying > 0) \
	  pthread_cond_wait(&region_##label##_queue_try, &region_##label##_mtx); \
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

#endif
