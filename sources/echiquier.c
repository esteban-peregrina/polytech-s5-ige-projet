#include <stdbool.h>
#include <stdlib.h>

#include ".././headers/piece.h"
#include ".././headers/echiquier.h"

Case* creationCase(int couleur) {
    /*
    Renvoie l'adresse d'une case vide dont la couleur est specifiée.
    */

    Case* Case = NULL;
    Case = malloc(sizeof(Case));
    if (Case == NULL) { exit(EXIT_FAILURE); }
    
    Case->couleur = couleur;
    Case->estSelectionne = false;
    Case->estAtteignable = false;

    Case->caseAtteignableSuivante = NULL;
    Case->caseAtteignablePrecedente = NULL;

    return Case;
}

void destructionCase(Case* Case) { // Nécessaire ?
    /*
    Libère la case pointée.  
    */

    if (Case == NULL) { exit(EXIT_FAILURE); } // On ne devrait pas passer de Case vide à cette fonction
    free(Case);
}

void insertionListeCasesAtteignables(Case* caseAtteignable, ListeCasesAtteignables* listeCasesAtteignables) {
    /*
    Insère caseAtteignable en tete de listeCasesAtteignables
    */

    Case* temp = listeCasesAtteignables->tete;
    if (temp != NULL) { temp->caseAtteignablePrecedente == NULL; }
    listeCasesAtteignables->tete = caseAtteignable;
    caseAtteignable->caseAtteignableSuivante = temp;
    caseAtteignable->caseAtteignablePrecedente = NULL;
}

void initialisePlateau(Case* Plateau[8][8]) {
    /*
    Rempli le plateau de cases vides aux couleurs alternées.
    */

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (i%2 == 0) {
                Plateau[i][j] = (j%2 == 0) ? creationCase(BLANC) : creationCase(NOIR);
            } else {
                Plateau[i][j] = (j%2 == 0) ? creationCase(NOIR) : creationCase(BLANC);
            }
        }
    }
}

void videPlateau(Case* Plateau[8][8]) {
    /*
    Rempli le plateau de cases vides aux couleurs alternées.
    */

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            destructionCase(Plateau[i][j]);
            Plateau[i][j] = NULL;
        }
    }
}

void affichePlateau(Case* Plateau[8][8]) {
    /*
    Affiche le plateau.
    */

    const int CELL_WIDTH = 8;  
    const int CELL_HEIGHT = 3;

    printf("Plateau :\n");

    // Pour chaque ligne de l'écosystème
    for (int i = 0; i < 8; i++) {
        // Ligne de séparation supérieure
        for (int j = 0; j < 8; j++) {
            printf("+");
            for (int k = 0; k < CELL_WIDTH; k++) {
                printf("-");
            }
        }
        printf("+\n");

        for (int row = 0; row < CELL_HEIGHT; row++) { // 3 lignes par case
            for (int j = 0; j < 8; j++) {
                printf("|");
                Case* caseCourante = Plateau[i][j];
                Piece* piecePresente = caseCourante->piece;

                // Afficher la ligne de la cellule
                if (caseCourante->couleur == BLANC) { // La case courante est blanche
                    //TODO - Activer en bleu (ou contour blanc et pièce noire --> Non galère, en fait comme on fait ligne par ligne faut uniquement changer les couleurs des pièces)
                } else { // La case courante est noire
                    //TODO - Activer en rouge 
                }
                if (piecePresente->role != VIDE) { printf("%-*s", CELL_WIDTH, piecePresente->forme); }
            }
            printf("|\n");
        }
    }

    // Dernière ligne de séparation inférieure
    for (int j = 0; j < 8; j++) {
        printf("+");
        for (int k = 0; k < CELL_WIDTH; k++) {
            printf("-");
        }
    }
    printf("+\n");
}

void partieEchec() {
    /*
    Démarre une partie d'échec.
    */
    
    // TODO - Proposer de charger une sauvegarde
    // TODO - Proposer de jouer contre un joueur ou contre une IA

    Case Plateau[8][8]; // Déclaration du Plateau
    initialisePlateau(Plateau); // Initialisation du plateau (chaque case est vide, non selectionnée ni atteignable, et de la bonne couleur)
    Piece* Joueurs[2];
    initialiseJoueur(Joueurs[2]);; // Déclaration des pièces des 2 joueurs, le roi est stocké comme tete de liste circulaire dans le tableau, index 0 pour Noir et 1 pour Blanc
    bool enEchec = false;
    int joueur = NOIR;

    while (!enEchec) {
        affichePlateau(Plateau); // TODO - Renommer car affiche aussi le menu (différent selon si piece ou coups)
        bool aJoue = false;
        Menu menu = PIECES;
        joueur == NOIR ? BLANC : NOIR; // On commute de joueur
        Piece* pieceCourante = Joueurs[joueur]; // On commence par les blancs (comme par hasard)
        
        while (!aJoue) {
            printf("Sélectionnez une pièce à déplacer, puis validez avec Entrée");
            char actionJoueur = getchar();

            if (actionJoueur == '\033') {
                Case* caseCourante = pieceCourante->casesAtteignables->tete;

                for (int i = 0; i < 2; i++) { actionJoueur = getchar(); }

                if (menu != COUPS && actionJoueur == 'B') { // La pièce est sélectionnée
                    caseCourante->estSelectionne = true;
                    menu = COUPS;
                } else if (menu != PIECES && actionJoueur == 'A') { // On retourne au choix des pièces
                    caseCourante->estSelectionne = false;
                    menu = PIECES;
                } else if (actionJoueur == 'D' || actionJoueur == 'C') { // Si c'est <- ou ->
                    if (menu == PIECES) {
                        if (actionJoueur == 'D') { // <-
                            pieceCourante = pieceCourante->piecePrecedente;
                        } else { // ->
                            pieceCourante = pieceCourante->pieceSuivante;
                        }
                        actualiseCasesAtteignables(pieceCourante, Plateau);
                        affichePlateau(Plateau);

                    } else { // menu == COUPS
                        if (actionJoueur == 'D') { // <-
                            caseCourante->estSelectionne = false;
                            caseCourante = caseCourante->caseAtteignablePrecedente;
                            caseCourante->estSelectionne = true;
                        } else { // ->
                            caseCourante->estSelectionne = false;
                            pieceCourante = caseCourante->caseAtteignableSuivante;
                            caseCourante->estSelectionne = true;
                        }
                        affichePlateau(Plateau);
                    }
                }
                
            } else if (menu == COUPS && actionJoueur == '\n') { // Le coup est validé
                // TODO - Mettre à jour les coordonnées de la pièce
                // TODO - Détruire la pièce ennemie si il y en a une (suppression de la liste des pièces de l'adversaire) ON SUPPOSE QUE LES CASES ATTEIGNABLES SONT VALIDES

                aJoue = true;
            }
        }
    }

}