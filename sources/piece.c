#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include ".././headers/echiquier.h"
#include ".././headers/piece.h"

Piece* creationPiece(Role role, int couleur) {
    /*
    Renvoie l'adresse de la Piece créée, initialisée avec le role et la couleur specifié.

    Remarque : La structure n'ayant pas de membres alloués dynamiquement, un simple appel de free() suffit à la détruire.
    */

    Piece* pieceCree = NULL;
    pieceCree = malloc(sizeof(Piece));
    if (pieceCree == NULL) { exit(EXIT_FAILURE); }

    pieceCree->role = role;

    if (role == ROI) {
        pieceCree->forme = "♚";
        pieceCree->calculAtteignable = calculAtteignableRoi;
    } else if (role == REINE) {
        pieceCree->forme = "♛";
        pieceCree->calculAtteignable = calculAtteignableReine;
    } else if (role == FOU) {
        pieceCree->forme = "♝";
        pieceCree->calculAtteignable = calculAtteignableFou;
    } else if (role == CAVALIER) {
        pieceCree->forme = "♞";
        pieceCree->calculAtteignable = calculAtteignableCavalier;
    } else if (role == TOUR) {
        pieceCree->forme = "♜";
        pieceCree->calculAtteignable = calculAtteignableTour;
    } else if (role == PION) {
        pieceCree->forme = "♟";
        pieceCree->calculAtteignable = calculAtteignablePion;
    }
    pieceCree->couleur = couleur;
    pieceCree->x = 0;
    pieceCree->y = 0;

    for (int i = 0; i < 64; i++) { pieceCree->casesAtteignables[i] = NULL; }

    pieceCree->estSelectionnee = false; 
    pieceCree->estCapturee = false; 

    return pieceCree;
}

bool CaseExiste(int x, int y){
    if(x<=8 && x>=1 && y<=8 && y >=1){                  
        return true;                          //Si les coordonées sont dans la limite du Echiquier alors la case existe
    }
    return false;
}

void calculAtteignablePion(Case* Echiquier[8][8], Piece* joueurAdverse[16], Piece* roiAllie, Piece* self) {
    //TODO - Promotion possible affichée avec une case jaune ou des caractres +/reine
    //TODO - Prise en passant (ajout d'un booleen "double pas" ?)
    int origine, droite, avant, bordAvant, bordDroite, bordGauche;
    switch (self->couleur) {
        case BLANC :
            origine = 1;
            droite = 1;
            avant = 1;
            bordAvant = 7;
            bordDroite = 7;
            bordGauche = 0;
            break;
        case NOIR :
            origine = 6;
            droite = -1;
            avant = -1;
            bordAvant = 0;
            bordDroite = 0;
            bordGauche = 7;
            break;
    }

    // Double pas
    if (self->x == origine) { // Le pion est sur sa rangée initiale (comme il ne peut jamais revenir en arrière il n'y a pas besoin d'un booléen pour vérifier qu'on joue son tout premier mouvement)
        if (Echiquier[self->x + 2 * avant][self->y]->piece == NULL) { // Il n'y a pas de piece devant
            //if (!exposeRoi(Echiquier, joueurAdverse, roiAllie, self, self->x + 1, self->y)) { // Si le mouvement n'expose pas le roi allié à la capture par le joueur adverse
                insertionCasesAtteignables(Echiquier[self->x + 2 * avant][self->y], self->casesAtteignables);
            //}
        }
    }
    // Pas simple
    if (self->x != bordAvant) { 
        // Avancer tout droit
        if (Echiquier[self->x + avant][self->y]->piece == NULL) { // Il n'y a pas de piece devant
            //if (!exposeRoi(Echiquier, joueurAdverse, roiAllie, self, self->x + 1, self->y)) { // Si le mouvement n'expose pas le roi allié à la capture par le joueur adverse
                insertionCasesAtteignables(Echiquier[self->x + avant][self->y], self->casesAtteignables);
            //}
        }
        // Prendre en diagonale droite
        if (self->y != bordDroite) {
            if ( (Echiquier[self->x + avant][self->y + droite]->piece != NULL) && (Echiquier[self->x + avant][self->y + droite]->piece->couleur != self->couleur) ) { // Il y a une piece ennemie en diagonale droite
                //if (!exposeRoi(Echiquier, joueurAdverse, roiAllie, self, self->x + 1, self->y)) { // Si le mouvement n'expose pas le roi allié à la capture par le joueur adverse
                    insertionCasesAtteignables(Echiquier[self->x + 1][self->y + 1], self->casesAtteignables);
                //}
            }
        }
        // Prendre en diagonale gauche
        if (self->y != bordGauche) {
            if ( (Echiquier[self->x + avant][self->y - droite]->piece != NULL) && (Echiquier[self->x + avant][self->y - droite]->piece->couleur != self->couleur) ) { // Il y a une piece ennemie en diagonale gauche
                //if (!exposeRoi(Echiquier, joueurAdverse, roiAllie, self, self->x + 1, self->y)) { // Si le mouvement n'expose pas le roi allié à la capture par le joueur adverse
                    insertionCasesAtteignables(Echiquier[self->x + avant][self->y - droite], self->casesAtteignables);
                //}
            }

        }
    }
}

