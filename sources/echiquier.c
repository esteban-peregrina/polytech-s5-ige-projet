#include <stdbool.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

#include ".././headers/lecture.h"
#include ".././headers/piece.h"
#include ".././headers/echiquier.h"

Case* creationCase(int couleur) {
    /*
    Renvoie l'adresse d'une case vide dont la couleur est specifiée.
    */

    Case* caseCree = NULL;
    caseCree = malloc(sizeof(Case));
    if (caseCree == NULL) { exit(EXIT_FAILURE); }
    
    caseCree->couleur = couleur;
    caseCree->estSelectionne = false;
    caseCree->estAtteignable = false;

    caseCree->caseAtteignableSuivante = NULL;
    caseCree->caseAtteignablePrecedente = NULL;

    return caseCree;
}

void destructionCase(Case* Case) { // Nécessaire ?
    /*
    Libère la case pointée.  
    */

    if (Case == NULL) { exit(EXIT_FAILURE); } // On ne devrait pas passer de Case vide à cette fonction
    free(Case);
}

ListeCasesAtteignables* creationListeCasesAtteignables() {
    /*
    Renvoie l'adresse de la ListeCasesAtteignable créée.
    */

    ListeCasesAtteignables* nouvelleListe = NULL;
    nouvelleListe = malloc(sizeof(ListeCasesAtteignables));
    if (nouvelleListe == NULL) { exit(EXIT_FAILURE); }

    return nouvelleListe;
}

void destructionListeCasesAtteignables(ListeCasesAtteignables* listeObsolete) {
    /*
    Libère la ListeCasesAtteignables pointée.  
    */

    if (listeObsolete == NULL) { exit(EXIT_FAILURE); } // On ne devrait pas passer de ListeCasesAtteignables vide à cette fonction
    free(listeObsolete);
}

void insertionListeCasesAtteignables(Case* caseAtteignable, ListeCasesAtteignables* listeCasesAtteignables) {
    /*
    Insère caseAtteignable en tete de listeCasesAtteignables
    */

    Case* temp = listeCasesAtteignables->tete;
    if (temp != NULL) { temp->caseAtteignablePrecedente = caseAtteignable; }
    listeCasesAtteignables->tete = caseAtteignable;
    caseAtteignable->caseAtteignableSuivante = temp;
    caseAtteignable->caseAtteignablePrecedente = NULL;
}

void supressionListeCasesAtteignables(Case* caseRetirable, ListeCasesAtteignables* listeCasesAtteignables) {
    /*
    Supprrime caseRetirable de listeCasesAtteignables en mettant à jour les liens. Ne détruit pas caseRetirable !
    */

    if (caseRetirable == NULL) { exit(EXIT_FAILURE); }
    if (caseRetirable->caseAtteignablePrecedente == NULL) {
        if (caseRetirable->caseAtteignableSuivante == NULL) {
            listeCasesAtteignables->tete = NULL;
        } else {
            caseRetirable->caseAtteignableSuivante->caseAtteignablePrecedente = NULL;
        }
    } else {
        if (caseRetirable->caseAtteignableSuivante == NULL) {
            caseRetirable->caseAtteignablePrecedente->caseAtteignableSuivante = NULL;
        } else {
            caseRetirable->caseAtteignablePrecedente->caseAtteignableSuivante = caseRetirable->caseAtteignableSuivante;
            caseRetirable->caseAtteignableSuivante->caseAtteignablePrecedente = caseRetirable->caseAtteignablePrecedente;
        }
    }
}

void initialiseEchiquier(Case* Echiquier[8][8]) {
    /*
    Rempli l'echiquier de cases vides aux couleurs alternées.
    */

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (i%2 == 0) {
                Echiquier[i][j] = (j%2 == 0) ? creationCase(BLANC) : creationCase(NOIR);
            } else {
                Echiquier[i][j] = (j%2 == 0) ? creationCase(NOIR) : creationCase(BLANC);
            }
        }
    }
}

void videEchiquier(Case* Echiquier[8][8]) {
    /*
    Vide l'echiquier.
    */

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            destructionCase(Echiquier[i][j]);
            Echiquier[i][j] = NULL;
        }
    }
}

