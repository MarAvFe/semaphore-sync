//
/*
Crea, escribe y lee múltiples enteros separados por espacios de un archivo global
Compilar:
  user~:$ gcc rwFile.c -o rwf -Wall
Ejecutar:
  user~:$ ./rwf fileName.ext
*/
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void createFile();
void writeIntoFile(char*);
void readFromFile();

FILE *file;
char * sourcefile;

int main(int argc, char * argv[]){
  sourcefile = argv[1];
  printf("\nStart with: %s\n\n", sourcefile);
  char * spc = " ";
  char * buf1 = "26463";
  char * buf2 = "6345";
  char * buf3 = "23";
  char * buf4 = "345236234";

  createFile();
  writeIntoFile(buf1);
  writeIntoFile(spc);
  writeIntoFile(buf2);
  writeIntoFile(spc);
  writeIntoFile(buf3);
  writeIntoFile(spc);
  writeIntoFile(buf4);
  readFromFile();

  printf("\nFinish.\n");
  return 0;
}

void createFile(){
    printf("Creating file %s...\n", sourcefile);
    unsigned char buffer[0] = "";
    file = fopen(sourcefile, "w");

    if (file != NULL){
      fprintf(file, "%s", buffer);
    }
    fclose(file);
}

void writeIntoFile(char * buf){
    printf("Writing \"%s\" into file %s...\n", buf, sourcefile);
    file = fopen(sourcefile, "a");

    if (file != NULL){
      fprintf(file, "%s\n", buf);
    }
    fclose(file);
}

void readFromFile(){
  printf("Reading from file %s...\n", sourcefile);
  size_t bytesRead = 0;
  file = fopen(sourcefile, "r");
  char buffer[255];
  int nums[255];
  int numInd = 0;
  char * tmpNum, *bufPtr = buffer;
  if (file != NULL){
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0){
      // printf("Read: %s\n", buffer);
      while ((tmpNum = strsep(&bufPtr, " ")) != NULL){ // Separa los numeros leidos por espacio
        nums[numInd] = atoi(tmpNum);
        printf("gotNum: %i\n", nums[numInd]);
        numInd++;
      }
    }
  }
  fclose(file);
};
