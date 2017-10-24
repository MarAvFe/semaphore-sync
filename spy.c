#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include "api.h"

#define PAGINATION 0
#define SEGMENTATION 1
#define PAGESIZE 2
#define NUMPAGES MEMSIZE/PAGESIZE

#define handle_error_en(en, msg) \
do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
do { perror(msg); exit(EXIT_FAILURE); } while (0)

void dumpMemory();
int * memory, memory_id, memory_size;

int main(int argc, char * argv[]){
	memory_size = MEMSIZE * UNITSIZE;

  if ((memory_id = shmget(MEMORY_KEY, memory_size, RWPERM)) < 0) {
		perror("shmget");
		exit(1);
	}

  if ((memory = shmat(memory_id, NULL, 0)) == NULL) {
		perror("shmat");
		exit(1);
  }

  printf("Last serched memory tid: %i\n", tidGettingMemory);

  dumpMemory();

  shmdt(memory);
  exit(EXIT_SUCCESS);
}

void dumpMemory(){
printf("MEM-DUMP (%p):\n", memory);
  for (int i = 0; i < MEMSIZE; i++) {
    printf("%i ", memory[i]);
    if (i % PAGESIZE == PAGESIZE-1) printf(",");
    if (i % 32 == 31) printf("\n");
  }
  printf("\n");
}
