#include "compito.h"
#include <iostream>

using namespace std;

// Bombs status: -2 (not unveiled), -1 (bomb), >0 (amount of bombs)

CampoMinato::CampoMinato(int n) {
    dimensione = n < 2 ? 10 : n;
    game_status = GAME_NOT_STARTED;
    bomb_amount = 0;
    campo_di_gioco = new int*[dimensione];
    for (int i = 0; i < dimensione; i++) {
        campo_di_gioco[i] = new int[dimensione];
        for (int j = 0; j < dimensione; j++) {
            campo_di_gioco[i][j] = -2;
        }
    }
}

CampoMinato::CampoMinato(const CampoMinato &cm) {
    dimensione = cm.dimensione;
    bomb_amount = cm.bomb_amount;
    cells_unveiled = 0;
    game_status = GAME_NOT_STARTED;
    campo_di_gioco = new int*[dimensione];
    for (int i = 0; i < dimensione; i++) {
        campo_di_gioco[i] = new int[dimensione];
        for (int j = 0; j < dimensione; j++) {
            campo_di_gioco[i][j] = cm.campo_di_gioco[i][j] == -1 ? -1 : -2;
        }
    }
}

bool CampoMinato::coordinate_scorrette(int righe, int colonne) const {
    return ((righe >= dimensione) || (righe < 0) || (colonne >= dimensione) || (colonne < 0));
}

int CampoMinato::conta_adiacenti(int righe, int colonne) const {
    int bomb_count = 0;
    for (int i = righe-1; i <= righe+1; i++) {
        for (int j = colonne-1; j <= colonne+1; j++) {
            if (coordinate_scorrette(i,j)) continue;
            if (campo_di_gioco[i][j] == -1) {
                bomb_count++;
            }
        }
    }
    return bomb_count;
}

bool CampoMinato::aggiungi_mina(int righe, int colonne) {
    if (game_status != GAME_NOT_STARTED) return false;
    if (coordinate_scorrette(righe,colonne)) return false;
    campo_di_gioco[righe][colonne] = -1;
    bomb_amount++;
    return true;
};

void CampoMinato::scopri(int righe, int colonne) {
    if (coordinate_scorrette(righe,colonne)) return;
    if (bomb_amount == 0 || game_status == GAME_OVER || game_status == GAME_WON) return;
    if (campo_di_gioco[righe][colonne] == -1) {
        game_status = GAME_OVER;
        return;
    }
    if (campo_di_gioco[righe][colonne] == -2) {
        cells_unveiled++;
        campo_di_gioco[righe][colonne] = conta_adiacenti(righe,colonne);
        if (campo_di_gioco[righe][colonne] == 0) {
            for (int i = righe-1; i <= righe+1; i++) {
                for (int j = colonne-1; j <= colonne+1; j++) {
                    if (coordinate_scorrette(i,j)) continue;
                    if (campo_di_gioco[i][j] == -2) {
                        scopri(i,j);
                    }
                }
            }
        }
    }
    if (cells_unveiled == ((dimensione*dimensione)-bomb_amount)) {
        game_status = GAME_WON;
    } else {
        game_status = GAME_STARTED;
    }
}

CampoMinato CampoMinato::operator+(const CampoMinato &cm) const {
    CampoMinato new_cm(dimensione+cm.dimensione);
    new_cm.bomb_amount = bomb_amount + cm.bomb_amount;
    for (int i = 0; i < dimensione; i++) {
        for (int j = 0; j < dimensione; j++) {
            new_cm.campo_di_gioco[i][j] = campo_di_gioco[i][j] == -1 ? -1 : -2;
        }
        for (int j = dimensione; j < new_cm.dimensione; j++) new_cm.campo_di_gioco[i][j] = -2;
    }
    for (int i = dimensione; i < new_cm.dimensione; i++) {
        for (int j = 0; j < dimensione; j++) new_cm.campo_di_gioco[i][j] = -2;
        for (int j = dimensione; j < new_cm.dimensione; j++) {
            new_cm.campo_di_gioco[i][j] = cm.campo_di_gioco[i-dimensione][j-dimensione] == -1 ? -1 : -2;
        }
    }
    return new_cm;
}

CampoMinato::~CampoMinato() {
    for (int i = 0; i < dimensione; i++) {
        if (campo_di_gioco[i]) delete[] campo_di_gioco[i];
    }
    if (campo_di_gioco) delete[] campo_di_gioco;
}

ostream &operator<<(ostream &os, CampoMinato &cm) {
    if (cm.game_status == CampoMinato::GAME_OVER) {
        os << "Game over\n";
        return os;
    }
    if (cm.game_status == CampoMinato::GAME_WON) {
        os << "Vittoria!\n";
        return os;
    }
    if (cm.game_status == CampoMinato::GAME_STARTED || cm.bomb_amount > 0) {
        os << "Campo Minato " << cm.dimensione << 'x' << cm.dimensione << " - Mine da trovare: " << cm.bomb_amount << endl;
        for (int i = 0; i < cm.dimensione; i++) {
            for (int j = 0; j < cm.dimensione; j++) {
                if (cm.campo_di_gioco[i][j] < 0) {
                    os << "X ";
                }
                if (cm.campo_di_gioco[i][j] == 0) {
                    os << "  ";
                }
                if (cm.campo_di_gioco[i][j] > 0) {
                    os << cm.campo_di_gioco[i][j] << ' ';
                }
            }
            os << endl;
        }
        return os;
    }
    if (cm.game_status == CampoMinato::GAME_NOT_STARTED) {
        os << "Inserire una mina per avviare il gioco\n";
        return os;
    }
}
