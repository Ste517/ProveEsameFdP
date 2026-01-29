#ifndef COMPITO_H
#define COMPITO_H

#include <iostream>
using namespace std;

class Biblioteca {

    struct Libro {
        char ISBN[14];
        char titolo[51];
        int anno_pubblicazione;
        bool stato_prestito;
    };

    static const Libro LibroVuoto;

    Libro* libri;
    int max_libri;

public:

    Biblioteca(int N);
    bool aggiungi(const char ISBN[], const char titolo[], int anno);
    friend ostream& operator<<(ostream& os, const Biblioteca& bb);
    int disponibili();
    bool prestito(const char ISBN[]);
    bool restituzione(const char ISBN[]);
    bool salvaFile(const char nomeFile[]);
    int caricaFile(const char nomeFile[]);
    ~Biblioteca();
    Biblioteca(const Biblioteca& bb);

};

#endif // COMPITO_H