void calculAtteignableTour(Case* Echiquier[8][8], Piece* joueurAdverse[16], Piece* roiAllie, Piece* self){
    // TODO - Le roque
    int avant, droite, limiteAvant, limiteArriere, limiteDroite, limiteGauche;
    switch (self->couleur) {
        case BLANC :
            avant = 1;
            droite = 1;
            limiteAvant = 8;
            limiteArriere = -1;
            limiteDroite = 8;
            limiteGauche = -1;
            break;
        case NOIR :
            avant = -1;
            droite = -1;
            limiteAvant = -1;
            limiteArriere = 8;
            limiteDroite = -1;
            limiteGauche = 8;
            break;
    }

    // Déplacement avant
    int xCourant = self->x + avant;
    while (xCourant != limiteAvant) { // La case est dans la limite avant
        if (Echiquier[xCourant][self->y]->piece != NULL) { // La case n'est pas vide
            if (Echiquier[xCourant][self->y]->piece->couleur != self->couleur) { // La case contient une pièce ennemie
                insertionCasesAtteignables(Echiquier[xCourant][self->y], self->casesAtteignables);
            }
            break; // On arrête la boucle à la rencontre d'une pièce
        } else { // La case est vide
            insertionCasesAtteignables(Echiquier[xCourant][self->y], self->casesAtteignables);
            xCourant += avant;
        }
    }
    // Déplacement arrière
    xCourant = self->x - avant;
    while (xCourant != limiteArriere) { // La case est dans la limite avant
        if (Echiquier[xCourant][self->y]->piece != NULL) { // La case n'est pas vide
            if (Echiquier[xCourant][self->y]->piece->couleur != self->couleur) { // La case contient une pièce ennemie
                insertionCasesAtteignables(Echiquier[xCourant][self->y], self->casesAtteignables);
            }
            break; // On arrête la boucle à la rencontre d'une pièce
        } else { // La case est vide
            insertionCasesAtteignables(Echiquier[xCourant][self->y], self->casesAtteignables);
            xCourant -= avant;
        }
    }
    // Déplacement droite
    int yCourant = self->y + droite;
    while (yCourant != limiteDroite) { // La case est dans la limite avant
        if (Echiquier[self->x][yCourant]->piece != NULL) { // La case n'est pas vide
            if (Echiquier[self->x][yCourant]->piece->couleur != self->couleur) { // La case contient une pièce ennemie
                insertionCasesAtteignables(Echiquier[self->x][yCourant], self->casesAtteignables);
            }
            break; // On arrête la boucle à la rencontre d'une pièce
        } else { // La case est vide
            insertionCasesAtteignables(Echiquier[self->x][self->y], self->casesAtteignables);
            yCourant += droite;
        }
    }
    // Déplacement gauche
    yCourant = self->y - droite;
    while (yCourant != limiteGauche) { // La case est dans la limite avant
        if (Echiquier[self->x][yCourant]->piece != NULL) { // La case n'est pas vide
            if (Echiquier[self->x][yCourant]->piece->couleur != self->couleur) { // La case contient une pièce ennemie
                insertionCasesAtteignables(Echiquier[self->x][yCourant], self->casesAtteignables);
            }
            break; // On arrête la boucle à la rencontre d'une pièce
        } else { // La case est vide
            insertionCasesAtteignables(Echiquier[self->x][yCourant], self->casesAtteignables);
            yCourant -= droite;
        }
    }
}

