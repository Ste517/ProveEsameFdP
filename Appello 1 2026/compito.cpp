#include "compito.h"
#include <cstring>

void sort_farmaci(Farmaco* &lista_farmaci, int nFarmaci) {
    bool scambiato;
    unsigned short int temp;

    for (int i = 0; i < nFarmaci-1; i++) {
        scambiato = false;
        for (int j = 0; j < nFarmaci - i -1; j++) {
            if (lista_farmaci[j].orario_assunzione > lista_farmaci[j+1].orario_assunzione) {
                temp = lista_farmaci[j].orario_assunzione;
                lista_farmaci[j].orario_assunzione = lista_farmaci[j+1].orario_assunzione;
                lista_farmaci[j+1].orario_assunzione = temp;
                scambiato = true;
            }
        }
        if (!scambiato) break;
    }
}

CasaDiCura::CasaDiCura(const char nome[], const int numeroOspiti) {
    if (strlen(nome) > 20 || strlen(nome) == 0) {
        strcpy(this->nome, "Default name");
    } else {
        strcpy(this->nome, nome);
    }
    if (numeroOspiti < 0) {
        nOspiti = 0;
    } else {
        nOspiti = numeroOspiti;
    }
    ospiti = new Ospite[nOspiti];
    for (int i = 0; i < nOspiti; i++) {
        ospiti[i].farmaci = new Farmaco[0];
        ospiti[i].nFarmaci = 0;
        ospiti[i].id = i+1;
    }

}

CasaDiCura::CasaDiCura(const CasaDiCura &cc2) {
    strcpy(nome, cc2.nome);
    nOspiti = cc2.nOspiti;
    ospiti = new Ospite[nOspiti];
    for (int i = 0; i < nOspiti; i++) {
        ospiti[i].id = cc2.ospiti[i].id;
        ospiti[i].nFarmaci = cc2.ospiti[i].nFarmaci;
        ospiti[i].farmaci = new Farmaco[ospiti[i].nFarmaci];
        for (int j = 0; j < ospiti[i].nFarmaci; j++) {
            ospiti[i].farmaci[j] = cc2.ospiti[i].farmaci[j];
        }
    }
}


CasaDiCura& CasaDiCura::operator=(const CasaDiCura &cc2) {
    for (int i = 0; i < nOspiti; i++) {
        delete[] ospiti[i].farmaci;
    }
    delete[] ospiti;
    strcpy(nome, cc2.nome);
    nOspiti = cc2.nOspiti;
    ospiti = new Ospite[nOspiti];
    for (int i = 0; i < nOspiti; i++) {
        ospiti[i].id = cc2.ospiti[i].id;
        ospiti[i].nFarmaci = cc2.ospiti[i].nFarmaci;
        ospiti[i].farmaci = new Farmaco[ospiti[i].nFarmaci];
        for (int j = 0; j < ospiti[i].nFarmaci; j++) {
            ospiti[i].farmaci[j] = cc2.ospiti[i].farmaci[j];
        }
    }
    return *this;
}


ostream &operator<<(ostream &os, const CasaDiCura& cc) {
    os << "Casa di Cura '" << cc.nome << "'\n";
    for (int i = 0; i < cc.nOspiti; i++) {
        os << "- Ospite " << cc.ospiti[i].id << ":\n";
        sort_farmaci(cc.ospiti[i].farmaci, cc.ospiti[i].nFarmaci);
        for (int j = 0; j < cc.ospiti[i].nFarmaci; j++) {
            os << "  " << j+1 << ". '" << cc.ospiti[i].farmaci[j].nome << "'\n";
            if (cc.ospiti[i].farmaci[j].assunto) {
                os << "     Assunto\n";
            } else {
                os << "     Non assunto\n";
            }
            os << "     Ore " << cc.ospiti[i].farmaci[j].orario_assunzione << endl;
        }
    }
    return os;
}

bool CasaDiCura::aggiungiFarmaco(int idOspite, char nome[], int orario) {
    if (idOspite < 1 || idOspite > nOspiti) {
        return false;
    }
    if (strlen(nome) > 15) {
        return false;
    }
    if (orario < 0 || orario > 23) {
        return false;
    }
    for (int i = 0; i < ospiti[idOspite-1].nFarmaci; i++) {
        if (strcmp(ospiti[idOspite-1].farmaci[i].nome, nome) == 0) return false;
        if (ospiti[idOspite-1].farmaci[i].orario_assunzione == orario) return false;
    }
    Farmaco* farmaci = new Farmaco[ospiti[idOspite-1].nFarmaci+1];
    for (int i = 0; i < ospiti[idOspite-1].nFarmaci; i++) {
        farmaci[i] = ospiti[idOspite-1].farmaci[i];
    }
    delete[] ospiti[idOspite-1].farmaci;
    strcpy(farmaci[ospiti[idOspite-1].nFarmaci].nome, nome);
    farmaci[ospiti[idOspite-1].nFarmaci].orario_assunzione = orario;
    farmaci[ospiti[idOspite-1].nFarmaci].assunto = false;
    ospiti[idOspite-1].farmaci = farmaci;
    ospiti[idOspite-1].nFarmaci++;
    return true;
}

bool CasaDiCura::assumiFarmaci(int idOspite, int orario) {
    if (idOspite < 1 || idOspite > nOspiti) {
        return false;
    }
    if (orario < 0 || orario > 23) {
        return false;
    }
    for (int i = 0; i < ospiti[idOspite-1].nFarmaci; i++) {
        if (ospiti[idOspite-1].farmaci[i].orario_assunzione == orario) {
            if (ospiti[idOspite-1].farmaci[i].assunto) {
                return false;
            }
            ospiti[idOspite-1].farmaci[i].assunto = true;
            return true;
        }
    }
    return false;
}

CasaDiCura::~CasaDiCura() {
    for (int i = 0; i < nOspiti; i++) {
        delete[] ospiti[i].farmaci;
    }
    delete[] ospiti;
}

CasaDiCura &CasaDiCura::rimuoviFarmaco(int idOspite, const char nome[]) {
    if (idOspite < 1 || idOspite > nOspiti) {
        return *this;
    }
    if (strlen(nome) > 15) {
        return *this;
    }
    for (int i = 0; i < ospiti[idOspite-1].nFarmaci; i++) {
        if (strcmp(ospiti[idOspite-1].farmaci[i].nome, nome) == 0) {
            Farmaco* farmaci = new Farmaco[ospiti[idOspite-1].nFarmaci-1];
            for (int j = 0; j < i; j++) {
                farmaci[j] = ospiti[idOspite-1].farmaci[j];
            }
            for (int j = i+1; j < ospiti[idOspite-1].nFarmaci; j++) {
                farmaci[j-1] = ospiti[idOspite-1].farmaci[j];
            }
            delete[] ospiti[idOspite-1].farmaci;
            ospiti[idOspite-1].farmaci = farmaci;
            ospiti[idOspite-1].nFarmaci--;
            return *this;
        }
    }
    return *this;
}

CasaDiCura &CasaDiCura::operator!() {
    for (int i = 0; i < nOspiti; i++) {
        for (int j = 0; j < ospiti[i].nFarmaci; j++) {
            if (ospiti[i].farmaci[j].orario_assunzione <= 12) {
                ospiti[i].farmaci[j].assunto = !ospiti[i].farmaci[j].assunto;
            }
        }
    }
    return *this;
}
