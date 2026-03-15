#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>  
#include <fcntl.h>
#include <semaphore.h>
#include "message.h"
#include <sys/types.h>
#include <sys/shm.h>
/* ============================================================================= */
/*                                                                               */
/* bal3_s.c  : serveur                                                           */
/*                                                                               */
/* ============================================================================= */
#define CHECK(sts, msg) if ((sts)==-1) {perror(msg); exit(-1);}
#define PROJECTID 1
#define NB_CLIENTS 100
#define BUFFER_READ_SIZE 256
#define BUFFER_WRITE_SIZE 256
#define TAILLE_MESSAGE 256
#define KEY_MEM 1234
char message[BUFFER_WRITE_SIZE];
char bufR[BUFFER_READ_SIZE];


int main(int argc, char *argv[])
{

struct msqid_ds buf;
t_requete uneRequete;
char nomTube[50] = "tubebroker";
#define MAX_CLIENTS 10
key_t laClef;
//Init du tube broker et de la sémaphore
CHECK(laClef =ftok("broker",PROJECTID),"ftok");
mkfifo("tubebroker", 0666);
sem_t *sem = sem_open("/semaphore", O_CREAT, 0666, 1);
/* Ouverture/Création + attachement de la mémoire partagée */
int test_mem;
int *liste_partagée;
test_mem=shmget(KEY_MEM,1024,0666 | IPC_CREAT);
if(test_mem == -1){
        printf("La sémphore à eu une erreur lors de la création");
      return -1;  
}
liste_partagée  =  (int*) shmat(test_mem,NULL,0);
if(liste_partagée == (int*) -1){
        printf("La mémoire partagé ne s'est pas attachée");
        return -1;
}    
//on vide la liste 
for (int i = 0; i < NB_CLIENTS; i++) {
    liste_partagée[i] = 0;
}        

do{
        if (strlen(uneRequete.corps.msg) == 0) {
        int sortieTube = open("tubebroker", O_RDONLY);
                        read(sortieTube, &uneRequete, sizeof(uneRequete));
                        close(sortieTube);
                        }
//MP
if(uneRequete.corps.choix_menu==2){
do
{                       //recup le message reçu
                        int sortieTube = open("tubebroker", O_RDONLY);
                        read(sortieTube, &uneRequete, sizeof(uneRequete));
                        close(sortieTube);

                        sem_wait(sem);
                        //renvoie au destinataire
                        for (int i = 0; i < NB_CLIENTS; i++) {
                                
                                if (liste_partagée[i] != 0 && liste_partagée[i] == uneRequete.corps.pid_destinataire) {
                                        sprintf(nomTube, "fifo_%d", liste_partagée[i]);
                                        int entreeTube = open(nomTube, O_WRONLY | O_NONBLOCK);
                                        write(entreeTube, uneRequete.corps.msg, sizeof(uneRequete.corps.msg));
                                        close(entreeTube);
                                }    
                                

                                
                        }
                        sem_post(sem);
        

        

                
                         
        

 } while (1);
}


//BROADCAST
if(uneRequete.corps.choix_menu==1){



        
do
{       
                while (1) {
                        sem_wait(sem);
                        //affichage des clients connectés
                                printf("Clients connectes : ");
                                for (int i = 0; i < NB_CLIENTS; i++) {
                                if (liste_partagée[i] != 0) {
                                        printf("[%d] ", liste_partagée[i]);
                                }       
                        }
                        //affichage du message reçu
                        int sortieTube = open("tubebroker", O_RDONLY);
                        read(sortieTube, &uneRequete, sizeof(uneRequete));
                        printf("\n Message reçu par le tube nommé : %s\n", uneRequete.corps.msg);
                        close(sortieTube);

                        //renvoie à tout les clients
                        for (int i = 0; i < NB_CLIENTS; i++) {
                                if (liste_partagée[i] != 0) {
                                        sprintf(nomTube, "fifo_%d", liste_partagée[i]);
                                        int entreeTube = open(nomTube, O_WRONLY | O_NONBLOCK);
                                        write(entreeTube, uneRequete.corps.msg, sizeof(uneRequete.corps.msg));
                                        close(entreeTube);
                                }    

                                
                        }
                        sem_post(sem);
        }

        

                
                         
        

 } while (1);
}
} while(1);
shmdt(liste_partagée);
shmctl(test_mem, IPC_RMID, NULL);
sem_close(sem);
sem_unlink("/semaphore");
unlink("tubebroker");   
return 0;

}