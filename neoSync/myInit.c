#include "globals.h"

void main(int argc, char *argv[]){

	int memory_size;
	int memory_id;
	int *memory;
	struct sharedMemory *pEmptyMemory;

    printf("╔═════════════════════════════════════╗\n");
    printf("║.............INITIALIZER.............║\n");
    printf("╚═════════════════════════════════════╝\n");

	// pedir al usuario tamaño de memoria
	/*printf(">> Enter memory size: ");
	scanf("%d",&memory_size);*/
	memory_size = sizeof(struct sharedMemory);

	if ((memory_id = shmget(MEMORY_KEY, memory_size, IPC_CREAT | RWPERM)) < 0) {
		perror("shmget");
		exit(1);
	}

	if ((memory = shmat(memory_id, NULL, 0)) == (int *) -1) {
		perror("shmat");
		exit(1);
	}

	pEmptyMemory = calloc(1, sizeof(struct sharedMemory));

	pEmptyMemory[0].fragment = calloc(MEMSIZE, sizeof(struct segPage));
	pEmptyMemory[0].threads = calloc(MEMSIZE, sizeof(struct thread_info));
	pEmptyMemory[0].semaphores = calloc(2, sizeof(int));

	// Reset memory spaces
	/*for (int i = 0; i < memory_size; i++) {
		printf("%p: mem[%lu]=%i -> ", &memory[i], i/UNITSIZE, memory[i]);
		memory[i] = -1;
		printf("%i\n", memory[i]);
	}*/

	printf("memy: %p\n", memory);

	printf("memory_id: %d with key: %i\n",memory_id, MEMORY_KEY);
	shmdt(memory);
}
