#include <stdlib.h>
#include <stdbool.h>
#include ".././headers/echiquier.h"
#include ".././headers/piece.h"



void calculAtteignablePion(Piece* self, Case Plateau[8][8]){
    int xPiece = self->x;
    int yPiece = self->y;

    if (self->couleur == BLANC){                                                                              //Déplacements classique
        if(xPiece<8 && Plateau[xPiece+1][yPiece].piece==NULL){
            insertionListeCaseAtteignables(&Plateau[xPiece+1][yPiece], self->coupsAtteignables);
        }
        if(xPiece == 2){
            if(Plateau[xPiece+2][yPiece].piece==NULL){                                                        //Avance de deux s'il est sur la ligne de départ
                insertionListeCaseAtteignables(&Plateau[xPiece+2][yPiece], self->coupsAtteignables);
            }
        }
        if(xPiece<8 && yPiece<8 && Plateau[xPiece+1][yPiece+1].piece!=NULL){                                  //Possibilité de manger en diagonale
            insertionListeCaseAtteignables(&Plateau[xPiece+1][yPiece+1], self->coupsAtteignables);
        }
        if(xPiece<8 && yPiece>1 && Plateau[xPiece+1][yPiece-1].piece!=NULL){
            insertionListeCaseAtteignables(&Plateau[xPiece+1][yPiece-1], self->coupsAtteignables);
        }
    }else{
        if(xPiece >1 && Plateau[xPiece-1][yPiece].piece==NULL){
            insertionListeCaseAtteignables(&Plateau[xPiece-1][yPiece], self->coupsAtteignables);
        }
        if(xPiece == 7){
            if(Plateau[xPiece-2][yPiece].piece==NULL){
                insertionListeCaseAtteignables(&Plateau[xPiece-2][yPiece], self->coupsAtteignables);
            }
        }
        if(xPiece>1 && yPiece<8 && Plateau[xPiece-1][yPiece+1].piece!=NULL){
            insertionListeCaseAtteignables(&Plateau[xPiece-1][yPiece+1], self->coupsAtteignables);
        }
        if(xPiece>1 && yPiece>1 && Plateau[xPiece+1][yPiece-1].piece!=NULL){
            insertionListeCaseAtteignables(&Plateau[xPiece-1][yPiece-1], self->coupsAtteignables);
        }
    }
}

void calculAtteignableTour(Piece* self, Case Plateau[8][8]){
    int xPiece = self->x;
    int yPiece = self->y;
    while(xPiece>=1 && xPiece<=8 && yPiece>=1 && yPiece<=8 && Plateau[xPiece][yPiece].piece!=NULL ){

    }
}