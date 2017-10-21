/*
Dispara una serie de hilos con tamaños y duraciones aleatorias. Luego imprime
  al finalizar la espera de cada uno.
Compilar:
  user~:$ gcc -o creth createThreads.c -pthread -Wall
Ejecutar:
  user~:$ ./creth
*/
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

#define handle_error_en(en, msg) \
do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
do { perror(msg); exit(EXIT_FAILURE); } while (0)

int randint(int,int);

struct thread_info {    /* Used as argument to thread_start() */
  pthread_t thread_id;        /* ID returned by pthread_create() */
  int       thread_num;       /* Application-defined thread # */
  int       numPages;
  int       timeWait;
};

/* Thread start function: display address near top of our stack,
and return upper-cased copy of argv_string */

static void * thread_start(void *arg){
  struct thread_info *tinfo = arg;

  printf("Thread %d: size %i pages, to wait %i seconds.\n", tinfo->thread_num, tinfo->numPages, tinfo->timeWait);
  sleep(tinfo->timeWait);
  printf("thread %d: Finished wait.\n", tinfo->thread_num);
  return 0;
}

int main(int argc, char *argv[]) {
  int s, tnum, num_threads;
  struct thread_info *tinfo;
  pthread_attr_t attr;
  void *res;

  num_threads = randint(3,14);

  /* Initialize thread creation attributes */

  s = pthread_attr_init(&attr);
  if (s != 0)
  handle_error_en(s, "pthread_attr_init");

  /* Allocate memory for pthread_create() arguments */

  tinfo = calloc(num_threads, sizeof(struct thread_info));
  if (tinfo == NULL)
  handle_error("calloc");

  /* Create one thread for each command-line argument */

  for (tnum = 0; tnum < num_threads; tnum++) {
    tinfo[tnum].thread_num = tnum + 1;
    tinfo[tnum].timeWait = randint(2,6);
    tinfo[tnum].numPages = randint(5,10);

    /* The pthread_create() call stores the thread ID into
    corresponding element of tinfo[] */

    s = pthread_create(&tinfo[tnum].thread_id, &attr, &thread_start, &tinfo[tnum]);
    if (s != 0)
    handle_error_en(s, "pthread_create");
  }

  /* Destroy the thread attributes object, since it is no
  longer needed */

  s = pthread_attr_destroy(&attr);
  if (s != 0)
  handle_error_en(s, "pthread_attr_destroy");

  /* Now join with each thread, and display its returned value */
  sleep(2);
  for (tnum = 0; tnum < num_threads; tnum++) {
    s = pthread_join(tinfo[tnum].thread_id, &res);
    if (s != 0)
    handle_error_en(s, "pthread_join");


    // printf("Joined with thread %d; returned value was %s\n", tinfo[tnum].thread_num, (char*)res);
    free(res);      /* Free memory allocated by thread */
  }

  free(tinfo);
  exit(EXIT_SUCCESS);
}

int randint(int l, int h){
  return l + rand() % (h - l);
}