void calculAtteignableCavalier(Piece* self, Case* Echiquier[8][8], Piece* Joueurs[2]){
    int xPiece = self->x;
    int yPiece = self->y;
    int xEchiquier = xPiece-1;
    int yEchiquier = yPiece-1;

    if(CaseExiste(xPiece+2,yPiece+1) && Echiquier[xEchiquier+2][yEchiquier+1]->piece == NULL && !leaveRoi(self,xPiece+2,yPiece+1,Joueurs,Echiquier)){
        insertionListeCasesAtteignables(Echiquier[xEchiquier+2][yEchiquier+1], self->casesAtteignables);
    }
    if(CaseExiste(xPiece+2,yPiece-1) && Echiquier[xEchiquier+2][yEchiquier-1]->piece == NULL && !leaveRoi(self,xPiece+2,yPiece-1,Joueurs,Echiquier)){
        insertionListeCasesAtteignables(Echiquier[xEchiquier+2][yEchiquier-1], self->casesAtteignables);
    }
    if(CaseExiste(xPiece+1,yPiece+2) && Echiquier[xEchiquier+1][yEchiquier+2]->piece == NULL && !leaveRoi(self,xPiece+1,yPiece+2,Joueurs,Echiquier)){
        insertionListeCasesAtteignables(Echiquier[xEchiquier+1][yEchiquier+2], self->casesAtteignables);
    }
    if(CaseExiste(xPiece+1,yPiece-2) && Echiquier[xEchiquier+1][yEchiquier-2]->piece == NULL && !leaveRoi(self,xPiece+1,yPiece-2,Joueurs,Echiquier)){
        insertionListeCasesAtteignables(Echiquier[xEchiquier+1][yEchiquier-2], self->casesAtteignables);
    }
    if(CaseExiste(xPiece-1,yPiece-2) && Echiquier[xEchiquier-1][yEchiquier-2]->piece == NULL && !leaveRoi(self,xPiece-1,yPiece-2,Joueurs,Echiquier)){
        insertionListeCasesAtteignables(Echiquier[xEchiquier-1][yEchiquier-2], self->casesAtteignables);
    }
    if(CaseExiste(xPiece-1,yPiece+2) && Echiquier[xEchiquier-1][yEchiquier+2]->piece == NULL && !leaveRoi(self,xPiece-1,yPiece+2,Joueurs,Echiquier)){
        insertionListeCasesAtteignables(Echiquier[xEchiquier-1][yEchiquier+2], self->casesAtteignables);
    }
    if(CaseExiste(xPiece-2,yPiece-1) && Echiquier[xEchiquier-2][yEchiquier-1]->piece == NULL && !leaveRoi(self,xPiece-2,yPiece-1,Joueurs,Echiquier)){
        insertionListeCasesAtteignables(Echiquier[xEchiquier-2][yEchiquier-1], self->casesAtteignables);
    }
    if(CaseExiste(xPiece-2,yPiece+1) && Echiquier[xEchiquier-2][yEchiquier+1]->piece == NULL && !leaveRoi(self,xPiece-2,yPiece+1,Joueurs,Echiquier)){
        insertionListeCasesAtteignables(Echiquier[xEchiquier-2][yEchiquier+1], self->casesAtteignables);
    }
}

