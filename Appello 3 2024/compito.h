#ifndef COMPITO_H
#define COMPITO_H
#include <iostream>
using namespace std;

class SpaceAsteroids {

    static int punteggio_record;
    int punteggio;

    char** campo_di_gioco; // Matrice di caratteri che rappresenta il campo di gioco
    int larghezza;
    int altezza;
    int energia_massima;
    int energia;

    int posizione_astronave; // Posizione astronave (a partire da 0)
    bool can_move; // Variabile utilizzata dalla funzione di shift per capire se l'astronave si puo' spostare

    void avanza_asteroidi();
    void riavvia_gioco();
public:

    SpaceAsteroids(int altezza, int larghezza, int energia_massima);
    ~SpaceAsteroids();

    bool colloca_asteroide(int col);
    void avanza();

    void operator<<=(int n);
    void operator>>=(int n);

    void operator|=(int n);

    friend ostream& operator<<(ostream& os, const SpaceAsteroids& sa);
};

#endif // COMPITO_H
