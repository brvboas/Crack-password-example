#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define N_ITENS 10
int buffer[N_ITENS];

int inicio = 0, final = 0;

sem_t bloqueio, pos_vazia, pos_ocupada;

void* produtor(void *v) {
  int i, aux;

  for (i = 0; i < 2 * N_ITENS; i++) {
    sem_wait(&pos_vazia);
    sem_wait(&bloqueio);
    final = (final + 1) % N_ITENS;
    buffer[final] = i;
    printf("Produtor, item = %d, posição = %d.\n", i, final);     
    sem_getvalue(&pos_vazia, &aux); 
    sleep(random() % 3);   /* Permite que a outra thread execute */
    printf("Produtor total = %d.\n", N_ITENS - aux);     
    sem_post(&bloqueio);
    sem_post(&pos_ocupada);
    //sleep(random() % 3);   /* Permite que a outra thread execute */
  }
  printf("Produção encerrada.\n");     
  return NULL;
}

void* consumidor(void *v) {
  int i, aux;

  for (i = 0; i < 2 * N_ITENS; i++) {
    sem_wait(&pos_ocupada);
    sem_wait(&bloqueio);
    inicio = (inicio + 1) % N_ITENS;
    aux = buffer[inicio]; /* Item é consumido */
    printf("Consumidor, item = %d, posição = %d.\n", aux, inicio);
    sem_getvalue(&pos_ocupada, &aux); 
    sleep(random() % 3);   /* Permite que a outra thread execute */
    printf("Consumidor total = %d.\n", aux);     
    sem_post(&bloqueio);
    sem_post(&pos_vazia);
    //sleep(random() % 3);   /* Permite que a outra thread execute */
  }
  printf("Consumo encerrado.\n");     
  return NULL;
}

int main() {
  int i;
  
  pthread_t thr_produtor, thr_consumidor;

  sem_init(&bloqueio, 0, 1);
  sem_init(&pos_vazia, 0, N_ITENS);
  sem_init(&pos_ocupada, 0, 0);

  /* Limpa o buffer */
  for (i = 0; i < N_ITENS; i++)
    buffer[i] = -1;
  
  pthread_create(&thr_produtor, NULL, produtor, NULL);
  pthread_create(&thr_consumidor, NULL, consumidor, NULL);

  pthread_join(thr_produtor, NULL); 
  pthread_join(thr_consumidor, NULL);

  return 0;
}
