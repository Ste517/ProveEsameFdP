#ifndef COMPITO_H
#define COMPITO_H

#include <iostream>
using namespace std;

class CampoMinato {
    int** campo_di_gioco;
    enum {GAME_NOT_STARTED,GAME_STARTED,GAME_WON,GAME_OVER} game_status;
    int dimensione;
    int bomb_amount;
    int cells_unveiled;
    bool coordinate_scorrette(int righe, int colonne) const;
    int conta_adiacenti(int righe, int colonne) const;
public:
    CampoMinato(int n);
    CampoMinato(const CampoMinato& cm);
    bool aggiungi_mina(int righe, int colonne);
    void scopri(int righe, int colonne);
    friend ostream& operator<<(ostream& os, CampoMinato& cm);
    CampoMinato operator+(const CampoMinato& cm) const;
    ~CampoMinato();
};

#endif // COMPITO_H
