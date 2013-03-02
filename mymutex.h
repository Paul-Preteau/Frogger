#ifndef MYMUTEX_H
#define MYMUTEX_H


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//pthread_mutexattr_t attributes;
//pthread_mutex_t mutex;
//pthread_mutex_t screen_mutex;



pthread_mutex_t inicialize_mutex_var(){
  pthread_mutexattr_t attributes;
  pthread_mutex_t mutex;
  pthread_mutexattr_init(&attributes);
  pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&mutex, &attributes);
  return mutex;
}

void destroy_mutex_var(pthread_mutex_t toDestroy){
  pthread_mutex_destroy(&toDestroy);
}

#define checkResults(string, val) {             \
 if (val) {                                     \
   printf("Failed with %d at %s", val, string); \
   exit(1);                                     \
 }                                              \
}

void mutex_lock(pthread_mutex_t* mutex){
    int rc = 0;
    rc = pthread_mutex_lock(mutex);
    checkResults("pthread_mutex_lock()\n", rc);
}

void mutex_unlock(pthread_mutex_t *mutex){
    int rc = 0;
    rc = pthread_mutex_unlock(mutex);
    checkResults("pthread_mutex_unlock()\n", rc);
}

#endif /* MYMUTEX_H */


