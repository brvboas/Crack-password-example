#define _GNU_SOURCE
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <crypt.h>
#include <stdlib.h>
#include <semaphore.h>

#define TAM_HASH 256
#define TAM_SENHA 4
#define N_ITENS 20

/*buffer para armazenar as senhas candidatas*/
char buffer[N_ITENS][TAM_SENHA+1];

/*argumento do main*/
char *argumento;

/*-----------------variaveis semaphore-----------------*/
int inicio = 0, final = 0;
sem_t bloqueio, pos_vazia, pos_ocupada;




/* Obtém o hash a partir de uma senha e coloca o resultado em hash.
   O vetor hash deve ter pelo menos 14 elementos. */
void calcula_hash_senha(const char *senha, char *hash);
void incrementa_senha(char *senha);
void testa_senha(const char *hash_alvo, const char *senha);
void* produtor(void *v);
void* consumidor(void *v);

/*--Main------------------------------------------------------------------------*/


int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("Uso: %s <hash>", argv[0]);
    return 1;
  }
  argumento = argv[1];

 // int i;
  
  pthread_t thr_produtor, thr_consumidor;

  sem_init(&bloqueio, 0, 1);
  sem_init(&pos_vazia, 0, N_ITENS);
  sem_init(&pos_ocupada, 0, 0);

  /* Limpa o buffer */
 /// for (i = 0; i < N_ITENS; i++)
////	buffer[i][TAM_SENHA+1] = '\0';
  
/*cria um produtor e dois consumidores...*/
  pthread_create(&thr_produtor, NULL, produtor, NULL);
  pthread_create(&thr_consumidor, NULL, consumidor, NULL);
  pthread_create(&thr_consumidor, NULL, consumidor, NULL);

  pthread_join(thr_produtor, NULL); 
  pthread_join(thr_consumidor, NULL);

  return 0;

}

/*--Testa senha-----------------------------------------------------------------*/


void testa_senha(const char *hash_alvo, const char *senha) {
  char hash_calculado[TAM_HASH + 1];

  calcula_hash_senha(senha, hash_calculado);
  if (!strcmp(hash_alvo, hash_calculado)) {
    printf("Achou! %s\n", senha);
    exit(0);
  }
}

/*--Incrementa senha------------------------------------------------------------*/

void incrementa_senha(char *senha) {
  int i;

  i = TAM_SENHA - 1;
  while (i >= 0) {
    if (senha[i] != 'z') {
      senha[i]++;
      i = -2;
    } else {
      senha[i] = 'a';
      i--;
    }
  }
  if (i == -1) {
    printf("Não achou!\n");
    exit(1);
  }
}

/*--Calcula hash senha----------------------------------------------------------*/


void calcula_hash_senha(const char *senha, char *hash) {
  struct crypt_data data;
  data.initialized = 0;
  strcpy(hash, crypt_r(senha, "aa", &data));
}

/*--PRODUTOR--------------------------------------------------------------------*/

void* produtor(void *v) {

  int i;
  char senha[TAM_SENHA+1];

  for (i = 0; i < TAM_SENHA; i++) {
    senha[i] = 'a';
  }
  senha[TAM_SENHA] = '\0';

  while(1){   
    final = (final + 1) % N_ITENS;
    sem_wait(&pos_vazia);
    sem_wait(&bloqueio);
    strcpy(buffer[final],senha);
    sem_post(&bloqueio);
    sem_post(&pos_ocupada);
    incrementa_senha(senha);   
  }
  printf("Produção encerrada.\n");     
  return NULL;
}

/*--CONSUMIDOR------------------------------------------------------------------*/

void* consumidor(void *v) {
char hash[TAM_SENHA];
  while(1) {
    inicio = (inicio + 1) % N_ITENS;
    sem_wait(&pos_ocupada);
    sem_wait(&bloqueio);
    strcpy(hash,buffer[inicio]);
    sem_post(&bloqueio);
    sem_post(&pos_vazia);
    testa_senha(argumento,hash); /* Item é consumido */
  }
  printf("Consumo encerrado.\n");     
  return NULL;
}


