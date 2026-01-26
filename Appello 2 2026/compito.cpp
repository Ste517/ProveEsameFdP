//
// Created by Stefano Brogi on 26/01/2026.
//

#include "compito.h"

TiltMaze::TiltMaze(int R, int C, int rSt, int cSt, int rEx, int cEx) {
    if (R < 3 || C < 3) {
        R = 3;
        C = 3;
    }

    // Imposto il limite inferiore
    rSt = rSt < 0 ? 0 : rSt;
    rEx = rEx < 0 ? 0 : rEx;
    cSt = cSt < 0 ? 0 : cSt;
    cEx = cEx < 0 ? 0 : cEx;

    // Imposto il limite superiore
    rSt = rSt > R-1 ? R-1 : rSt;
    rEx = rEx > R-1 ? R-1 : rEx;
    cSt = cSt > C-1 ? C-1 : cSt;
    cEx = cEx > C-1 ? C-1 : cEx;

    this->rSt = rSt;
    this->rEx = rEx;
    this->cSt = cSt;
    this->cEx = cEx;

    this->R = R;
    this->C = C;

    stato_gioco = MODIFICA;

    labirinto = new char*[R];
    for (int i = 0; i < R; i++) {
        labirinto[i] = new char[C];
        for (int j = 0; j < C; j++) {
            labirinto[i][j] = '.';
        }
    }

    labirinto[rEx][cEx] = 'E';
    labirinto[rSt][cSt] = 'S';

    rPalla = rSt;
    cPalla = cSt;
}

bool TiltMaze::aggiungiMuro(int r1, int c1, int r2, int c2) {
    // Controllo che il gioco sia nello stato MODIFICA
    if (stato_gioco != MODIFICA) return false;
    // Controllo che le coordinate siano nell'ordine giusto
    if (r2 < r1 || c2 < c1) return false;
    // Controllo che le coordinate non escano dall'area di gioco
    if (r1 < 0 || r1 >= R || r2 < 0 || r2 >= R) return false;
    if (c1 < 0 || c1 >= C || c2 < 0 || c2 >= C) return false;
    // Controllo se le coordinate sono messe in diagonale
    if (r1 != r2 && c1 != c2) return false;
    // Controllo che lungo il percorso non ci siano Start o Exit
    if (r1 == r2) {
        for (int j = c1; j <= c2; j++) {
            if ((labirinto[r1][j] == 'S') || (labirinto[r1][j] == 'E')) return false;
        }
    }
    if (c1 == c2) {
        for (int i = r1; i <= r2; i++) {
            if ((labirinto[i][c1] == 'S') || (labirinto[i][c1] == 'E')) return false;
        }
    }
    // Finiti i controlli, posiziono il muro
    if (r1 == r2) {
        for (int j = c1; j <= c2; j++) {
            labirinto[r1][j] = '#';
        }
    }
    if (c1 == c2) {
        for (int i = r1; i <= r2; i++) {
            labirinto[i][c1] = '#';
        }
    }
    return true;
}

bool TiltMaze::aggiungiBuca(int r, int c) {
    // Controllo che il gioco sia nello stato MODIFICA
    if (stato_gioco != MODIFICA) return false;
    // Controllo che le coordinate non escano dall'area di gioco
    if (r < 0 || r >= R) return false;
    if (c < 0 || c >= C) return false;
    // Controllo che la buca non si sovrapponga a Start o Exit
    if ((labirinto[r][c] == 'S') || (labirinto[r][c] == 'E')) return false;
    labirinto[r][c] = 'O';
    return true;
}


ostream &operator<<(ostream &os, const TiltMaze &t) {
    os << "Stato: ";
    switch (t.stato_gioco) {
        case MODIFICA:
            os << "MODIFICA";
            break;
        case AVVIATO:
            os << "AVVIATO";
            break;
        case VINTO:
            os << "VINTO";
            break;
        case PERSO:
            os << "PERSO";
            break;
    }
    os << endl;
    for (int i = t.R - 1; i >= 0; i--) {
        os << i << ' ';
        for (int j = 0; j < t.C; j++) {
            os << t.labirinto[i][j] << ' ';
        }
        os << endl;
    }
    os << "  ";
    for (int j = 0; j < t.C; j++) {
        os << j << ' ';
    }
    return os;
}

Stato TiltMaze::stato() const {
    return stato_gioco;
}

void TiltMaze::avvia() {
    if (cSt == cEx && rSt == rEx) {
        stato_gioco = VINTO;
        return;
    }
    stato_gioco = AVVIATO;
    rPalla = rSt;
    cPalla = cSt;
    labirinto[rPalla][cPalla] = 'x';
}

TiltMaze::~TiltMaze() {
    for (int i = 0; i < R; i++) {
        delete[] labirinto[i];
    }
    delete[] labirinto;
}

bool TiltMaze::controlloPalla(int currentR, int currentC) {
    if (labirinto[currentR][currentC] == 'O') {
        stato_gioco = PERSO;
        if (rPalla == rSt && cPalla == cSt) {
            labirinto[rPalla][cPalla] = 'S';
        } else {
            labirinto[rPalla][cPalla] = '.';
        }
        return true;
    }
    if (labirinto[currentR][currentC] == 'E') {
        labirinto[rPalla][cPalla] = '.';
        stato_gioco = VINTO;
        return true;
    }
    return false;
}


