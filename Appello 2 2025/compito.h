#ifndef COMPITO_H
#define COMPITO_H

#include <iostream>
using namespace std;

struct Utente {
    char nome[26];
    bool prioritario;
    Utente* prossimoUtente;
};

struct Sportello {
    Utente* primoUtente;
    unsigned long long int nUtenti; // numero (quasi) illimitato di utenti, metto tipo ull per questa evenienza (variabile di utilita')
    unsigned long long int nUtentiPrioritari;
    unsigned int id;
};

class UfficioPostale {
    unsigned int nSportelli;
    Sportello* sportelli;
public:
    UfficioPostale(int M); // Costruttore che prende in input il numero di sportelli
    friend ostream& operator<<(ostream& os, const UfficioPostale& up);
    void accodaUtente(const char nome[], int numeroSportello);
    void serviUtente(int numeroSportello);
    ~UfficioPostale();
    void accodaPrioritario(const char nome[]);
};

#endif // COMPITO_H
