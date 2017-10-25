// Muestra el flujo de comportamiento de la memoria compartida.
// Reserva una cantidad, consulta e imprime su información, la adjunta, escribe,
//   lee de ella, la desadjunta y la libera.
// Compilar:
//   user:~$ gcc shmgetUse.c -o shmuse -Wall
// Ejecutar:
//   user:~$ ./shmuse
// http://pubs.opengroup.org/onlinepubs/009695399/functions/shmctl.html
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>
#include "globals.h"

void error(const char *msg);

struct shmid_ds shm_id_ds;
struct shmid_ds * ptrDs = &shm_id_ds;
int shm_id; // Address of first shared memory byte
int * memy; // Map shared memory access

int main(int argc, char *argv[])
{
  int resSize = sizeof(struct sharedMemory); // Reserved size
  int err;

  // Reserve shared memory
  shm_id = shmget(IPC_PRIVATE, resSize, IPC_CREAT | 0666); // Reserve with read/write (0666) permission
  if (shm_id < 0) {
     printf("shmget error\n");
     exit(1);
  }
  printf("Reserved %i bytes starting in %i\n", resSize, shm_id);

  memy = &shm_id;

  // Load data structure of shared memory
  err = shmctl(shm_id, IPC_STAT, ptrDs);
  if(err == 0 ){
    printf("Shared memory info: \n"); // Print all data structure info
    printf("    shm_perm:\n");
    printf("        uid: %i\n", shm_id_ds.shm_perm.uid);
    printf("        gid: %i\n", shm_id_ds.shm_perm.gid);
    printf("        mode: %i\n", shm_id_ds.shm_perm.mode);
    printf("    shm_segsz: %lu\n", shm_id_ds.shm_segsz);
    printf("    shm_lpid: %i\n", shm_id_ds.shm_lpid);
    printf("    shm_cpid: %i\n", shm_id_ds.shm_cpid);
    printf("    shm_nattch: %lu\n", shm_id_ds.shm_nattch);
    printf("    shm_atime: %ld\n", shm_id_ds.shm_atime);
    printf("    shm_dtime: %ld\n", shm_id_ds.shm_dtime);
    printf("    shm_ctime: %ld\n", shm_id_ds.shm_ctime);
  }else{
    printf("Error(%i) releasing memory: %s.\n", errno, strerror(errno));
  }

  // Attach memory segment to process
  memy = (int *)shmat(shm_id, 0, 0);
  if(errno == -1){
    printf("Error attaching memory: %s.\n", strerror(errno));
  }else{
    printf("Attached memory in %i.\n", shm_id);
  }

  int i;
  // Write to shared memory
  printf("Writing...\n");
  for(i=0; i<4; i++){
      memy[i] = i*i;
  }


  // Read from shared memory
  printf("Reading...\n");
  for(i=0; i<4; i++){
      printf("memy[%i] = %i\n", i, memy[i]);
  }

  getchar();

  // Detach memory segment to process
  shmdt((void *) memy);
  if(errno == -1){
    printf("Error detaching memory: %s.\n", strerror(errno));
  }else{
    printf("Detached memory in %i.\n", shm_id);
  }

  // Release reserved memory
  err = shmctl(shm_id, IPC_RMID, ptrDs);
  if(err == 0){
    printf("Released memory in %i.\n", shm_id);
  }else{
    printf("Error releasing memory: %s.\n", strerror(errno));
  }

  return 0;
}
