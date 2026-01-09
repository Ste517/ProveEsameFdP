#ifndef APPELLO_1_2026_COMPITO_H
#define APPELLO_1_2026_COMPITO_H
#include <iostream>
using namespace std;

struct Farmaco {
    char nome[16];
    unsigned short int orario_assunzione;
    bool assunto;
};

class CasaDiCura {
    char nome[21];
    int nOspiti;
public:
    CasaDiCura(const char nome[], int numeroOspiti);
    friend ostream& operator<<(ostream& os, CasaDiCura cc);
};


#endif //APPELLO_1_2026_COMPITO_H