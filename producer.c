#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include "api.h"

#define PAGINATION 0
#define SEGMENTATION 1
#define PAGESIZE 2
#define NUMPAGES MEMSIZE/PAGESIZE

#define handle_error_en(en, msg) \
do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
do { perror(msg); exit(EXIT_FAILURE); } while (0)


sem_t semLogFile, semGetMemory;
int pageTable[NUMPAGES];
int * memory, memory_id, memory_size;
int threads_size, threads_id, *threadPtrs;

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

static void * thread_start(void *arg){
  struct thread_info *tinfo = arg;

  printf("Thread %d: size %i pages, to wait %i seconds.\n", tinfo->thread_num, tinfo->numPages, tinfo->timeWait);

  time_t timer;
  char buffer[26];
  struct tm* tm_info;

  sem_wait(&semGetMemory);
  tidGettingMemory = tinfo->thread_num;
  tinfo->stage = SCHSPC;
  int alloc = allocateThreadShm(arg);

  sem_wait(&semLogFile);
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
    for (int i = 0; i < tinfo->numPages; i++) {
      pastPage = nextThreadPage(tinfo->thread_num, pastPage);
      if (pastPage < 0) break;
      if (pastPage == 0) pastPage++;
      writeIntoFile("(");
      writeIntIntoFile(i+1);
      writeIntoFile(",");
      writeIntIntoFile(pastPage);
      writeIntoFile("), ");
    }
    writeIntoFile("]");
  }
  writeIntoFile("\n");
  sem_post(&semLogFile);

  tinfo->stage = GVMEM1;
  sem_post(&semGetMemory);

  if (alloc != 0){
    printf("DEEEEEEED: %i\n", tinfo->thread_num);
    tinfo->stage = DED;
    return 0;
  };

  tinfo->stage = SLEEPS;
  sleep(tinfo->timeWait);

  printf("Thread %d: Finished wait.\n", tinfo->thread_num);

  tinfo->stage = WTMEM2;
  sem_wait(&semGetMemory);

  sem_wait(&semLogFile);
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
    for (int i = 0; i < tinfo->numPages; i++) {
      pastPage = nextThreadPage(tinfo->thread_num, pastPage);
      if (pastPage < 0) break;
      if (pastPage == 0) pastPage++;
      writeIntoFile("(");
      writeIntIntoFile(i+1);
      writeIntoFile(",");
      writeIntIntoFile(pastPage);
      writeIntoFile("), ");
    }
    writeIntoFile("]");
  }
  writeIntoFile("\n");
  sem_post(&semLogFile);

  tinfo->stage = RLSSPC;
  releaseThreadShm(arg);

  tinfo->stage = GVMEM2;
  sem_post(&semGetMemory);

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
	memory_size = MEMSIZE * UNITSIZE;
	threads_size = LIMITPROCESSES * sizeof(pthread_t);
  schema = PAGINATION;

  createFile();


  if ((memory_id = shmget(MEMORY_KEY, memory_size, RWPERM)) < 0) {
		perror("shmget");
		exit(1);
	}
  if ((memory = shmat(memory_id, NULL, 0)) == NULL) {
		perror("shmat");
		exit(1);
	}

  if ((threads_id = shmget(THREADS_KEY, threads_size, RWPERM)) < 0) {
    perror("shmget threads");
    exit(1);
  }

  if ((threadPtrs = shmat(threads_id, NULL, 0)) == NULL) {
    perror("shmat threads");
    exit(1);
  }

  sem_init(&semLogFile, 0, 1);
  sem_init(&semGetMemory, 0, 1);

  /* Initialize thread creation attributes */
  s = pthread_attr_init(&attr);
  if (s != 0)
  handle_error_en(s, "pthread_attr_init");

  /* Allocate memory for pthread_create() arguments */
  tinfo = calloc(num_threads, sizeof(struct thread_info));
  printf("val: %lu\n", num_threads*sizeof(struct thread_info));
  printf("val: %i\n", threads_size);
  if (tinfo == NULL) handle_error("calloc");

  /* Create one thread for each command-line argument */
  int secs;
  tnum = 0;
  //for (tnum = 0; tnum < num_threads; tnum++) {
  while(1){
    tinfo[tnum].thread_num = tnum + 1;
    tinfo[tnum].timeWait = randint(2,6);
    tinfo[tnum].numPages = randint(5,10);
    tinfo[tnum].stage = WTMEM1;

    /* The pthread_create() call stores the thread ID into
    corresponding element of tinfo[] */

    s = pthread_create(&tinfo[tnum].thread_id, &attr, &thread_start, &tinfo[tnum]);
    if (s != 0)
    handle_error_en(s, "pthread_create");

    secs = randint(5,10);
    printf("Sleepin' %i secs.\n", secs);
    sleep(randint(5,10));
    tnum++;
  }

  /* Destroy the thread attributes object, since it is no
  longer needed */

  s = pthread_attr_destroy(&attr);
  if (s != 0)
  handle_error_en(s, "pthread_attr_destroy");

  /* Now join with each thread, and display its returned value */
  //sleep(2);
  for (tnum = 0; tnum < num_threads; tnum++) {
    s = pthread_join(tinfo[tnum].thread_id, &res);
    if (s != 0)
    handle_error_en(s, "pthread_join");


    // printf("Joined with thread %d; returned value was %s\n", tinfo[tnum].thread_num, (char*)res);
    free(res);      /* Free memory allocated by thread */
  }

  /* ========== DEVELOPMENT ONLY ========== */

  // dumpMemory();

  // Reset memory spaces
	for (int i = 0; i < memory_size; i++) {
		memory[i] = -1;
	}

  /* TestArea */
  printf("\n=== Unit tests ===\n");
  printf("MEMSIZE: %i\n", MEMSIZE);
  printf("TotalPages: %i\n", NUMPAGES);
  printf("pagInd(2): %i\n", getPageIndex(2));
  printf("pagInd(6): %i\n", getPageIndex(6));
  printf("pagInd(7): %i\n", getPageIndex(7));
  printf("totEmptyPages: %i\n", getTotalEmptyPages());
  printf("emptyPage: %i\n", getEmptyPage());
  writePage(0,3,1);
  writePage(1,3,2);
  writePage(2,3,3);
  printf("totEmptyPages: %i\n", getTotalEmptyPages());
  printf("emptyPage: %i\n", getEmptyPage());
  resetPage(1);
  printf("emptyPage: %i\n", getEmptyPage());

  // Reset memory spaces
	for (int i = 0; i < memory_size; i++) {
		memory[i] = -1;
	}
  /* ======== END DEVELOPMENT ONLY ======== */

  shmdt(memory);
  shmdt(threadPtrs);
  free(tinfo);
  //exit(EXIT_SUCCESS);
}

