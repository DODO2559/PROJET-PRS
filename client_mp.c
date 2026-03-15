#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <string.h>
#include <string.h>
#include <sys/stat.h>  
#include <fcntl.h>
#include "message.h"
#include <sys/shm.h>
#include <sys/types.h>

/* ============================================================================= */
/*                                                                               */
/* bal3_c.c    : client                                                          */
/*                                                                               */
/* ============================================================================= */
#define CHECK(sts, msg) if ((sts)==-1) {perror(msg); exit(-1);}
#define PROJECTID 1
#define TAILLE_MESSAGE 256
#define NB_CLIENTS 100
#define KEY_MEM 1234
pid_t pids[NB_CLIENTS]={0};
int main(int argc, char *argv[])
{
int balId;
struct msqid_ds buf;
t_requete uneRequete;
int i=0;
char leTexte[100];
key_t laClef;
char nomTube[50] = "tube";
char message[TAILLE_MESSAGE]="RIEN";
CHECK(laClef =ftok("broker",PROJECTID),"ftok");
uneRequete.corps.choix_menu = 2;
/* Ouverture/Création + attachement de la mémoire partagée */
int test_mem;
int *liste_partagée;
test_mem=shmget(KEY_MEM,1024,0666 | IPC_CREAT);
if(test_mem == -1){
        printf("La sémphore à eu une erreur lors de la création");
      return -1;  
}
liste_partagée  =  (int*) shmat(test_mem,NULL,SHM_RDONLY);
if(liste_partagée == (int*) -1){
        printf("La mémoire partagé ne s'est pas attachée");
        return -1;
}  

//On crée le tube (le nom est en fonction du pid du client pour pouvoir les dissocier)
sprintf(nomTube, "fifo_%d", getpid());
mkfifo(nomTube, 0666);


//lecture des tubes
if (fork() == 0) {

    while (1) {
        int sortieTube = open(nomTube, O_RDONLY);
        read(sortieTube, message, TAILLE_MESSAGE);
        printf("\n Message reçu par le tube nommé : %s\n", message);
        close(sortieTube);
    }
    exit(0);
}

//saisie des msgs
   while (1) {
//////////////////////////// 
// MEMOIRE PARTAGEE
        for (i=0; i<NB_CLIENTS; i++){
            if (liste_partagée[i] == 0)
            {
                liste_partagée[i] = getpid();
                break;
            }
        }
        printf("Clients connectes : ");
        for (i = 0; i < NB_CLIENTS; i++) {
            if (liste_partagée[i] != 0) {
                printf("[%d] ", liste_partagée[i]);
            }       
}
///////////////////////////////        
            printf("\nSaisissez votre message : ");
            fgets(leTexte,sizeof(leTexte),stdin);
            uneRequete.corps.pid_expediteur = getpid();
            if (strncmp(leTexte,"EXIT",4) == 0)
            {
                // On libère la place dans la mémoire partagée quand on ferme le client
                for ( i = 0; i < NB_CLIENTS; i++)
                {
                    if (liste_partagée[i] == getpid())
                    {
                        liste_partagée[i] = 0;
                        break;
                    }
                }  
                strcpy(uneRequete.corps.msg,"EXIT");
                CHECK(msgsnd(balId,&uneRequete,sizeof(t_corps),0),"msgsnd");
                break;
            }
            else {
                sprintf(uneRequete.corps.msg, "%s", leTexte);
                CHECK(msgsnd(balId,&uneRequete,sizeof(t_corps),0),"msgsnd");
            }
            printf("\n");
        

}

unlink(nomTube);
return 0;


}
