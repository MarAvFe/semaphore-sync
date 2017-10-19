// Genera e imprime un número aleatorio
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

int randint(int l, int h){
  return l + rand() % (h - l);
}

int main(){
  srand(time(NULL));
  printf("Número aleatorio entre [30-60]: %i\n", randint(30,60));
}
