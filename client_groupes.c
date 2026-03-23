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
#include <semaphore.h>
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
char nomTube[50] = "tube";
char message[TAILLE_MESSAGE]="RIEN";
CHECK(laClef =ftok("broker",PROJECTID),"ftok");
uneRequete.corps.choix_menu = 3;
int listé = 0;
int trouvé = 0;
int liste_pids[100];
int message_recu = 0;
sem_t *sem = sem_open("/semaphore", 0);
//Envoi du choix du mode 
int entreeTube = open("tubebroker", O_WRONLY | O_NONBLOCK);
            write(entreeTube, &uneRequete, sizeof(uneRequete));
            close(entreeTube);
/* Ouverture/Création + attachement de la mémoire partagée */
int test_mem;
int *liste_pids_partagée;
char (*liste_pseudos_partagée)[50];
char (*liste_groupes_partagée)[11][50];
uneRequete.corps.groupe_id = -1;
test_mem=shmget(KEY_MEM,12000,0666);
if(test_mem == -1){
        printf("La sémphore à eu une erreur lors de la création \n");
      return -1;  
}
liste_pids_partagée  =  (int*) shmat(test_mem,NULL,0);
liste_pseudos_partagée  =  (char (*)[50])(liste_pids_partagée + 100);
liste_groupes_partagée = (char (*)[11][50])(liste_pseudos_partagée + 100);
if(liste_pids_partagée == (int*) -1){
        printf("La mémoire partagé ne s'est pas attachée \n");
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
        if (read(sortieTube, message, TAILLE_MESSAGE) > 0) {
            printf("\n%s\n", message);
            printf("Saisissez le nom du groupe : ");
            fflush(stdout);
        }
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
                if (liste_pids_partagée[j] == 0)
                {
                    liste_pids_partagée[j] = getpid();
                    strcpy(liste_pseudos_partagée[j], argv[1]);
                    break;
                }
            }
        }
        for(int i=0; i<10; i++) {
        liste_pids[i] = liste_pids_partagée[i]; 
        }
        sem_post(sem);
            printf("Vos Groupes :");
            for (int j=0; j<10; j++){
            if (liste_groupes_partagée[j][0][0] != 0) {
                for (int k=1; k<11; k++){
                    if (strcmp(liste_groupes_partagée[j][k], argv[1]) == 0) {
                        printf(" [%s]", liste_groupes_partagée[j][0]);
                        break;
                    }
                }
            }
        }
        printf("\n");
        
        printf("Saisissez le nom du groupe : "); 
        fgets(leTexte,sizeof(leTexte),stdin);
        leTexte[strcspn(leTexte, "\n")] = 0;
        if (strncmp(leTexte, "EXIT", 4) == 0) {
            sem_wait(sem);
            for (int j=0; j<NB_CLIENTS; j++){
                if (liste_pids_partagée[j] == getpid())
                {
                    liste_pids_partagée[j] = 0;
                    break;
                }
            }
            sem_post(sem);
            printf("\n Sortie de l'application\n");
            break;
        }
        int groupe_existe = 0;
        for (int j=0; j<10; j++){
            if (strcmp(liste_groupes_partagée[j][0], leTexte) == 0)
            {
                uneRequete.corps.groupe_id = j;
                groupe_existe = 1;
                break;
            }
        }
        int est_membre = 0;
        if (groupe_existe) {
            for (int k = 1; k < 11; k++) {
                if (strcmp(liste_groupes_partagée[uneRequete.corps.groupe_id][k], argv[1]) == 0) {
                    est_membre = 1;
                    break;
                }
            }
        }
        if (!groupe_existe) {
            printf("Le groupe n'existe pas, souhaitez-vous le créer ? (O/N)\n");
            char reponse;
            scanf(" %c", &reponse);
            while(getchar() != '\n');
            if (reponse == 'O' || reponse == 'o') {
                for (int j = 0; j < 10; j++) {
                    if (liste_groupes_partagée[j][0][0] == 0) {
                        strcpy(liste_groupes_partagée[j][0], leTexte);
                        strcpy(liste_groupes_partagée[j][1], argv[1]);
                        uneRequete.corps.groupe_id = j;
                        break;
                    }
                }
            }
            continue;
        }
        else if (!est_membre) {
            printf("Vous n'êtes pas membre de ce groupe. Souhaitez-vous le rejoindre ? (O/N)\n");
            char reponse;
            scanf(" %c", &reponse);
            while(getchar() != '\n');
            if (reponse == 'O' || reponse == 'o') {
                for (int k = 1; k < 11; k++) {
                    if (liste_groupes_partagée[uneRequete.corps.groupe_id][k][0] == '\0') {
                        strcpy(liste_groupes_partagée[uneRequete.corps.groupe_id][k], argv[1]);
                        printf("Vous avez rejoint le groupe %s\n", leTexte);
                        break;
                    }
                }
            }
            continue;
        }
        printf("Saisissez votre message : "); 
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
            sprintf(uneRequete.corps.msg, "[%s] %s : %s", liste_groupes_partagée[uneRequete.corps.groupe_id][0], argv[1], leTexte);
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