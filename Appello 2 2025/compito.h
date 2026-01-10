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
};

class UfficioPostale {
    unsigned int nSportelli;
    Sportello* sportelli;
public:
    UfficioPostale(int M); // Costruttore che prende in input il numero di sportelli
    UfficioPostale();
    friend ostream& operator<<(ostream& os, const UfficioPostale& up);
    void accodaUtente(const char nome[], int numeroSportello);
    void serviUtente(int numeroSportello);
    ~UfficioPostale();
    void accodaPrioritario(const char nome[]);
    void passaAvanti(char nome[], long long int nSportello, int nPosizioni); // uso long long int consapevole che unsigned long long int tenga il doppio dei numeri, ma altrimenti la sanitizzazione sarebbe impossibile
    UfficioPostale& operator!();
};

#endif // COMPITO_H
