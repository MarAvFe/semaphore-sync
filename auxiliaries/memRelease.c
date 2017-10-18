// Libera la memoria reservada en la dirección recibida como parámetro
// Compilar usual con gcc y correr con un parámetro (int)
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

  if (argc == 2) {
    shm_id = atoi(argv[1]);
  }else{
    printf("Missing argument address\n");
    exit(1);
  }

  // Release reserved memory
  err = shmctl(shm_id, IPC_RMID, NULL);                             // Release reserved memory
  if(err == 0 ){
    printf("Released memory in %i.\n", shm_id);
  }else{
    printf("Error releasing memory: %s.\n", strerror(errno));
  }

  return 0;
}