void calculAtteignableFou(Piece* self, Case* Echiquier[8][8], Piece* Joueurs[2]){
    int xPiece = self->x;
    int yPiece = self->y;
    int xEchiquier = xPiece-1;
    int yEchiquier = yPiece-1;

    // fou diagonale haut-droit (PDV blancs)

    while(CaseExiste(xPiece + 1, yPiece + 1) && Echiquier[xEchiquier+1][yEchiquier+1]->piece == NULL && !leaveRoi(self,xPiece+1,yPiece+1,Joueurs,Echiquier)){
        insertionListeCasesAtteignables(Echiquier[xEchiquier+1][yEchiquier+1], self->casesAtteignables);
        xPiece++;
        xEchiquier++;
        yPiece++;
        yEchiquier++;
    }
    if(CaseExiste(xPiece+1,yPiece+1) && Echiquier[xEchiquier+1][yEchiquier+1]->piece != NULL){
        if((Echiquier[xEchiquier+1][yEchiquier+1]->piece)->couleur != self->couleur){
            insertionListeCasesAtteignables(Echiquier[xEchiquier+1][yEchiquier+1], self->casesAtteignables);
        }
    }
    xPiece = self->x;
    xEchiquier = xPiece-1;
    yPiece = self->y;
    yEchiquier = yPiece-1;

    // fou diagonale haut-gauche(PDV blancs)

    while(CaseExiste(xPiece + 1, yPiece - 1) && Echiquier[xEchiquier+1][yEchiquier-1]->piece == NULL && !leaveRoi(self,xPiece+1,yPiece-1,Joueurs,Echiquier)){
        insertionListeCasesAtteignables(Echiquier[xEchiquier+1][yEchiquier-1], self->casesAtteignables);
        xPiece++;
        xEchiquier++;
        yPiece--;
        yEchiquier--;
    }
    if(CaseExiste(xPiece+1,yPiece-1) && Echiquier[xEchiquier+1][yEchiquier-1]->piece != NULL){
        if((Echiquier[xEchiquier+1][yEchiquier-1]->piece)->couleur != self->couleur){
            insertionListeCasesAtteignables(Echiquier[xEchiquier+1][yEchiquier-1], self->casesAtteignables);
        }
    }
    xPiece = self->x;
    xEchiquier = xPiece-1;
    yPiece = self->y;
    yEchiquier = yPiece-1;

    // fou diagonale bas-droit (PDV blancs)

    while(CaseExiste(xPiece - 1, yPiece + 1) && Echiquier[xEchiquier-1][yEchiquier+1]->piece == NULL && !leaveRoi(self,xPiece-1,yPiece+1,Joueurs,Echiquier)){
        insertionListeCasesAtteignables(Echiquier[xEchiquier-1][yEchiquier+1], self->casesAtteignables);
        xPiece--;
        xEchiquier--;
        yPiece++;
        yEchiquier++;
    }
    if(CaseExiste(xPiece-1,yPiece+1) && Echiquier[xEchiquier-1][yEchiquier+1]->piece != NULL){
        if((Echiquier[xEchiquier-1][yEchiquier+1]->piece)->couleur != self->couleur){
            insertionListeCasesAtteignables(Echiquier[xEchiquier-1][yEchiquier+1], self->casesAtteignables);
        }
    }
    xPiece = self->x;
    xEchiquier = xPiece-1;
    yPiece = self->y;
    yEchiquier = yPiece-1;

    // fou diagonale bas-gauche (PDV blancs)

    while(CaseExiste(xPiece - 1, yPiece - 1) && Echiquier[xEchiquier-1][yEchiquier-1]->piece == NULL && !leaveRoi(self,xPiece-1,yPiece-1,Joueurs,Echiquier)){
        insertionListeCasesAtteignables(Echiquier[xEchiquier-1][yEchiquier-1], self->casesAtteignables);
        xPiece--;
        xEchiquier--;
        yPiece--;
        yEchiquier--;
    }
    if(CaseExiste(xPiece-1,yPiece-1) && Echiquier[xEchiquier-1][yEchiquier-1]->piece == NULL){
        if((Echiquier[xEchiquier-1][yEchiquier-1]->piece)->couleur != self->couleur){
            insertionListeCasesAtteignables(Echiquier[xEchiquier-1][yEchiquier-1], self->casesAtteignables);
        }
    }
}

