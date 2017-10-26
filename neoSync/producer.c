#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>

#include "globals.h"

#define PAGINATION 0
#define SEGMENTATION 1
#define NUMPAGES MEMSIZE/sizeof(struct segPage)
#define handle_error_en(en, msg) \
do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
do { perror(msg); exit(EXIT_FAILURE); } while (0)

int semLogFile, semGetMemory;
int memory_id, memory_size;
struct sharedMemory * memory;

int getPageIndex();
int getEmptyPage();
int getTotalEmptyPages();
void writePage(int, int, int);
void resetPage(int);
void clearPageTableRow(int);
void setPageTableRow(int, int);
int allocateThreadShm(void *);
int releaseThreadShm(void *);
int nextThreadPage(int, int);
void dumpMemory();
void p();
void v();

static void * thread_start(void *arg){
  struct thread_info *tinfo = arg;

  printf("Thread %d: with size %i pages, to wait %i seconds.\n", tinfo->thread_num, tinfo->numPages, tinfo->timeWait);

  time_t timer;
  char buffer[26];
  struct tm* tm_info;

  p(semGetMemory, SEM_MEMORY);
  //sem_wait(semGetMemory);

  tidGettingMemory = tinfo->thread_num;
  tinfo->stage = SCHSPC;
  int alloc = 0;//allocateThreadShm(arg);

  p(semLogFile, SEM_FILE);
  //sem_wait(semLogFile);
  time(&timer);
  tm_info = localtime(&timer);
  strftime(buffer, 26, "%H:%M:%S", tm_info);

  tinfo->stage = WRTLG1;
  writeIntoFile(buffer);
  writeIntoFile(" ASSIGN: ");
  writeIntIntoFile(tinfo->thread_num);
  if (schema == PAGINATION) {
    writeIntoFile(", pages: [");
    int pastPage = 0;
    /*for (int i = 0; i < tinfo->numPages; i++) {
      pastPage = nextThreadPage(tinfo->thread_num, pastPage);
      if (pastPage < 0) break;
      if (pastPage == 0) pastPage++;
      writeIntoFile("(");
      writeIntIntoFile(i+1);
      writeIntoFile(",");
      writeIntIntoFile(pastPage);
      writeIntoFile("), ");
    }*/
    writeIntoFile("]");
  }
  writeIntoFile("\n");
  v(semLogFile, SEM_FILE);
  //sem_post(semGetMemory);

  tinfo->stage = GVMEM1;
  v(semGetMemory, SEM_MEMORY);
  //sem_post(semGetMemory);

  if (alloc != 0){
    printf("DEEEEEEED: %i\n", tinfo->thread_num);
    tinfo->stage = DED;
    return 0;
  };

  tinfo->stage = SLEEPS;
  sleep(tinfo->timeWait);

  printf("Thread %d: Finished wait.\n", tinfo->thread_num);

  tinfo->stage = WTMEM2;
  p(semGetMemory, SEM_MEMORY);
  //sem_wait(semGetMemory);

  p(semLogFile, SEM_FILE);
  //sem_wait(semLogFile);
  time(&timer);
  tm_info = localtime(&timer);
  strftime(buffer, 26, "%H:%M:%S", tm_info);
  tinfo->stage = WRTLG1;
  writeIntoFile(buffer);
  writeIntoFile(" DISIGN: ");
  writeIntIntoFile(tinfo->thread_num);
  if (schema == PAGINATION) {
    writeIntoFile(", pages: [");
    int pastPage = 0;
    /*for (int i = 0; i < tinfo->numPages; i++) {
      pastPage = nextThreadPage(tinfo->thread_num, pastPage);
      if (pastPage < 0) break;
      if (pastPage == 0) pastPage++;
      writeIntoFile("(");
      writeIntIntoFile(i+1);
      writeIntoFile(",");
      writeIntIntoFile(pastPage);
      writeIntoFile("), ");
    }*/
    writeIntoFile("]");
  }
  writeIntoFile("\n");
  //sem_post(semLogFile);
  v(semLogFile, SEM_FILE);

  tinfo->stage = RLSSPC;
  //releaseThreadShm(arg);

  tinfo->stage = GVMEM2;
  //sem_post(semGetMemory);
  v(semGetMemory, SEM_MEMORY);

  tinfo->stage = FNSHED;
  return 0;
}

