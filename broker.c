#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>  
#include <fcntl.h>
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
pid_t pids[NB_CLIENTS];
char message[BUFFER_WRITE_SIZE];
char bufR[BUFFER_READ_SIZE];


int main(int argc, char *argv[])
{

struct msqid_ds buf;
t_requete uneRequete;
char nomTube[50] = "tubebroker";
#define MAX_CLIENTS 10
key_t laClef;
pid_t pids[MAX_CLIENTS]={0};

CHECK(laClef =ftok("broker",PROJECTID),"ftok");
mkfifo("tubebroker", 0666);

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
//MP
if(uneRequete.corps.choix_menu==2){
do
{
        // Lecture de tous les messages de type = 1 
        // msgflg = 0 : nous sommes en attente bloquante de messages de type 1 
        if (uneRequete.type == 1) {
                for (int i = 0; i < MAX_CLIENTS; i++) {
                        if (pids[i] == uneRequete.corps.pid_expediteur) {
                                break;
                        }
                        if (pids[i] == 0) {
                                pids[i] = uneRequete.corps.pid_expediteur;
                                break;
                        }
                }
        }
        if (uneRequete.type == 3) {
        t_requete reponse;
        reponse.type = uneRequete.corps.pid_expediteur; 
        for(int i=0; i < MAX_CLIENTS; i++) {
                reponse.corps.pids[i] = pids[i];
        }
        }
    
        else{
        //test affichage du destinataire du message
        printf("Message reçu du client %d destiné au client %d \n", uneRequete.corps.pid_expediteur, uneRequete.corps.pid_destinataire);
        //On écris dans le message de le tube attribué à chaque client
        for (int i = 0; i < MAX_CLIENTS; i++) {
                if (pids[i] != 0 && pids[i] == uneRequete.corps.pid_destinataire) {
                        sprintf(nomTube, "fifo_%d", pids[i]);
                        int entreeTube = open(nomTube, O_WRONLY | O_NONBLOCK);
                        write(entreeTube, uneRequete.corps.msg, sizeof(uneRequete.corps.msg));
                        close(entreeTube);
                }
        }
        }

        
        
        

 } while (strncmp(uneRequete.corps.msg,"EXIT",4) !=0);
}


//BROADCAST
//if(uneRequete.corps.choix_menu==1){



        
do
{       
        /*
        for (int i = 0; i < MAX_CLIENTS; i++) {
                if (pids[i] == 0) { 
                        pids[i] = uneRequete.corps.pid_expediteur;
                        break;
                } 
                if (pids[i] == uneRequete.corps.pid_expediteur){
                        break;
                }  
        }
                */

                while (1) {
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

        }

        

                
       // }                       
        

 } while (1);
//}
} while(1);

return 0;
}