void calculAtteignableReine(Piece* self, Case* Echiquier[8][8], Piece* Joueurs[2]){
    calculAtteignableFou(self,Echiquier, Joueurs);
    calculAtteignableTour(self,Echiquier, Joueurs);
}

void calculAtteignableRoi(Piece* self, Case* Echiquier[8][8], Piece* Joueurs[2]){
    // TODO - Le roque
    int xPiece = self->x;
    int yPiece = self->y;
    int xEchiquier = xPiece-1;
    int yEchiquier = yPiece-1;
    if(CaseExiste(xPiece+1, yPiece) && !suisjeAtteignable(xPiece+1,yPiece, Echiquier)){                                           //Si la case existe et n'est pas atteignable par une autre piece
        if(Echiquier[xEchiquier+1][yEchiquier]->piece==NULL || (Echiquier[xEchiquier+1][yEchiquier]->piece)->couleur != self->couleur){       //Si la case est libre ou occupé par un ennemi
            insertionListeCasesAtteignables(Echiquier[xEchiquier+1][yEchiquier], self->casesAtteignables);                           //Alors on ajoute la case a la liste des cases atteignable
        }
    }
    if(CaseExiste(xPiece+1, yPiece-1) && !suisjeAtteignable(xPiece+1,yPiece-1, Echiquier)){                                       //Si la case existe et n'est pas atteignable par une autre piece
        if(Echiquier[xEchiquier+1][yEchiquier-1]->piece==NULL || (Echiquier[xEchiquier+1][yEchiquier-1]->piece)->couleur != self->couleur){   //Si la case est libre ou occupé par un ennemi
            insertionListeCasesAtteignables(Echiquier[xEchiquier+1][yEchiquier-1], self->casesAtteignables);                         //Alors on ajoute la case a la liste des cases atteignable
        }
    }
    if(CaseExiste(xPiece+1, yPiece+1) && !suisjeAtteignable(xPiece+1,yPiece+1, Echiquier)){                                       //Si la case existe et n'est pas atteignable par une autre piece
        if(Echiquier[xEchiquier+1][yEchiquier+1]->piece==NULL || (Echiquier[xEchiquier+1][yEchiquier+1]->piece)->couleur != self->couleur){   //Si la case est libre ou occupé par un ennemi
            insertionListeCasesAtteignables(Echiquier[xEchiquier+1][yEchiquier+1], self->casesAtteignables);                         //Alors on ajoute la case a la liste des cases atteignable
        }
    }
    if(CaseExiste(xPiece-1, yPiece) && !suisjeAtteignable(xPiece-1,yPiece, Echiquier)){                                           //Si la case existe et n'est pas atteignable par une autre piece
        if(Echiquier[xEchiquier-1][yEchiquier]->piece==NULL || (Echiquier[xEchiquier-1][yEchiquier]->piece)->couleur != self->couleur){       //Si la case est libre ou occupé par un ennemi
            insertionListeCasesAtteignables(Echiquier[xEchiquier-1][yEchiquier], self->casesAtteignables);                           //Alors on ajoute la case a la liste des cases atteignable
        }
    }
    if(CaseExiste(xPiece-1, yPiece+1) && !suisjeAtteignable(xPiece-1,yPiece+1, Echiquier)){                                       //Si la case existe et n'est pas atteignable par une autre piece
        if(Echiquier[xEchiquier-1][yEchiquier+1]->piece==NULL || (Echiquier[xEchiquier-1][yEchiquier+1]->piece)->couleur != self->couleur){   //Si la case est libre ou occupé par un ennemi
            insertionListeCasesAtteignables(Echiquier[xEchiquier-1][yEchiquier+1], self->casesAtteignables);                         //Alors on ajoute la case a la liste des cases atteignable
        }
    }
    if(CaseExiste(xPiece-1, yPiece-1) && !suisjeAtteignable(xPiece-1,yPiece-1, Echiquier)){                                       //Si la case existe et n'est pas atteignable par une autre piece
        if(Echiquier[xEchiquier-1][yEchiquier-1]->piece==NULL || (Echiquier[xEchiquier-1][yEchiquier-1]->piece)->couleur != self->couleur){   //Si la case est libre ou occupé par un ennemi
            insertionListeCasesAtteignables(Echiquier[xEchiquier-1][yEchiquier-1], self->casesAtteignables);                         //Alors on ajoute la case a la liste des cases atteignable
        }
    }
    if(CaseExiste(xPiece, yPiece+1) && !suisjeAtteignable(xPiece,yPiece+1, Echiquier)){                                           //Si la case existe et n'est pas atteignable par une autre piece
        if(Echiquier[xEchiquier][yEchiquier+1]->piece==NULL || (Echiquier[xEchiquier][yEchiquier+1]->piece)->couleur != self->couleur){       //Si la case est libre ou occupé par un ennemi
            insertionListeCasesAtteignables(Echiquier[xEchiquier][yEchiquier+1], self->casesAtteignables);                           //Alors on ajoute la case a la liste des cases atteignable
        }
    }
    if(CaseExiste(xPiece, yPiece-1) && !suisjeAtteignable(xPiece,yPiece-1, Echiquier)){                                           //Si la case existe et n'est pas atteignable par une autre piece
        if(Echiquier[xEchiquier][yEchiquier-1]->piece==NULL || (Echiquier[xEchiquier][yEchiquier-1]->piece)->couleur != self->couleur){       //Si la case est libre ou occupé par un ennemi
            insertionListeCasesAtteignables(Echiquier[xEchiquier][yEchiquier-1], self->casesAtteignables);                           //Alors on ajoute la case a la liste des cases atteignable
        }
    }
    
}

