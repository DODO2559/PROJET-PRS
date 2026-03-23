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
int *liste_pids_partagée;
char (*liste_pseudos_partagée)[50];
char (*liste_groupes_partagée)[11][50];
test_mem=shmget(KEY_MEM, 12000,0666 | IPC_CREAT);
if(test_mem == -1){
        printf("La sémphore à eu une erreur lors de la création");
      return -1;  
}
liste_pids_partagée  =  (int*) shmat(test_mem,NULL,0);
liste_pseudos_partagée  =  (char (*)[50])(liste_pids_partagée + 100);
liste_groupes_partagée = (char (*)[11][50])(liste_pseudos_partagée + 100);
if(liste_pids_partagée == (int*) -1){
        printf("La mémoire partagé ne s'est pas attachée");
        return -1;
}    
//on vide la liste 
for (int i = 0; i < NB_CLIENTS; i++) {
    liste_pids_partagée[i] = 0;
    liste_pseudos_partagée[i][0] = '\0';
}     
for (int i = 0; i < 10; i++) { 
    for (int j = 0; j < 11; j++) {
        liste_groupes_partagée[i][j][0] = '\0';
    }
}

do{
        int sortieTube = open("tubebroker", O_RDONLY);
        read(sortieTube, &uneRequete, sizeof(uneRequete));
        close(sortieTube);
                        
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
                                
                                if (liste_pids_partagée[i] != 0 && liste_pids_partagée[i] == uneRequete.corps.pid_destinataire) {
                                        sprintf(nomTube, "fifo_%d", liste_pids_partagée[i]);
                                        int entreeTube = open(nomTube, O_WRONLY);
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
                        //recup le message reçu
                        int sortieTube = open("tubebroker", O_RDONLY);
                        read(sortieTube, &uneRequete, sizeof(uneRequete));
                        close(sortieTube);
                        sem_wait(sem);

                        //renvoie à tout les clients
                        if (strlen(uneRequete.corps.msg) > 0) {
                        for (int i = 0; i < NB_CLIENTS; i++) {
                                if (liste_pids_partagée[i] != 0) {
                                        sprintf(nomTube, "fifo_%d", liste_pids_partagée[i]);
                                        int entreeTube = open(nomTube, O_WRONLY);
                                        write(entreeTube, uneRequete.corps.msg, sizeof(uneRequete.corps.msg));
                                        close(entreeTube);
                                }    

                                
                        }
                }
                        sem_post(sem);
        }

        

                
                         
        

 } while (1);
}
//mode groupe
if(uneRequete.corps.choix_menu==3){



        
do
{       
                while (1) {
                        //recup le message reçu
                        int sortieTube = open("tubebroker", O_RDONLY);
                        read(sortieTube, &uneRequete, sizeof(uneRequete));
                        close(sortieTube);
                        sem_wait(sem);

                        //renvoie à tout les clients
                        if (uneRequete.corps.groupe_id >= 0 && uneRequete.corps.groupe_id < 10) {
                        for (int i = 1; i < 10; i++) {
                                if (liste_groupes_partagée[uneRequete.corps.groupe_id][i][0] != '\0') {
                                        for (int j = 0; j < NB_CLIENTS; j++) {
                                                if (strcmp(liste_pseudos_partagée[j], liste_groupes_partagée[uneRequete.corps.groupe_id][i]) == 0) {
                                                        uneRequete.corps.pid_destinataire = liste_pids_partagée[j];
                                                        break;
                                                }
                                        }
                                        if(uneRequete.corps.pid_destinataire != uneRequete.corps.pid_expediteur){
                                        sprintf(nomTube, "fifo_%d", uneRequete.corps.pid_destinataire);
                                        int entreeTube = open(nomTube, O_WRONLY);
                                        write(entreeTube, uneRequete.corps.msg, sizeof(uneRequete.corps.msg));
                                        close(entreeTube);
                                        }
                                }
                        }    
                        }
                        sem_post(sem);
                }
                
        }while (1);

        

                
                         
        

 
}
} while(1);
shmdt(liste_pids_partagée);
shmctl(test_mem, IPC_RMID, NULL);
sem_close(sem);
sem_unlink("/semaphore");
unlink("tubebroker");   
return 0;

}