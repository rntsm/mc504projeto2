#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define N_MAX_PESSOAS 20            //Mudar para um scanf
#define CHANCE_NUMERO_2 10 //%
#define TEMPO_HOMEM 1
#define TEMPO_MULHER 3
#define FALSE 0
#define TRUE 1

/*Variaveis globais*/

typedef struct{
	int homens;
	int homensEsperando;
	int mulheres;
	int mulheresEsperando;
	pthread_mutex_t mutex;
	pthread_cond_t condHomens;
	pthread_cond_t condMulheres;
}dados;

static dados sharedData={
	.homens=0,
	.homensEsperando=0,
	.mulheres=0,
	.mulheresEsperando=0,
	.mutex=PTHREAD_MUTEX_INITIALIZER,
	.condHomens=PTHREAD_COND_INITIALIZER,
	.condMulheres=PTHREAD_COND_INITIALIZER
};

/*Declaracao de metodos*/


/*Ainda falta implementar*/
//So 3 de cada sexo entram por vez no banheiro - talvez precisa usar barreira/semaforo. Nao sei se da pra fazer tudo com mutex
//Scanf do parametro de pessoas que querem usar o banheiro
//Algum controle para que se menos de 3 pessoas de um dado sexo querem entrar elas possam entrar tb.

void imprimirMensagemFila(char pes){
	//printf("%c na fila - T: %d H: %d M:%d\n", pes, pes>0 ? sharedData.homens : sharedData.mulheres, sharedData.homensEsperando,sharedData.mulheresEsperando);
}

void usaBanheiro(int tempo){               //Fazer algo aqui
	int num2 = rand() % 100; 
	if (num2 <= CHANCE_NUMERO_2) 
		sleep(2.5*tempo);
	else
		sleep(tempo);
}

void fazerAlgoDaVida(){
	int tempo = rand() % 60;
	sleep(tempo);
}

void* homem(void *info){
	dados *banheiro=(dados*)info;
	while(TRUE){
		fazerAlgoDaVida();
		pthread_mutex_lock(&banheiro->mutex);
		//Dorme enquanto existerem mulheres no banheiro ou tem 3 homens para usar
		while(banheiro->mulheres>0||banheiro->homens>2){ 			
			banheiro->homensEsperando++;
			imprimirMensagemFila('H');
			pthread_cond_wait(&banheiro->condHomens, &banheiro->mutex);
			banheiro->homensEsperando--;
		}
		banheiro->homens++;
		
		//A thread usa o banheiro e acorda as outras do outro sexo
		printf("H no banheiro - T: %d H: %d M:%d\n", banheiro->homens, banheiro->homensEsperando,banheiro->mulheresEsperando);
		pthread_mutex_unlock(&banheiro->mutex);
		usaBanheiro(TEMPO_HOMEM);
		pthread_mutex_lock(&banheiro->mutex);
		printf("H sai banheiro\n");
		banheiro->homens--;
		if(banheiro->mulheresEsperando > 0)
			pthread_cond_broadcast(&banheiro->condMulheres);
		else
			pthread_cond_signal(&banheiro->condHomens);
		pthread_mutex_unlock(&banheiro->mutex);
	}
	return NULL;
}

void* mulher(void *info){
	dados *banheiro=(dados*)info;
	while(TRUE){
		fazerAlgoDaVida();
		pthread_mutex_lock(&banheiro->mutex);
		//Dorme enquanto existerem homens no banheiro ou tem 3 mulheres para usar
		while(banheiro->homens>0||banheiro->mulheres>2){
 			banheiro->mulheresEsperando++;
			imprimirMensagemFila('M');
			pthread_cond_wait(&banheiro->condMulheres, &banheiro->mutex);
			banheiro->mulheresEsperando--;
		}
		banheiro->mulheres++;
		
		//A thread usa o banheiro e acorda as outras do outro sexo

		printf("M no banheiro - T: %d H: %d M:%d\n", banheiro->mulheres, banheiro->homensEsperando,banheiro->mulheresEsperando);
		pthread_mutex_unlock(&banheiro->mutex);
		usaBanheiro(TEMPO_MULHER);
		pthread_mutex_lock(&banheiro->mutex);
		printf("M sai banheiro\n");
		banheiro->mulheres--;
		if(banheiro->homensEsperando > 0)
			pthread_cond_broadcast(&banheiro->condHomens);
		else
			pthread_cond_signal(&banheiro->condMulheres);
		pthread_mutex_unlock(&banheiro->mutex);
	}
	return NULL;
}

int main(){
  pthread_t threads[N_MAX_PESSOAS];
  int i;
  //pthread_mutex_init(&mutex, NULL);
  //pthread_cond_init(&banheiroOcupado, NULL);

  for(i=0; i<N_MAX_PESSOAS; i++){
    pthread_create(&threads[i], NULL, i % 2 == 0 ? mulher : homem, &sharedData);
  }

  for(i=0; i<N_MAX_PESSOAS; i++){
    pthread_join(threads[i], NULL);
  }

  //pthread_mutex_destroy(&mutex);
  //pthread_cond_destroy(&banheiroOcupado);
  return 0;
}