int main(int argc, char * argv[]){
  srand(time(NULL));
  int s, tnum, num_threads;
  struct thread_info *tinfo;
  pthread_attr_t attr;
  void *res;
  ptrLogFile = "actions.log";
  num_threads = randint(3,14);
  memory_size = sizeof(struct sharedMemory);
  schema = PAGINATION;

  createFile();

  if ((memory_id = shmget(MEMORY_KEY, memory_size, RWPERM)) < 0) {
    perror("shmget");
    exit(1);
  }
  printf("memory_id: %d with key: %i\n",memory_id, MEMORY_KEY);
  if ((memory = shmat(memory_id, NULL, 0)) == NULL) {
    perror("shmat");
    exit(1);
  }

  // getSemaphores
  semLogFile = memory[0].semaphore;
  semGetMemory = memory[0].semaphore;
  printf("semAddr: %i\n", semLogFile);


  /* Initialize thread creation attributes */
  s = pthread_attr_init(&attr);
  if (s != 0)
  handle_error_en(s, "pthread_attr_init");

  /* Allocate memory for pthread_create() arguments */
  tinfo = calloc(num_threads, sizeof(struct thread_info));
  printf("val: %lu\n", num_threads*sizeof(struct thread_info));
  if (tinfo == NULL) handle_error("calloc");

  /* Create one thread for each command-line argument */
  int secs;
  tnum = 0;
  int meminx = 0;
  
  for (tnum = 0; tnum < num_threads; tnum++) {
    //while(1){
    tinfo[tnum].thread_num = tnum + 1;
    tinfo[tnum].timeWait = randint(2,6);
    tinfo[tnum].numPages = randint(5,10);
    tinfo[tnum].stage = WTMEM1;

    /* The pthread_create() call stores the thread ID into
    corresponding element of tinfo[] */
	
    s = pthread_create(&tinfo[tnum].thread_id, &attr, &thread_start, &tinfo[tnum]);
    if (s != 0)
    handle_error_en(s, "pthread_create");
	
	printf("bef - tinfo thread_id: %lu\n", (unsigned long) tinfo[tnum].thread_id);
	// AQUI
	
	for (meminx = 0; meminx < MEMSIZE; meminx++){
		if(memory[0].threads[meminx].thread_id == 0){
			printf("meminx: %i\n", meminx);
			memory[0].threads[meminx] = tinfo[tnum];
			//memory[0].threads[meminx].thread_num = tinfo[tnum].thread_num;
			//memory[0].threads[meminx].timeWait = tinfo[tnum].timeWait;
			//memory[0].threads[meminx].numPages = tinfo[tnum].numPages;
			//memory[0].threads[meminx].stage = tinfo[tnum].stage;
			printf("tinfo thread_id: %lu\n", (unsigned long) memory[0].threads[meminx].thread_id);
			printf("tinfo timewait: %d\n", (int) memory[0].threads[meminx].timeWait);
			break;
		}
	}
	
    secs = randint(5,10);
    //printf("Sleepin' %i secs.\n", secs);
    //sleep(randint(5,10));
    tnum++;
  }

  /* Destroy the thread attributes object, since it is no
  longer needed */

  s = pthread_attr_destroy(&attr);
  if (s != 0)
  handle_error_en(s, "pthread_attr_destroy");

  /* Now join with each thread, and display its returned value */
  sleep(2);

  /* ========== DEVELOPMENT ONLY ========== */

  // dumpMemory();

  // Reset memory spaces
  //memset(memory->fragment, -1, sizeof(struct segPage)*MEMSIZE);

  /* TestArea */
  printf("\n=== Unit tests ===\n");
  printf("MEMSIZE: %i\n", MEMSIZE);
  printf("TotalPages: %lu\n", NUMPAGES);
  //printf("pagInd(2): %i\n", getPageIndex(2));
  //printf("pagInd(6): %i\n", getPageIndex(6));
  //printf("pagInd(7): %i\n", getPageIndex(7));
  //printf("totEmptyPages: %i\n", getTotalEmptyPages());
  //printf("emptyPage: %i\n", getEmptyPage());
  //writePage(0,3,1);
  //writePage(1,3,2);
  //writePage(2,3,3);
  //printf("totEmptyPages: %i\n", getTotalEmptyPages());
  //printf("emptyPage: %i\n", getEmptyPage());
  //resetPage(1);
  //printf("emptyPage: %i\n", getEmptyPage());

  // Reset memory spaces
  //memset(memory->fragment, -1, sizeof(struct segPage)*MEMSIZE);
  /* ======== END DEVELOPMENT ONLY ======== */

  shmdt(memory);
  free(tinfo);
  exit(EXIT_SUCCESS);
}

/* ===== SEMAPHORE MANAGEMENT =====*/
void p(int semid, int numSem) {
    struct sembuf sops;
    sops.sem_num = numSem;
    sops.sem_op = 1;
    sops.sem_flg = 0;

    if (semop(semid, &sops, 1) == -1) {
        perror("Sem P error");
        exit(EXIT_FAILURE);
    }
}

void v(int semid, int numSem) {
    struct sembuf sops;
    sops.sem_num = numSem;
    sops.sem_op = -1;
    sops.sem_flg = 0;

    if (semop(semid, &sops, 1) == -1) {
      perror("Sem V error");
      exit(EXIT_FAILURE);
    }
}
