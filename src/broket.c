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
/* bal3_s.c  : serveur                                                           */
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
    printf("Nombre maximum de bytes dans la file (msg_qbytes) %ld \n",buf.msg_qbytes);

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

key_t laClef;
CHECK(laClef =ftok("bal3_s.c",PROJECTID),"ftok");
/* Creation de la boite aux lettres */

CHECK(balId = msgget(laClef,IPC_CREAT | IPC_EXCL | 0666 ),"msgget");


/* Lecture des informations sur la boite aux lettres */
printf("%s : Informations sur la boite aux lettres créée \n",argv[0]);
CHECK(infoQueue(balId),"infoQueue");


do
{
        /* Lecture de tous les messages de type = 1 */
        /* msgflg = 0 : nous sommes en attente bloquante de messages de type 1 */
        CHECK(msgrcv(balId,&uneRequete,sizeof(t_corps),1,0),"msgrcv");

        printf("%s : Réception d'un message :\n",argv[0]);
        printf("%s : Type du message %ld\n",argv[0],uneRequete.type);
        printf("%s : pid de l'emetteur %ld\n",argv[0],(long)uneRequete.corps.pid);
        printf("%s : texte du message %s\n",argv[0],uneRequete.corps.msg);    
        
        /* Lecture des informations sur la boite aux lettres */
        printf("%s : Informations sur la boite après le msgrcv :  \n",argv[0]);
        CHECK(infoQueue(balId),"infoQueue");
 } while (strncmp(uneRequete.corps.msg,"EXIT",4) !=0);
 

/* Lecture des informations sur la boite aux lettres */
printf("%s : Informations sur la boite après le msgsnd \n",argv[0]);
CHECK(infoQueue(balId),"infoQueue");

/* Destruction de la boite aux lettres */
CHECK(msgctl(balId,IPC_RMID,&buf),"msgctl");


return 0;
}
