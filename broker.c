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
pid_t pids[NB_CLIENTS];
char message[BUFFER_WRITE_SIZE];
char bufR[BUFFER_READ_SIZE];


int main(int argc, char *argv[])
{

int balId;
struct msqid_ds buf;
t_requete uneRequete;
char nomTube[50] = "tube";
#define MAX_CLIENTS 10
key_t laClef;
CHECK(laClef =ftok("broker",PROJECTID),"ftok");
/* Creation de la boite aux lettres */

CHECK(balId = msgget(laClef,IPC_CREAT | 0666 ),"msgget");
pid_t pids[MAX_CLIENTS]={0};



do
{
        /* Lecture de tous les messages de type = 1 */
        /* msgflg = 0 : nous sommes en attente bloquante de messages de type 1 */
        CHECK(msgrcv(balId,&uneRequete,sizeof(t_corps),1,0),"msgrcv");
        for (int i = 0; i < MAX_CLIENTS; i++) {
                if (pids[i] == 0) { 
                        pids[i] = uneRequete.corps.pid;
                        break;
                } 
        }
        for (int i = 0; i < MAX_CLIENTS; i++) {
                if (pids[i] != 0) {
                        sprintf(nomTube, "fifo_%d", pids[i]);
                        int entreeTube = open(nomTube, O_WRONLY);
                        write(entreeTube, uneRequete.corps.msg, sizeof(uneRequete.corps.msg));
                        close(entreeTube);
                }
        }


        
        
        

 } while (strncmp(uneRequete.corps.msg,"EXIT",4) !=0);


/* Destruction de la boite aux lettres */
CHECK(msgctl(balId,IPC_RMID,&buf),"msgctl");


return 0;
}