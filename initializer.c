// Programa Inicializador: Este programa se encarga de crear el ambiente.
// Pide los recursos y los inicializa de manera que los actores encuentren todo listo en el momento de empezar a funcionar.
// Pide la cantidad de páginas o espacios de memoria que va a haber.
// Solicita la memoria compartida al sistema operativo.
// Este proceso debe morir después de realizar la inicialización.

// http://www.chuidiang.org/clinux/ipcs/mem_comp.php

#include "api.h"

void main(int argc, char *argv[]){

	int memory_size, threads_size; // tamaño de bytes que deseamos para la memoria.
	int memory_id, threads_id; // identificador de la zona de memoria.
	int *memory, *threadPtrs; // puntero a la zona de memoria.

    printf("╔═════════════════════════════════════╗\n");
    printf("║.............INITIALIZER.............║\n");
    printf("╚═════════════════════════════════════╝\n");

	// pedir al usuario tamaño de memoria
	/*printf(">> Enter memory size: ");
	scanf("%d",&memory_size);*/
	memory_size = MEMSIZE * UNITSIZE;

	struct thread_info * ptrthi;
	threads_size = LIMITPROCESSES * sizeof(pthread_t);

	// Función int shmget (key_t, int, int).
	// Creamos la memoria y nos devuelve un identificador para dicha zona.
	// Si la zona de memoria correspondiente a MEMORY_KEY ya estuviera creada, simplemente nos daría el identificdor de la memoria.
	// El primer parámetro es la clave MEMORY_KEY obtenida de share.h y debería ser la misma para todos los programas.
	// El segundo parámetro es memory_size.
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
	// Los otros dos bastará rellenarlos con ceros.
	// El puntero devuelto es de tipo char *. Debemos hacerle un "cast" al tipo que queramos.

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

	// Reset memory spaces
	for (int i = 0; i < memory_size; i++) {
		printf("%p: mem[%lu]=%i -> ", &memory[i], i/UNITSIZE, memory[i]);
		memory[i] = -1;
		printf("%i\n", memory[i]);
	}

	printf("memy: %p\n", memory);
	printf("memz: %p\n", threadPtrs);


	printf("memory_id: %d with key: %i\n",memory_id, MEMORY_KEY);

	printf("threadPtrs: %d with key: %i\n",threads_id, THREADS_KEY);
	shmdt(memory);
	shmdt(threadPtrs);
}
