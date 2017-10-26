#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include "globals.h"

#define PAGINATION 0
#define SEGMENTATION 1
#define PAGESIZE 2
#define NUMPAGES MEMSIZE/sizeof(struct segPage)

#define handle_error_en(en, msg) \
do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
do { perror(msg); exit(EXIT_FAILURE); } while (0)

void dumpMemory();
int memory_id, memory_size;
struct sharedMemory * memory;

int main(int argc, char * argv[]){
	memory_size = sizeof(struct sharedMemory);

  if ((memory_id = shmget(MEMORY_KEY, memory_size, RWPERM)) < 0) {
		perror("shmget");
		exit(1);
	}

  if ((memory = shmat(memory_id, NULL, 0)) == NULL) {
		perror("shmat");
		exit(1);
  }

  printf("Last searched memory tid: %i\n", tidGettingMemory);

  dumpMemory();

  shmdt(memory);
  exit(EXIT_SUCCESS);
}

void dumpMemory(){
  int i, k = 0;
  printf("MEM-DUMP (%p):\n", memory);
  printf("  MEM\n");
  for (i = 0; i < MEMSIZE; i++) {
    if(memory[0].fragment[i].processNum != -1) {
      printf("(%i,%i),", memory[0].fragment[i].processNum, memory[0].fragment[i].numLogicPage);
      if (k % 10 == 9) printf("\n");
      k++;
    }
  }
  printf("\n%i Pages written.", k);
  printf("\n  THREADS: PENDING:");
  /*for (int i = 0; i < MEMSIZE; i++) {
    printf("%i, ", memory[0].threads[i].thread_num);
  }*/
  printf("\n  SEMID: %i\n", memory[0].semaphore);

  printf("\n");
}
