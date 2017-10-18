# semaphore-sync
El siguiente documento explicará el funcionamiento de los programas y el método de ejecución.

Inicializador
---
Se encarga de inicializar un espacio o más espacios en memoria compartida del sistema operativo y guardar la referencia a la misma en un archivo.

Productor
---
Se encarga de producir una cantidad aleatoria de programas (hilos) con un tamaño aleatorio en un rango y un tiempo de duración de cada uno. Utiliza la memoria compartida reservada por el inicializador.

Espía
---
Lee la memoria compartida reservada por el inicializador e imprime en pantalla el estado de la misma. Programas en espera, en cola, sus identificadores, entre otros datos.

Finalizador
---
Libera toda la memoria reservada, mata los procesos existentes y finaliza el ciclo de ejecución.
