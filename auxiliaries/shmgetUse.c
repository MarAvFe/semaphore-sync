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

struct shmid_ds /*{
  struct ipc_perm shm_perm,
  size_t shm_segsz,
  pid_t shm_lpid,
  pid_t shm_cpid,
  shmatt_t shm_nattch,
  time_t shm_atime,
  time_t shm_dtime,
  time_t shm_ctime
}*/ shm_id_ds;
struct shmid_ds * ptrDs = &shm_id_ds;

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
  err = shmctl(shm_id, IPC_STAT, ptrDs);                             // Load data structure of shared memory
  if(err == 0 ){
    printf("Sum intel: \n");                                         // Print all data structure info
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

  // Release reserved memory
  err = shmctl(shm_id, IPC_RMID, NULL);                             // Release reserved memory
  if(err == 0 ){
    printf("Released memory in %i.\n", shm_id);
  }else{
    printf("Error releasing memory: %s.\n", strerror(errno));
  }

  return 0;
}
