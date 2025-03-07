#include <pthread.h> // pthread_create()
#include <time.h> // sleep()
#include <stdbool.h> // bool
#include <stdio.h> // printf()

#include ".././include/lecture.h" // deplacerCurseur()
#include ".././include/echiquier.h" // couleurJoueurCourant, doitArreterTimer

#include ".././include/timer.h"

int tempsImpartiBlancs = TEMPS_IMPARTI; 
int tempsImpartiNoirs = TEMPS_IMPARTI; 

pthread_mutex_t doitArreterTimer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t couleurJoueurCourant_mutex = PTHREAD_MUTEX_INITIALIZER;

void* threadTimer() {
    /*
    Gère le timer de la partie.
    */

    while (!doitArreterTimer) {
        sleep(1); // On attend une seconde

        pthread_mutex_lock(&couleurJoueurCourant_mutex);
        couleurJoueurCourant == 1 ? tempsImpartiBlancs-- : tempsImpartiNoirs--; // On décrémente le temps imparti du joueur courant
        pthread_mutex_unlock(&couleurJoueurCourant_mutex);

        if (tempsImpartiBlancs < 0 || tempsImpartiNoirs < 0) {
            pthread_mutex_lock(&doitArreterTimer_mutex);
            doitArreterTimer = true;
            pthread_mutex_unlock(&doitArreterTimer_mutex);
        } else {
            printf("\033[s"); // Enregistre la position du curseur

            deplacerCurseur(1, 1); // On se déplace en haut à gauche du terminal
            printf("\033[K");

            pthread_mutex_lock(&couleurJoueurCourant_mutex);
            int minutesNoirs = tempsImpartiNoirs / 60;
            int secondesNoirs = tempsImpartiNoirs % 60;
            int minutesBlancs = tempsImpartiBlancs / 60;
            int secondesBlancs = tempsImpartiBlancs % 60;

            printf("Temps restant : %02d:%02d | %02d:%02d", minutesNoirs, secondesNoirs, minutesBlancs, secondesBlancs);
            pthread_mutex_unlock(&couleurJoueurCourant_mutex);
        
            printf("\033[u"); // Restaure la position
            fflush(stdout);  // Ensure the output is printed immediately
        }

    }

    return NULL;
}