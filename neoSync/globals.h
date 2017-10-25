#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <errno.h>

#define MEMORY_KEY ftok("actions.log",1337)
#define MEMSIZE 512
#define RWPERM 0666

#define DED   -1 // Couldn't create. Died instead
#define FNSHED 0 // Finished
#define WTMEM1 1 // Wait mem semaphore
#define SCHSPC 2 // Search mem space
#define WRTLG1 3 // Write in log 1
#define GVMEM1 4 // Give back mem semaphore
#define SLEEPS 5 // Sleep/work
#define WTMEM2 6 // Wait mem semaphore
#define RLSSPC 7 // Release mem space
#define WRTLG2 8 // Write in log 2
#define GVMEM2 9 // Give back mem semaphore

struct thread_info {          /* Used as argument to thread_start() */
  pthread_t thread_id;        /* ID returned by pthread_create() */
  int       thread_num;       /* Application-defined thread # */
  int       numPages;
  int       timeWait;
  int       stage;
};

struct segPage {
  int processNum;
  int numLogicPage;
};

struct sharedMemory {
  struct segPage fragment[MEMSIZE];
  struct thread_info threads[MEMSIZE];
  int semaphores[2];
};