void afficheEchiquier(Case* Echiquier[8][8]) {
    /*
    Affiche l'echiquier.
    */

    const int CELL_HEIGHT = 3; // Impair pour avoir un milieu
    const int CELL_WIDTH = CELL_HEIGHT * 2 + 1; // Le ratio largeur:hauteur ASCII est 1:2 mais j'ai besoin d'un centre
    
    printf("\033[H\033[J"); // Vide le terminal
    printf("Echiquier :\n");

    for (int l = 0; l < 8 * CELL_WIDTH + 4; l++) {printf("\033[46m ");} // Bordure supérieure
    printf("\033[0m \n"); // Réinitialise la couleur par défaut et saute la ligne

    for (int i = 0; i < 8; i++) {
        for (int row = 0; row < CELL_HEIGHT; row++) { // Chaque ligne de la case affichée
            printf("\033[46m  "); // Bordure gauche
            for (int j = 0; j < 8; j++) {
                // Pour chaque case réelle de l'echiquier
                Case* caseCourante = Echiquier[i][j];
                Piece* contenuCase = caseCourante->piece;
                for (int column = 0; column < CELL_WIDTH; column++) {
                    if (caseCourante->estAtteignable == true) {
                        if (contenuCase != NULL) { // Une piece adverse est atteignable
                            printf("\033[0;41m"); // On active le fond rouge
                        } else {
                            printf("\033[0;42m"); // On active le fond vert
                        }
                        
                    } else if (caseCourante->estSelectionne == true) { //! - ATTENTION ELSE IF
                        printf("\033[0;44m"); // On active le fond bleu
                    } else {
                        caseCourante->couleur == BLANC ? printf("\033[47m") : printf("\033[40m"); // On active le fond de la bonne couleur neutre
                    }

                    if ( (row == CELL_HEIGHT / 2) && (column == CELL_WIDTH / 2) && (contenuCase != NULL) ) { // Au centre de la case affichée
                        contenuCase->couleur == BLANC ? printf("\033[34m%s\033[0m", contenuCase->forme) : printf("\033[31m%s\033[0m", contenuCase->forme); // TODO - Affiche la piece de la bonne couleur
                    } else {
                        printf(" ");
                    }
                }
                printf("\033[0m"); // Réinitialise la couleur
            }
            printf("\033[46m  \033[0m\n"); // Bordure droite
        }
    }

    for (int l = 0; l < 8 * CELL_WIDTH + 4; l++) {printf("\033[46m ");} // Bordure inférieure
    printf("\033[0m\n"); // Réinitialise la couleur par défaut et saute la ligne
}

void partieEchec() {
    /*
    Démarre une partie d'échec.
    */
    
    // TODO - Proposer de charger une sauvegarde
    // TODO - Proposer de jouer contre un joueur ou contre une IA

    Case* Echiquier[8][8]; // Déclaration du Echiquier
    initialiseEchiquier(Echiquier); // Initialisation de l'echiquier (chaque case est vide, non selectionnée ni atteignable, et de la bonne couleur)
    Piece* Joueurs[2];
    initialiseJoueur(Joueurs, Echiquier); // Déclaration des pièces des 2 joueurs, le roi est stocké comme tete de liste circulaire dans le tableau, index 0 pour Noir et 1 pour Blanc
    bool enEchec = false;
    int joueur = NOIR;

    while (!enEchec) {
        afficheEchiquier(Echiquier);
        bool aJoue = false;
        Menu menu = PIECES;
        joueur = (joueur == NOIR) ? BLANC : NOIR; // On commute de joueur
        Piece* pieceCourante = Joueurs[joueur]; // On commence par les blancs (comme par hasard)
        Case* caseCourante = pieceCourante->casesAtteignables->tete; //! - Ne verifie pas si cette liste est nulle

        while (!aJoue) {
            // Sauvegarder les paramètres originaux du terminal
            struct termios orig_termios;
            tcgetattr(STDIN_FILENO, &orig_termios);

            // Configurer le terminal en mode "raw"
            set_terminal_raw_mode();

            printf("Sélectionnez une pièce à jouer à l'aide des touches directionnelles (appuyez sur 'q' pour quitter)\n");

            char actionJoueur;
            if (read(STDIN_FILENO, &actionJoueur, 1) != 1) { exit(EXIT_FAILURE); } // Écrit l'entrée utilisateur lue dans &actionJoueur et vérifie que cela à fonctionné
            
            if (menu == PIECES) {
                if (actionJoueur == 'b') { // La pièce est sélectionnée
                    caseCourante = pieceCourante->casesAtteignables->tete;  //! - Ne verifie pas si cette liste est nulle
                    caseCourante->estSelectionne = true;
                    menu = COUPS;
                } else if (actionJoueur == 'd') { // <-
                    pieceCourante = pieceCourante->piecePrecedente; 
                } else if (actionJoueur == 'c') { // ->
                    pieceCourante = pieceCourante->pieceSuivante;
                }
                actualiseCasesAtteignables(pieceCourante->piecePrecedente, pieceCourante, Echiquier, Joueurs);
                afficheEchiquier(Echiquier);
                
            } else { //menu == COUPS
                if (menu != PIECES && actionJoueur == 'a') { // On retourne au choix des pièces
                    caseCourante->estSelectionne = false;
                    menu = PIECES;
                } else if (actionJoueur == 'd') { // <-
                    caseCourante->estSelectionne = false;
                    caseCourante = caseCourante->caseAtteignablePrecedente;
                    caseCourante->estSelectionne = true;
                } else if (actionJoueur == 'c') { // ->
                    caseCourante->estSelectionne = false;
                    caseCourante = caseCourante->caseAtteignableSuivante;
                    caseCourante->estSelectionne = true;
                } else if (actionJoueur == '\n') { // Le coup est validé
                    // TODO - Mettre à jour les coordonnées de la pièce
                    // TODO - Détruire la pièce ennemie si il y en a une (suppression de la liste des pièces de l'adversaire) ON SUPPOSE QUE LES CASES ATTEIGNABLES SONT VALIDES

                    aJoue = true;
                    afficheEchiquier(Echiquier);
                }
            }

            // Rétablir les paramètres originaux
            reset_terminal_mode(&orig_termios);
        }
    }
}