#include "compito.h"
#include <cstring>

bool sort_farmaci(Farmaco* &lista_farmaci) {

}

CasaDiCura::CasaDiCura(const char nome[], int numeroOspiti) {
    if (strlen(nome) > 20 || strlen(nome) == 0) {
        strcpy(this->nome, "Default name");
    } else {
        strcpy(this->nome, nome);
    }
    nOspiti = numeroOspiti;
}

ostream &operator<<(ostream &os, CasaDiCura cc) {
    os << "Casa di Cura '" << cc.nome << "'\n";

}
