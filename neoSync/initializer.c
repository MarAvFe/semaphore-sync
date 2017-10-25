// Pide los recursos y los inicializa de manera que los actores encuentren todo listo en el momento de empezar a funcionar.
// Pide la cantidad de páginas o espacios de memoria que va a haber.
// Solicita la memoria compartida al sistema operativo.
// Este proceso debe morir después de realizar la inicialización.

// http://www.chuidiang.org/clinux/ipcs/mem_comp.php

#include "globals.h"


void main(int argc, char *argv[]){
	
	
	
	ptrLogFile = "actions.log";
	int memory_size; // tamaño de bytes que deseamos para la memoria.
	int memory_id, sem_id; // identificador de la zona de memoria.
	//int *memory, *threadPtrs; // puntero a la zona de memoria.
	struct sharedMemory * memory;


		
    printf("╔═════════════════════════════════════╗\n");
    printf("║.............INITIALIZER.............║\n");
    printf("╚═════════════════════════════════════╝\n");

	// pedir al usuario tamaño de memoria
	/*printf(">> Enter memory size: ");
	scanf("%d",&memory_size);*/
	memory_size = sizeof(struct sharedMemory);

	// Función int shmget (key_t, int, int).
	// Creamos la memoria y nos devuelve un identificador para dicha zona.
	// El tercer parámetro son permisos de lectura/escritura/ejecución para propietario/grupo/otros, al igual que los ficheros.
	// IPC_CREAT indica si se debe crear la memoria en caso de que no exista.

	if ((memory_id = shmget(MEMORY_KEY, memory_size, IPC_CREAT | RWPERM)) < 0) {
		perror("shmget");
		exit(1);
	}

	// Función char * shmat (int, char *, int).
	// obtener un puntero que apunte la zona de memoria, para poder escribir o leer en ella.
	// Declaramos en nuestro código un puntero al tipo que sepamos que va a haber en la zona de memoria
	// El primer parámetro es el identificador de la memoria obtenido en el paso anterior.
	// El puntero devuelto es de tipo char *. Debemos hacerle un "cast" al tipo que queramos.

	if ((memory = shmat(memory_id, NULL, 0)) == NULL) {
		perror("shmat");
		exit(1);
	}
	
	
	
	createFile();
	
	
	// crear semaforos
    if((sem_id=semget(SEM_KEY,
            SEM_CANT,
            IPC_CREAT | 0700))<0) {
        perror(NULL);

    }

    // solo un proceso puede estar en la region critica
    // se puede cambiar el 1, por la cantidad de procesos que pueden estar en RC
    initSem(sem_id,SEM_MEMORY,1);
    initSem(sem_id,SEM_FILE,1);

	
	// iniciar structs en -1 	
	memory = malloc(sizeof(struct sharedMemory));
	memset(memory, -1, sizeof(struct sharedMemory));



	printf("memory_id: %d with key: %i\n",memory_id, MEMORY_KEY);
	printf("sem_id: %d with key: %i\n",sem_id, SEM_KEY);
	
	shmdt(memory);


}
