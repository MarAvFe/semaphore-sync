#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <sys/shm.h>
/*
#include <sys/stat.h>
#include <wait.h>
#include <unistd.h>*/

#define MEMORY_KEY ftok("actions.log",1337)
#define MEMSIZE 128
#define RWPERM 0666
#define UNITSIZE sizeof(int)

FILE *logFile, *memFile;
char * ptrLogFile, * ptrMemFile;

int randint(int,int);
void createFile();
void writeIntoFile(char*);
void writeIntIntoFile(int);
void readFromFile();

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