bool exposeRoi(Case* Echiquier[8][8], Piece* joueurAdverse[16], Piece* roiAllie, Piece* pieceCourante, int xSouhaite, int ySouhaite) {
    /*
    Vérifie si le mouvement de pieceCourante en (xSouhaite, ySouhaite) expose le Roi à l'echec.
    */
    //! - Invalide

    int xCourant = pieceCourante->x;
    int yCourant = pieceCourante->y;

    pieceCourante->x = xSouhaite;
    pieceCourante->y = ySouhaite;

    for (int i = 0; i < 16; i++) { actualiseCasesAtteignables(Echiquier, joueurAdverse[4], joueurAdverse[i]); } // On recalcul l'ensemble des tableaux de casesAtteignables de l'adversaire.
    bool res = Echiquier[roiAllie->x][roiAllie->y]->estAtteignable;

    pieceCourante->x = xCourant;
    pieceCourante->y = yCourant;

    for (int i = 0; i < 16; i++) { actualiseCasesAtteignables(Echiquier, joueurAdverse[4], joueurAdverse[i]); }
    return res;
}

void insertionCasesAtteignables(Piece* pieceCourante, Case* caseAtteignable) {
    /* 
    Insère caseAtteignbale dans le tableau de casesAtteignables de pieceCourante, après le dernier élément non-nul.
    */

    int i = 0;
    while ( (i < 64) && (pieceCourante->casesAtteignables[i] != NULL) ) { i++; }
    if (i >= 64) { exit(EXIT_FAILURE); } // Il y a plus de cases atteignables que de cases dans le plateau.
    pieceCourante->casesAtteignables[i] = caseAtteignable;
    caseAtteignable->estAtteignable = true;
}

