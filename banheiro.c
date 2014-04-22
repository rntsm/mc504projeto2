#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define N_MAX_PESSOAS 20            
#define CHANCE_NUMERO_2 10 //%
#define TEMPO_HOMEM 1
#define TEMPO_MULHER 3
#define FALSE 0
#define TRUE 1

/*Variaveis globais*/
int hAliviados =0;
int mAliviadas=0;
int hTotal = 0;
int mTotal=0;
int numeroPessoasTotais;

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

void usaBanheiro(int tempo){               
	int num2 = rand() % 100; 
	if (num2 <= CHANCE_NUMERO_2) 
		sleep(2.5*tempo);
	else
		sleep(tempo);
}

/*Da um tempo*/
void fazerAlgoDaVida(){
	int tempo = rand() % 60;
	sleep(tempo);
}

/*Animacao do que ocorre*/
void animate(){
	
	system("clear");
	
	printf("================================\n");
	printf("==== Sexo atual no banheiro ====\n");

	if(sharedData.mulheres > 0){
	printf("====        Feminino        ====\n");
	printf("====      Quantidade: %1d     ====\n", sharedData.mulheres);
	}
	else if(sharedData.homens > 0){
	printf("====        Masculino       ====\n");
	printf("====      Quantidade: %1d     ====\n", sharedData.homens);
	}else{
	printf("====          Vazio         ====\n");
	}
	printf("================================\n");
	printf("====          Filas         ====\n");
	printf("==== Homens: %2d             ====\n", sharedData.homensEsperando);
	printf("==== Mulheres: %2d           ====\n", sharedData.mulheresEsperando);
	printf("================================\n");
	printf("====          Vezes         ====\n");
	printf("==== Homens: %2d             ====\n", hAliviados);
	printf("==== Mulheres: %2d           ====\n", mAliviadas);
	printf("================================\n");

}

/*Thread homem*/
void* homem(void *info){
	dados *banheiro=(dados*)info;
	fazerAlgoDaVida();
	pthread_mutex_lock(&banheiro->mutex);
	//Dorme enquanto existerem mulheres no banheiro ou tem 3 homens para usar
	while(banheiro->mulheres>0||banheiro->homens>2){ 			
		banheiro->homensEsperando++;
		//imprimirMensagemFila('H');
		animate();
		pthread_cond_wait(&banheiro->condHomens, &banheiro->mutex);
		banheiro->homensEsperando--;
	}
	banheiro->homens++;
	//A thread usa o banheiro e acorda as outras do outro sexo
	//printf("H no banheiro - T: %d H: %d M:%d\n", banheiro->homens, banheiro->homensEsperando,banheiro->mulheresEsperando);
	animate();
	pthread_mutex_unlock(&banheiro->mutex);
	usaBanheiro(TEMPO_HOMEM);
	pthread_mutex_lock(&banheiro->mutex);
	hAliviados++;
	//printf("H sai banheiro\n");
	animate();
	banheiro->homens--;
	if(banheiro->mulheresEsperando > 0)                    //Passa a vez para o outro sexo usar o banheiro
		pthread_cond_broadcast(&banheiro->condMulheres);
	else																									 //Se nao tiver ninguem do outro sexo acorda quem estiver dormindo
		pthread_cond_signal(&banheiro->condHomens);
	pthread_mutex_unlock(&banheiro->mutex);
return NULL;
}

/*Thread mulher*/
void* mulher(void *info){
	dados *banheiro=(dados*)info;

	fazerAlgoDaVida();
	pthread_mutex_lock(&banheiro->mutex);
	//Dorme enquanto existerem homens no banheiro ou tem 3 mulheres para usar
	while(banheiro->homens>0||banheiro->mulheres>2){
		banheiro->mulheresEsperando++;
		//imprimirMensagemFila('M');
		animate();
		pthread_cond_wait(&banheiro->condMulheres, &banheiro->mutex);
		banheiro->mulheresEsperando--;
	}
	banheiro->mulheres++;	
	//A thread usa o banheiro e acorda as outras do outro sexo
	//printf("M no banheiro - T: %d H: %d M:%d\n", banheiro->mulheres, banheiro->homensEsperando,banheiro->mulheresEsperando);
	animate();
	pthread_mutex_unlock(&banheiro->mutex);
	usaBanheiro(TEMPO_MULHER);
	pthread_mutex_lock(&banheiro->mutex);
	mAliviadas++;
	//printf("M sai banheiro\n");
	animate();
	banheiro->mulheres--;
	if(banheiro->homensEsperando > 0)
		pthread_cond_broadcast(&banheiro->condHomens);
	else
		pthread_cond_signal(&banheiro->condMulheres);
	pthread_mutex_unlock(&banheiro->mutex);
return NULL;
}

int main(){
  int i;

	while(TRUE){
		int numH, numM;
		printf("Entre o numero de homens e mulheres:\n");
		scanf("%d %d", &numH, &numM);
		numeroPessoasTotais=numH+numM;
		pthread_t threads[numeroPessoasTotais];	
		for(i=0; i<numH; i++){
			pthread_create(&threads[i], NULL, homem, &sharedData);
		}
		for(i=numH; i<numM+numH; i++){
			pthread_create(&threads[i], NULL, mulher, &sharedData);
		}
		for(i=0; i<numeroPessoasTotais; i++){
			pthread_join(threads[i], NULL);	
		}
	}

  return 0;
}
