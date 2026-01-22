#ifndef COMPITO_H
#define COMPITO_H

#include <iostream>
using namespace std;

struct Prodotto {
    float prezzoUnitario;
    unsigned int quantita;
    Prodotto* next;
    char* nome;
    ~Prodotto();
};

ostream& operator<<(ostream os, Prodotto* lista_prodotti);

struct Carrello {
    Prodotto* listaProdotti;
    unsigned long long int cliente; // Utilizzo ull perché a parità di memoria utilizzata ho più dati (a causa del padding)
    ~Carrello();
};

class Supermercato {
    Carrello* carrelli[5]{};
    Prodotto* listaProdotti;
public:
    Supermercato();
    void crea_prodotto(const char* nome_prodotto, float pu);
    friend ostream& operator<<(ostream& os, const Supermercato& supermercato);
    void esponi(const char* p, int quantita, float pu = 0.0);
    ~Supermercato();
    void operator+=(long long int idCliente);
    void metti_nel_carrello(long long int c, const char* p, int q);
    float acquista(long long int idCliente);
};

#endif // COMPITO_H
