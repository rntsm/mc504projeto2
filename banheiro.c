#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define N_MAX_PESSOAS 6            //Mudar para um scanf
#define FALSE 0
#define TRUE 1

/*Variaveis globais*/
pthread_mutex_t mutex;         
pthread_cond_t banheiroOcupado;    //Determina se o banheiro esta ocupado
int ocupado=FALSE;

typedef struct{
	int homens;
	int mulheres;
	int homem_esperando;
	int mulher_esperando;
	pthread_mutex_t mutex;
	pthread_cond_t condHomens;
	pthread_cond_t condMulheres;
}dados;

static dados sharedData={
	.homens=0,
	.mulheres=0,
	.homem_esperando=0,
	.mulher_esperando=0,
	.mutex=PTHREAD_MUTEX_INITIALIZER,
	.condHomens=PTHREAD_COND_INITIALIZER,
	.condMulheres=PTHREAD_COND_INITIALIZER
};

/*Declaracao de metodos*/
void usaBanheiro();

/*Ainda falta implementar*/
//So 3 de cada sexo entram por vez no banheiro - talvez precisa usar barreira/semaforo. Nao sei se da pra fazer tudo com mutex
//Scanf do parametro de pessoas que querem usar o banheiro
//Algum controle para que se menos de 3 pessoas de um dado sexo querem entrar elas possam entrar tb.


void* homemTentaUsar(void *info){
	dados *banheiro=(dados*)info;
	while(TRUE){
		pthread_mutex_lock(&banheiro->mutex);
		banheiro->homens++;
		while(banheiro->mulheres>0||banheiro->homens>=3){ 							//Dorme enquanto existerem mulheres no banheiro ou tem 3 homens para usar
			pthread_cond_wait(&banheiro->condHomens, &banheiro->mutex);	    
		}
		usaBanheiro();																									//A thread usa o banheiro e acorda as outras do outro sexo
		pthread_cond_broadcast(&banheiro->condMulheres);
		pthread_mutex_unlock(&banheiro->mutex);
	}
	return NULL;
}

void* mulher(void *info){
  dados *banheiro = (dados*) info;
  
  while(TRUE){
    pthread_mutex_lock(&banheiro->mutex);
    while(banheiro->homens>0 || banheiro->mulheres>=3){
      mulher_esperando++;
      pthread_cond_wait(&banheiro->condMulher, &banheiro->mutex);
      mulher_esperando--;
    }
    banheiro->mulheres++;
    pthread_mutex_unlock(&banheiro->mutex);
    usaBanheiro(MULHER, getpid());
    
    pthread_mutex_lock(&banheiro->mutex);
    banheiro->mulheres--;
    if(homem_esperando)
      pthread_cond_broadcast(&banheiro->condHomem);
    else
      pthread_cond_broadcast(&banheiro->condMulher);
    pthread_mutex_unlock(&banheiro->mutex);    
  }

  return NULL;
}


void* tentaAbrirBanheiro(){        //Talvez mudar, outra funcao deve chamar esta (?)
  pthread_mutex_lock(&mutex);
  if(ocupado==FALSE){
    //Entra no banheiro
    ocupado = TRUE;
    usaBanheiro();
    ocupado = FALSE;               //Tirar daqui depois essa atribuicao
  }else{
    pthread_cond_wait(&banheiroOcupado, &mutex);  //Se estiver ocupado, dorme ate vagar
  }
  pthread_mutex_unlock(&mutex);

	return NULL;
}

void usaBanheiro(){               //Fazer algo aqui
  printf("Entrei no banheiro\n");
}

int main(){
  pthread_t threads[N_MAX_PESSOAS];
  int i;
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&banheiroOcupado, NULL);

  for(i=0; i<N_MAX_PESSOAS; i++){
    pthread_create(&threads[i], NULL, tentaAbrirBanheiro, NULL);   //Mudar para outra funcao depois, passar parametros talvez
  }

  for(i=0; i<N_MAX_PESSOAS; i++){
    pthread_join(threads[i], NULL);
  }

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&banheiroOcupado);
  return 0;
}
