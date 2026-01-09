#ifndef APPELLO_1_2026_COMPITO_H
#define APPELLO_1_2026_COMPITO_H
#include <iostream>
using namespace std;

struct Farmaco {
    char nome[16];
    unsigned short int orario_assunzione;
    bool assunto;
};

struct Ospite {
    unsigned int id;
    unsigned int nFarmaci;
    Farmaco* farmaci;
};

class CasaDiCura {
    char nome[21]{};
    int nOspiti;
    Ospite* ospiti;
public:
    CasaDiCura(const char nome[], int numeroOspiti);
    CasaDiCura(const CasaDiCura&);
    CasaDiCura& operator=(const CasaDiCura&);
    friend ostream& operator<<(ostream& os, const CasaDiCura& cc);
    bool aggiungiFarmaco(int idOspite, char nome[], int orario);
    bool assumiFarmaci(int idOspite, int orario);
    ~CasaDiCura();
    CasaDiCura rimuoviFarmaco(int idOspite, const char nome[]);
    CasaDiCura& operator!();
};


#endif //APPELLO_1_2026_COMPITO_H