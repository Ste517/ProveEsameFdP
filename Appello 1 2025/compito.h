#include <iostream>
using namespace std;

struct cell {
    char valore;
    cell* ptr = nullptr; // Puntatore che punta al prossimo carattere presente nella lista
};

class ListaCaratteri {
    cell* testa;
    void inserisciTesta(char carattere);
    void inserisciCoda(char carattere);
    bool rimuoviCarattere(char carattere);
    bool rimuoviTutti(char carattere);

public:
    ListaCaratteri();
    void inserisci(char carattere, bool fine);
    bool rimuovi(char carattere, bool tutti);
    friend ostream& operator<<(ostream& os, ListaCaratteri lista);
    void operator~();
    bool operator==(ListaCaratteri lista);
    bool controllaPalindroma();
};