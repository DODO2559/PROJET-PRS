typedef struct corps {
        pid_t pid_expediteur;
        pid_t pid_destinataire;
        char msg[256];
        int choix_menu;
        int groupe_id;
} t_corps;

typedef struct requete {
       long type;
        t_corps corps;
} t_requete;

typedef struct Client Client;

struct Client
{
    pid_t pidClient;
    Client *suivant;
};

typedef struct ListeClient ListeClient;

struct ListeClient
{
    Client *premier;
};
