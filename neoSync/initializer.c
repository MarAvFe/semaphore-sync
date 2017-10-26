#include "globals.h"


int main(int argc, char *argv[]){
	ptrLogFile = "actions.log";
	int memory_size; // tamaño de bytes que deseamos para la memoria.
	int memory_id; // identificador de la zona de memoria.
	struct sharedMemory *memory; // puntero a la zona de memoria.
	int sid;

	printf("╔═════════════════════════════════════╗\n");
	printf("║             INITIALIZER             ║\n");
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

	if ((memory = shmat(memory_id, NULL, 0)) == (struct sharedMemory *) -1) {
		perror("shmat");
		exit(1);
	}

	createFile();

	if((sid=semget(SEM_KEY, SEM_CANT, IPC_CREAT | RWPERM))<0) {
		perror("semget");
	}
	printf("sid: %i\n", sid);

	if (semctl(sid, SEM_MEMORY, SETVAL, 1) < 0) {
		perror("semctl");
	}

	if (semctl(sid, SEM_FILE, SETVAL, 1) < 0) {
		perror("semctl");
	}

	memset(memory, -1, sizeof(struct sharedMemory));
	memory[0].semaphore = sid;

	printf("Memy addr: %p\n", &memory);

	//memory->semaphores[0] = sem_id;

	printf("memory_id: %d with key: %i\n",memory_id, MEMORY_KEY);
	printf("sem_id: %d with key: %i\n",sid, SEM_KEY);

	shmdt(memory);

	exit(EXIT_SUCCESS);
}
