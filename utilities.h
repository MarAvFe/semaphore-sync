#include <time.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// Para el entero, bastaría con poner un #define en algún fichero.h de forma que todos los programas que vayan a utilizar la memoria compartida incluyan dicho fichero y utilicen como entero el del #define
#define MEMORY_KEY 2819