/* PAGING HELPERS */
int getPageIndex(int page){
  return page * PAGESIZE;
}

int getEmptyPage(){
  int k;
  for (int i = 0; i < NUMPAGES; i++) {
    k = getPageIndex(i);
    if (memory[k] == -1) {
      return k;
    }
  }
  return -1;
}

int getProcessPage(int tid){
  int k;
  for (int i = 0; i < NUMPAGES; i++) {
    k = getPageIndex(i);
    if (memory[k] == tid) {
      return k;
    }
  }
  return -1;
}

int getTotalEmptyPages(){
  int k, res = 0;
  for (int i = 0; i < NUMPAGES; i++) {
    k = getPageIndex(i);
    if (memory[k] == -1) {
      res++;
    }
  }
  return res;
}

void writePage(int pageNum, int threadId, int threadPageNum){
  int ind = pageNum;
  memory[ind] = threadId;
  memory[ind+1] = threadPageNum;
  //printf("Writing to page %i: tid(%i) thrPage(%i)\n", ind, threadId, threadPageNum);
}

void resetPage(int pageNum){
  memory[getPageIndex(pageNum)] = -1;
}

void setPageTableRow(int logicPage, int physicalPage){
  pageTable[logicPage] = physicalPage;
}

void clearPageTableRow(int logicPage){
  pageTable[logicPage] = -1;
}

int getPhysicalPage(int logicPage){
  return pageTable[logicPage];
}

int allocateThreadShm(void *arg){
  struct thread_info *tinfo = arg;
  printf("Allocating %i pages from tid: %i.\n", tinfo->numPages, tinfo->thread_num);
  if (schema == PAGINATION) {
    int empPag = getTotalEmptyPages();
    if (empPag < tinfo->numPages) return -1;
    for (int i = 0; i < tinfo->numPages; i++) {
      writePage(getEmptyPage(), tinfo->thread_num, i+1);
    }
  }else{
    printf("SEGMENTATION NOT IMPLEMENTED. QUITTING.\n");
    exit(EXIT_FAILURE);
  }
  return 0;
}

int releaseThreadShm(void *arg){
  struct thread_info *tinfo = arg;
  printf("Releasing %i pages from tid: %i.\n", tinfo->numPages, tinfo->thread_num);
  if (schema == PAGINATION) {
    int empPag = getTotalEmptyPages();
    if (empPag < tinfo->numPages) return -1;
    for (int i = 0; i < tinfo->numPages; i++) {
      writePage(getProcessPage(tinfo->thread_num), -1, -1);
    }
  }else{
    printf("SEGMENTATION NOT IMPLEMENTED. QUITTING.\n");
    exit(EXIT_FAILURE);
  }
  return 0;
}

int nextThreadPage(int tid, int pastPage){
  int k;
  if (pastPage != 0) pastPage++;
  for (int i = pastPage; i < NUMPAGES; i++) {
    k = getPageIndex(i);
    if (memory[k] == tid) {
      return i;
    }
  }
  return -1;
}

void dumpMemory(){
printf("MEM-DUMP (%p):\n", memory);
  for (int i = 0; i < MEMSIZE; i++) {
    printf("%i ", memory[i]);
    if (i % PAGESIZE == PAGESIZE-1) printf(",");
    if (i % 32 == 31) printf("\n");
  }
  printf("\n");
}
