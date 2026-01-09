#include "compito.h"
#include <cstring>

CasaDiCura::CasaDiCura(const char nome[], int numeroOspiti) {
    if (strlen(nome) > 20) {
        strcpy(this->nome, nome);
    }
}