void TiltMaze::inclina(int dirV, int dirH) {
    // Controllo che il gioco sia avviato
    if (stato_gioco != AVVIATO) return;
    // Controllo che i valori inseriti siano corretti
    if (dirV == 0 && dirH == 0) return;
    if (dirV != 0 && dirH != 0) return;
    if (dirV != 1 && dirV != -1 && dirV != 0) return;
    if (dirH != 1 && dirH != -1 && dirH != 0) return;
    // Muovo la pallina
    if (dirH) {
        if (dirH > 0) {
            for (int j = cPalla; j < C; j++) {
                if (controlloPalla(rPalla, j)) return;
                if (j < C-1) {
                    if (labirinto[rPalla][j+1] == '#') {
                        if (rPalla == rSt && cPalla == cSt) {
                            labirinto[rPalla][cPalla] = 'S';
                        } else {
                            labirinto[rPalla][cPalla] = '.';
                        }
                        labirinto[rPalla][j] = 'x';
                        cPalla = j;
                        break;
                    }
                } else {
                    if (rPalla == rSt && cPalla == cSt) {
                        labirinto[rPalla][cPalla] = 'S';
                    } else {
                        labirinto[rPalla][cPalla] = '.';
                    }
                    labirinto[rPalla][j] = 'x';
                    cPalla = j;
                }
            }
        } else {
            for (int j = cPalla; j >= 0; j--) {
                if (controlloPalla(rPalla, j)) return;
                if (j > 0) {
                    if (labirinto[rPalla][j-1] == '#') {
                        if (rPalla == rSt && cPalla == cSt) {
                            labirinto[rPalla][cPalla] = 'S';
                        } else {
                            labirinto[rPalla][cPalla] = '.';
                        }
                        labirinto[rPalla][j] = 'x';
                        cPalla = j;
                        break;
                    }
                } else {
                    if (rPalla == rSt && cPalla == cSt) {
                        labirinto[rPalla][cPalla] = 'S';
                    } else {
                        labirinto[rPalla][cPalla] = '.';
                    }
                    labirinto[rPalla][j] = 'x';
                    cPalla = j;
                }
            }
        }
    } else {
        if (dirV > 0) {
            for (int i = rPalla; i < R; i++) {
                if (controlloPalla(i, cPalla)) return;
                if (i < R-1) {
                    if (labirinto[i+1][cPalla] == '#') {
                        if (rPalla == rSt && cPalla == cSt) {
                            labirinto[rPalla][cPalla] = 'S';
                        } else {
                            labirinto[rPalla][cPalla] = '.';
                        }
                        labirinto[i][cPalla] = 'x';
                        rPalla = i;
                        break;
                    }
                } else {
                    if (rPalla == rSt && cPalla == cSt) {
                        labirinto[rPalla][cPalla] = 'S';
                    } else {
                        labirinto[rPalla][cPalla] = '.';
                    }
                    labirinto[i][cPalla] = 'x';
                    rPalla = i;
                }
            }
        } else {
            for (int i = rPalla; i >= 0; i--) {
                if (controlloPalla(i, cPalla)) return;
                if (i > 0) {
                    if (labirinto[i-1][cPalla] == '#') {
                        if (rPalla == rSt && cPalla == cSt) {
                            labirinto[rPalla][cPalla] = 'S';
                        } else {
                            labirinto[rPalla][cPalla] = '.';
                        }
                        labirinto[i][cPalla] = 'x';
                        rPalla = i;
                        break;
                    }
                } else {
                    if (rPalla == rSt && cPalla == cSt) {
                        labirinto[rPalla][cPalla] = 'S';
                    } else {
                        labirinto[rPalla][cPalla] = '.';
                    }
                    labirinto[i][cPalla] = 'x';
                    rPalla = i;
                }
            }
        }
    }
}

TiltMaze::TiltMaze(const TiltMaze& t) {
    stato_gioco = MODIFICA;

    R = t.R;
    C = t.C;

    rSt = t.rSt;
    cSt = t.cSt;

    rEx = t.rEx;
    cEx = t.cEx;

    rPalla = rSt;
    cPalla = cSt;
    labirinto = new char*[R];
    for (int i = 0; i < R; i++) {
        labirinto[i] = new char[C];
        for (int j = 0; j < C; j++) {
            labirinto[i][j] = t.labirinto[i][j];
        }
    }
    labirinto[t.rPalla][t.cPalla] = '.';
    labirinto[rSt][cSt] = 'S';
}

TiltMaze TiltMaze::operator+(const TiltMaze &t) {
    if (R != t.R) return TiltMaze(*this);
    TiltMaze new_t(R,C+t.C,rSt,cSt,t.rEx,t.cEx+C);
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            new_t.labirinto[i][j] = labirinto[i][j];
        }
        for (int j = C; j < new_t.C; j++) {
            new_t.labirinto[i][j] = t.labirinto[i][j-C];
        }
    }
    new_t.labirinto[rPalla][cPalla] = '.';
    new_t.labirinto[t.rPalla][t.cPalla] = '.';
    new_t.labirinto[rEx][cEx] = '.';
    new_t.labirinto[t.rSt][t.cSt+C] = '.';
    new_t.labirinto[new_t.rSt][new_t.cSt] = 'S';
    new_t.labirinto[new_t.rEx][new_t.cEx] = 'E';
    return new_t;
}
// fine file