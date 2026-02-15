#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <string.h>
#include "message.h"
/* ============================================================================= */
/*                                                                               */
/* bal3_c.c    : client                                                          */
/*                                                                               */
/* ============================================================================= */
#define CHECK(sts, msg) if ((sts)==-1) {perror(msg); exit(-1);}
#define PROJECTID 1

int infoQueue(int queueId)
{


    struct msqid_ds buf;

    CHECK(msgctl(queueId,IPC_STAT,&buf),"msgctl");
    printf("Informations sur la boite aux lettres : %d\n",queueId);
    printf("Heure du dernier msgsnd : %s\n",ctime(&(buf.msg_stime)));
    printf("Heure du dernier msgrcv : %s\n",ctime(&(buf.msg_rtime)));
    printf("Heure de la dernière modif : %s\n",ctime(&(buf.msg_ctime)));
    printf("Nombre actuel d'octets dans la file (non standard) : %ld\n",buf.__msg_cbytes);
    printf("Nombre actuel de messages dans la file : %ld\n",buf.msg_qnum);
    printf("Nombre max de bytes dans la file (msg_qbytes) %ld \n",buf.msg_qbytes);
    printf("PID du dernier msgsnd : %d\n",buf.msg_lspid);
    printf("PID du dernier msgrcv : %d\n",buf.msg_lrpid);
    printf("UID effectif du propriétaire : %d\n",buf.msg_perm.uid);
    printf("GID effectif du propriétaire : %d\n",buf.msg_perm.gid);
    printf("UID effectif du créateur : %d\n",buf.msg_perm.cuid);
    printf("ID effectif du créateur : %d\n",buf.msg_perm.cgid);
    printf("Permissions : %d\n",buf.msg_perm.mode);

    return(buf.msg_qnum);
}
int main(int argc, char *argv[])
{
int balId;
struct msqid_ds buf;
t_requete uneRequete;
int i=0;
char leTexte[100];
key_t laClef;
CHECK(laClef =ftok("bal3_s.c",PROJECTID),"ftok");

/* Ouverture  de la boite aux lettres (déjà créée par le serveur )*/

CHECK(balId = msgget(laClef,0 ),"msgget");

/* Lecture des informations sur la boite aux lettres */
printf("%s : Informations sur la boite aux lettres créées \n",argv[0]);
CHECK(infoQueue(balId),"infoQueue");

/* Ecriture de dix messages dans la boite aux lettres */
/* Type de message = 1                             */
uneRequete.type=1;
uneRequete.corps.pid=getpid();
for (i=0;i < 10; i++)
{
        printf("\nSaisissez votre message : ");
        fgets(leTexte,sizeof(leTexte),stdin);
        if (strncmp(leTexte,"EXIT",4) == 0)
        {
            strcpy(uneRequete.corps.msg,"EXIT");
            CHECK(msgsnd(balId,&uneRequete,sizeof(t_corps),0),"msgsnd")
            break;
        }
        else {
            sprintf(uneRequete.corps.msg, "%s %ld - message N° %d  - %s", argv[0], (long) getpid(), i, leTexte);
            CHECK(msgsnd(balId,&uneRequete,sizeof(t_corps),0),"msgsnd");
        }
}

return 0;
}
