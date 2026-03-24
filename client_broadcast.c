#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <string.h>
#include <string.h>
#include <sys/stat.h>  
#include <semaphore.h>
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
#define KEY_MEM 1234

int main(int argc, char *argv[])
{
int balId;
struct msqid_ds buf;
t_requete uneRequete;
int i=0;
#define NB_CLIENTS 100
char leTexte[100];
key_t laClef;
int liste_pids[100];
char nomTube[50] = "tube";
char message[TAILLE_MESSAGE]="RIEN";
CHECK(laClef =ftok("broker",PROJECTID),"ftok");
uneRequete.corps.choix_menu = 1;
sem_t *sem = sem_open("/semaphore", 0);
int listé = 0;
/* Ouverture/Création + attachement de la mémoire partagée */
int test_mem;
int *liste_pids_partagée;
char (*liste_pseudos_partagée)[50];
test_mem=shmget(KEY_MEM,12000,0666);
if(test_mem == -1){
        printf("La sémphore à eu une erreur lors de la création");
      return -1;  
}
liste_pids_partagée  =  (int*) shmat(test_mem,NULL,0);
liste_pseudos_partagée  =  (char (*)[50])(liste_pids_partagée + 100);
if(liste_pids_partagée == (int*) -1){
        printf("La mémoire partagé ne s'est pas attachée");
        return -1;
}  

//On crée le tube (le nom est en fonction du pid du client pour pouvoir les dissocier)
uneRequete.corps.pid_expediteur = getpid();
sprintf(nomTube, "fifo_%d", uneRequete.corps.pid_expediteur);
mkfifo(nomTube, 0666);


//lecture des tubes sans arret
if (fork() == 0) {
    while (1) {
        int sortieTube = open(nomTube, O_RDONLY);
        read(sortieTube, message, TAILLE_MESSAGE);
        printf("\n%s\n", message);
        printf("Saisissez votre message : "); 
        fflush(stdout); 
        close(sortieTube);
    }
    exit(0);
}
//saisie des msgs
for (i=0;i < 1000; i++)
{   
    sem_wait(sem);
        for (int j=0; j<NB_CLIENTS; j++){
            if (liste_pids_partagée[j] == getpid())
            {
                listé = 1;
                break;
            }
        }
        if (listé == 0)
        {
            for (int j=0; j<NB_CLIENTS; j++){
                if (liste_pids_partagée[j] != 0 && strcmp(liste_pseudos_partagée[j], argv[1]) == 0)
                {
                    sem_post(sem);
                    printf("Le pseudo '%s' est déjà utilisé. Connexion refusée.\n", argv[1]);
                    shmdt(liste_pids_partagée);
                    sem_close(sem);
                    unlink(nomTube);
                    return 1;
                }
            }
        for (int j=0; j<NB_CLIENTS; j++){
        if (liste_pids_partagée[j] == 0){
            liste_pids_partagée[j] = getpid();
            strcpy(liste_pseudos_partagée[j], argv[1]);
            break;
        }
        }
        }
        sem_post(sem);
        if(i==0) printf("Saisissez votre message : "); 
        fgets(leTexte,sizeof(leTexte),stdin);
        if (strncmp(leTexte,"EXIT",4) == 0)
        {
            sem_wait(sem);
            for (int j=0; j<NB_CLIENTS; j++){
                if (liste_pids_partagée[j] == getpid())
                {
                    liste_pids_partagée[j] = 0;
                    break;
                }
            }
            sem_post(sem);
            printf("\n Sortie de l'application");
            break;
        }
        else {
            sprintf(uneRequete.corps.msg, "%s : %s", argv[1],leTexte);
            int entreeTube = open("tubebroker", O_WRONLY);
            write(entreeTube, &uneRequete, sizeof(uneRequete));
            close(entreeTube);
        }
    }
shmdt(liste_pids_partagée);
sem_close(sem);
unlink(nomTube);
return 0;


}
