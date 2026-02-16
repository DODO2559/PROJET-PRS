typedef struct corps {
        pid_t pid;
        char msg[256];
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

