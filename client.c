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
/* ============================================================================= */
/*                                                                               */
/* bal3_c.c    : client                                                          */
/*                                                                               */
/* ============================================================================= */
#define CHECK(sts, msg) if ((sts)==-1) {perror(msg); exit(-1);}
#define PROJECTID 1
#define TAILLE_MESSAGE 256
#define NB_CLIENTS 100
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

/* Ouverture  de la boite aux lettres (déjà créée par le serveur )*/

CHECK(balId = msgget(laClef,0 ),"msgget");



/* Type de message = 1 message privée
   Type de message = 2 message public       
   Type de message = 3 demande la liste des clients au broker               */


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

uneRequete.type = 1;
uneRequete.corps.pid_expediteur = getpid();
uneRequete.corps.pid_destinataire = 0; 
strcpy(uneRequete.corps.msg, "--- INIT ---"); // <--- AJOUTE CECI
msgsnd(balId, &uneRequete, sizeof(t_corps), 0);
//saisie des msgs
   while (1) {
        // type 3 donc demande au broker laliste
        uneRequete.type = 3;
        uneRequete.corps.pid_expediteur = getpid();
        msgsnd(balId, &uneRequete, sizeof(t_corps), 0);
        msgrcv(balId, &uneRequete, sizeof(t_corps), getpid(), 0);
        
        printf("\nClients connectes :\n");
        for (int i = 0; i < 10; i++) {
            if (uneRequete.corps.pids[i] != 0 && uneRequete.corps.pids[i] != getpid())
                printf("  - PID: %d\n", uneRequete.corps.pids[i]);
            pids[i] = uneRequete.corps.pids[i];
        }
        
        printf("\nEntrez le PID du destinataire : ");
        scanf("%d", &uneRequete.corps.pid_destinataire);
        getchar(); 
        printf("\n");
        
        //verif si destinaire est connecté 
        int destinataireConnecte = 0;
        for (int i = 0; i < NB_CLIENTS; i++) {
            if (pids[i] == uneRequete.corps.pid_destinataire) {
                destinataireConnecte = 1;
                break;
            }
        }
        if(destinataireConnecte==1){
            printf("\nSaisissez votre message : ");
            fgets(leTexte,sizeof(leTexte),stdin);
            uneRequete.type = 3;
            uneRequete.corps.pid_expediteur = getpid();
            if (strncmp(leTexte,"EXIT",4) == 0)
            {
                strcpy(uneRequete.corps.msg,"EXIT");
                CHECK(msgsnd(balId,&uneRequete,sizeof(t_corps),0),"msgsnd");
                break;
            }
            else {
                sprintf(uneRequete.corps.msg, "%s", leTexte);
                CHECK(msgsnd(balId,&uneRequete,sizeof(t_corps),0),"msgsnd");
            }
            printf("\n");
        }else {
            printf("Il n'y a pas de client avec ce PID dans la liste veuillez en saisir un valide \n");
            continue;
        }

}

unlink(nomTube);
return 0;


}
