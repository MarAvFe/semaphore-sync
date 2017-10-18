// Aloca el espacio para almacenar 4 enteros en memoria compartida y luego los libera
// Compilar usual con gcc y correr sin parámetros
// http://pubs.opengroup.org/onlinepubs/009695399/functions/shmctl.html
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>

void error(const char *msg);

int main(int argc, char *argv[])
{
  int shm_id;                                                       // Address of first shared memory byte
  int err;
  int resSize = 4*sizeof(int);                                      // Reserved size

  // Reserve shared memory
  shm_id = shmget(IPC_PRIVATE, resSize, IPC_CREAT | 0666);          // Reserve with read/write permission
  if (shm_id < 0) {
     printf("shmget error\n");
     exit(1);
  }
  printf("Reserved %i bytes starting in &%i\n", resSize, shm_id);

  // Release reserved memory
  err = shmctl(shm_id, IPC_RMID, NULL);                             // Release reserved memory
  if(err == 0 ){
    printf("Released memory in %i.\n", shm_id);
  }else{
    printf("Error releasing memory: %s.\n", strerror(errno));
  }

  return 0;
}
