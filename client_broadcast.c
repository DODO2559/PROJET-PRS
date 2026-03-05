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
#define KEY_MEM 1234

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
int test_att;
CHECK(laClef =ftok("broker",PROJECTID),"ftok");
uneRequete.corps.choix_menu = 1;

//On crée le tube (le nom est en fonction du pid du client pour pouvoir les dissocier)
sprintf(nomTube, "fifo_%d", uneRequete.corps.pid_expediteur);
mkfifo(nomTube, 0666);
// j'attache le processus à la mémoire partagée
//test_att = shmat(KEY_MEM,NULL,0);
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
for (i=0;i < 10; i++)
{
        printf("\nSaisissez votre message : ");
        fgets(leTexte,sizeof(leTexte),stdin);
        if (strncmp(leTexte,"EXIT",4) == 0)
        {
            printf("\n Sortie de l'application");
            break;
        }
        else {
            sprintf(uneRequete.corps.msg, "%s", leTexte);
            int entreeTube = open(nomTube, O_WRONLY | O_NONBLOCK);
            write(entreeTube, uneRequete.corps.msg, sizeof(uneRequete.corps.msg));
            close(entreeTube);
        }
    }
unlink(nomTube);
return 0;


}