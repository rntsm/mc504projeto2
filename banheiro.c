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

/*Declaracao de metodos*/
void usaBanheiro();

/*Ainda falta implementar*/
//So 3 de cada sexo entram por vez no banheiro - talvez precisa usar barreira/semaforo. Nao sei se da pra fazer tudo com mutex
//Scanf do parametro de pessoas que querem usar o banheiro
//Algum controle para que se menos de 3 pessoas de um dado sexo querem entrar elas possam entrar tb.


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
