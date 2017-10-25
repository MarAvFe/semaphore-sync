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
#include <wait.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <signal.h>



#define MEMORY_KEY ftok("actions.log",1337)
#define THREADS_KEY MEMORY_KEY+1 //ftok("tmp.log",1337)
#define SEM_KEY THREADS_KEY+1
#define SEM_CANT 2
#define SEM_MEMORY 0
#define SEM_FILE 1
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

FILE *logFile, *memFile;
char * ptrLogFile, * ptrMemFile;
int schema, tidGettingMemory;

int randint(int,int);
void createFile();
void writeIntoFile(char*);
void writeIntIntoFile(int);
void readFromFile();


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


// funciones auxiliares

int randint(int l, int h){
  return l + rand() % (h - l);
}

void createFile(){
  printf("Creating logFile %s...\n", ptrLogFile);
  unsigned char buffer[0] = "";
  logFile = fopen(ptrLogFile, "w");

  if (logFile != NULL){
    fprintf(logFile, "%s", buffer);
  }
  fclose(logFile);
}

void writeIntoFile(char * buf){
  //printf("Writing \"%s\" into logFile %s...\n", buf, ptrLogFile);

  // critical section here
  logFile = fopen(ptrLogFile, "a");

  if (logFile != NULL){
    fprintf(logFile, "%s", buf);
  }
  fclose(logFile);
}

void writeIntIntoFile(int buf){
  // critical section here
  //printf("Writing \"%i\" into logFile %s...\n", buf, ptrLogFile);
  logFile = fopen(ptrLogFile, "a");

  if (logFile != NULL){
    fprintf(logFile, "%i", buf);
  }
  fclose(logFile);
}

void readFromFile(){
  //printf("Reading from logFile %s...\n", ptrLogFile);
  size_t bytesRead = 0;
  logFile = fopen(ptrLogFile, "r");
  char buffer[255];
  int nums[255];
  int numInd = 0;
  char * tmpNum, *bufPtr = buffer;
  if (logFile != NULL){
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), logFile)) > 0){
      // printf("Read: %s\n", buffer);
      while ((tmpNum = strsep(&bufPtr, " ")) != NULL){ // Separa los numeros leidos por espacio
        nums[numInd] = atoi(tmpNum);
        printf("gotNum: %i\n", nums[numInd]);
        numInd++;
      }
    }
  }
  fclose(logFile);
};


void initSem(int semid, int numSem, int value) { //iniciar un semaforo
    if (semctl(semid, numSem, SETVAL, value) < 0) {
    perror(NULL);
        perror("Sem: init error");
    }
};

void getSource(int sem_id, int sem_num) {
    struct sembuf sops;
    sops.sem_num = sem_num; 
    sops.sem_op = -1;
    sops.sem_flg = 0;
 
    if (semop(sem_id, &sops, 1) == -1) {
        perror(NULL);
    }
};

void releaseSource(int sem_id, int sem_num) {
    struct sembuf sops; 
    sops.sem_num = sem_num;
    sops.sem_op = 1;
    sops.sem_flg = 0;
 
    if (semop(sem_id, &sops, 1) == -1) {
        perror(NULL);
    }
};

