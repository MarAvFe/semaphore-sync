#include "globals.h"

int main(){


	int memory_id;
	int sem_id;
	struct sharedMemory * memory;


	if ((memory_id = shmget(MEMORY_KEY, MEMSIZE, RWPERM)) < 0) {
	perror("shmget");
	exit(1);
	}

	if ((memory = shmat(memory_id, NULL, 0)) == NULL) {
	perror("shmat");
	exit(1);
	}

	sem_id = memory[0].semaphore;

	printf("memory_id: %d\nsem_id: %d\n",
    memory_id, sem_id);

	// matar threads

	int thr;
	pthread_t thread_id;

	// probar
	for( thr = 0; thr < MEMSIZE; thr++ ){
		if(memory[0].threads[thr].thread_id != 0){

			thread_id = memory[0].threads[thr].thread_id;
			printf("thread_id: %lu\n cont: %d", (unsigned long) thread_id, thr);

			if(thread_id != 0){

				pthread_kill(thread_id,SIGKILL);
				printf("Thread killed id: %lu\n", (unsigned long) thread_id);
			}
		}
	}



	// liberar semaforos
    if ((semctl(sem_id, 0, IPC_RMID)) == -1) {
        perror(NULL);
    }

	// liberar memoria
    shmctl(memory_id,IPC_RMID,NULL);



	// cerrar archivo
    FILE *pf;
    pf = fopen("actions.log","r");
    fclose(pf);

	return 0;

}
