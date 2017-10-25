#include "api.h"

main(){

	int memory_size, threads_size; // tamaño de bytes que deseamos para la memoria.
	int memory_id, threads_id; // identificador de la zona de memoria.
	int *memory, *threadPtrs; // puntero a la zona de memoria.


	if ((memory_id = shmget(MEMORY_KEY, memory_size, IPC_CREAT | RWPERM)) < 0) {
		perror("shmget");
		exit(1);
	}

	if ((memory = shmat(memory_id, NULL, 0)) == (int *) -1) {
		perror("shmat");
		exit(1);
	}


	if ((threads_id = shmget(THREADS_KEY, threads_size, IPC_CREAT | RWPERM)) < 0) {
		perror("shmget threads");
		exit(1);
	}

	if ((threadPtrs = shmat(threads_id, NULL, 0)) == (int *) -1) {
		perror("shmat threads");
		exit(1);
	}


	// liberar memoria
    shmctl(memory_id,IPC_RMID,NULL);
    shmctl(threads_id,IPC_RMID,NULL);

	// cerrar archivo
    FILE *pf;
    pf = fopen("actions.log","r");
    fclose(pf);
    


}


