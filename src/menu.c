#include <ncurses.h>
#include <string.h>
#include <time.h>
/* ========================================================= */
/* Installation de la librairie ncurses + documentation      */
/* sudo apt install libncurses5-dev libncursesw5-dev         */
/* sudo apt install ncurses-doc                              */
/* Documentation :                                           */
/* https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/index.html */
/* ========================================================= */
#define NB_CHOICES 3

const char *choices[NB_CHOICES] = {
    "Option 1 : Dire bonjour",
    "Option 2 : Afficher l'heure",
    "Option 3 : Quitter"
};

void print_menu(int highlight) {
    clear();
    start_color();			/* Start color 			*/
    init_pair(1, COLOR_RED, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    for (int i = 0; i < NB_CHOICES; i++) {
        if (i == highlight)
            attron(A_REVERSE);   // Inverse vidéo pour surligner
        mvprintw(2 + i, 4, "%s", choices[i]);
        if (i == highlight)
            attroff(A_REVERSE);
    }
    attroff(COLOR_PAIR(1));
    refresh();
}

void wait_for_enter() {
    mvprintw(10, 4, "Appuyez sur ENTREE pour revenir au menu");
    refresh();
    while (getch() != '\n');
}
void affiche_heure(char *resultat) {
    time_t maintenant = time(NULL);
    struct tm *t = localtime(&maintenant);

    char buffer[100];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y il est %H:%M:%S", t);
    //printf("Date et heure : %s\n", buffer);
    strcpy(resultat, buffer);
}

int main() {
    int highlight = 0;
    int ch;
    char buffer[100];


    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    while (1) {
        print_menu(highlight);
        ch = getch();

        switch (ch) {
            case KEY_UP:
                highlight = (highlight == 0) ? NB_CHOICES - 1 : highlight - 1;
                break;

            case KEY_DOWN:
                highlight = (highlight == NB_CHOICES - 1) ? 0 : highlight + 1;
                break;

            case '\n':  // touche Enter
                clear();
                if (highlight == 0) {
                    mvprintw(4, 4, "Bonjour !");
                    wait_for_enter();
                }
                else if (highlight == 1) {
                    //mvprintw(4, 4, "Il est temps de coder !");
                    affiche_heure(buffer);
                    mvprintw(4, 4, "Nous sommes le %s, il est grand temps de coder !",buffer);
                    wait_for_enter();
                }
                else if (highlight == 2) {
                    endwin();
                    return 0;
                }
                break;
        }
    }

    endwin();
    return 0;
}