void actualiseCasesAtteignables(Case* Echiquier[8][8], Piece* Roi, Piece* pieceActualisable) {
    /*
    Vide puis remplis le tableau des cases atteignables de pieceActualisable.
    */
    if (!(pieceActualisable->estCapturee)) { // N'actualise que les pièces encore en jeu.
        for (int i = 0; i < 64; i++) {
            if (pieceActualisable->casesAtteignables[i] == NULL) { break; } // On arrête le parcourt après le dernier élément non-nul du tableau
            pieceActualisable->casesAtteignables[i]->estAtteignable = false; 
            pieceActualisable->casesAtteignables[i] = NULL;
        }
        pieceActualisable->calculAtteignable(Echiquier, Roi, pieceActualisable); // TODO - màj les signatures
    }
}

void initialiseJoueur(Case* Echiquier[8][8], Piece* Joueur[16], int couleur) {
    /*
    Initialise les pièces de chacun des joueurs.
    */

    int rangeePromotion, rangeePions;
    switch(couleur) {
        case BLANC :
            rangeePromotion = 0;
            rangeePions = 1;
            break;
        case NOIR :
            rangeePromotion = 7;
            rangeePions = 6;
            break;
    }

    // Roi
    Echiquier[rangeePromotion][4]->piece = creationPiece(ROI, couleur);
    Echiquier[rangeePromotion][4]->piece->x = rangeePromotion;
    Echiquier[rangeePromotion][4]->piece->y = 4;
    Joueur[4] = Echiquier[rangeePromotion][4]->piece;

    // Reine
    Echiquier[rangeePromotion][3]->piece = creationPiece(REINE, couleur);
    Echiquier[rangeePromotion][3]->piece->x = rangeePromotion;
    Echiquier[rangeePromotion][3]->piece->y = 3;
    Joueur[3] = Echiquier[rangeePromotion][3]->piece;

    // Fous
    Echiquier[rangeePromotion][2]->piece = creationPiece(FOU, couleur);
    Echiquier[rangeePromotion][2]->piece->x = rangeePromotion;
    Echiquier[rangeePromotion][2]->piece->y = 2;
    Joueur[2] = Echiquier[rangeePromotion][2]->piece;

    Echiquier[rangeePromotion][5]->piece = creationPiece(FOU, couleur);
    Echiquier[rangeePromotion][5]->piece->x = rangeePromotion;
    Echiquier[rangeePromotion][5]->piece->y = 5;
    Joueur[5] = Echiquier[rangeePromotion][5]->piece;

    // Cavaliers
    Echiquier[rangeePromotion][1]->piece = creationPiece(CAVALIER, couleur);
    Echiquier[rangeePromotion][1]->piece->x = rangeePromotion;
    Echiquier[rangeePromotion][1]->piece->y = 1;
    Joueur[5] = Echiquier[rangeePromotion][1]->piece;

    Echiquier[rangeePromotion][6]->piece = creationPiece(CAVALIER, couleur);
    Echiquier[rangeePromotion][6]->piece->x = rangeePromotion;
    Echiquier[rangeePromotion][6]->piece->y = 6;
    Joueur[5] = Echiquier[rangeePromotion][6]->piece;

    // Tours
    Echiquier[rangeePromotion][0]->piece = creationPiece(TOUR, couleur);
    Echiquier[rangeePromotion][0]->piece->x = rangeePromotion;
    Echiquier[rangeePromotion][0]->piece->y = 0;
    Joueur[5] = Echiquier[rangeePromotion][0]->piece;

    Echiquier[rangeePromotion][7]->piece = creationPiece(TOUR, couleur);
    Echiquier[rangeePromotion][7]->piece->x = rangeePromotion;
    Echiquier[rangeePromotion][7]->piece->y = 7;
    Joueur[5] = Echiquier[rangeePromotion][7]->piece;

    // Pions
    for (int colonne = 0; colonne < 8; colonne++) {
        Echiquier[rangeePions][colonne]->piece = creationPiece(PION, couleur);
        Echiquier[rangeePions][colonne]->piece->x = rangeePions;
        Echiquier[rangeePions][colonne]->piece->y = colonne;
        Joueur[7 + colonne] = Echiquier[rangeePromotion][5]->piece;
    }
}
