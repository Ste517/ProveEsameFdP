#include "compito.h"

#include <cstring>
#include <iostream>

using namespace std;

UfficioPostale::UfficioPostale(int M) {
    if (M < 2) {
        M = 2;
    }
    nSportelli = M;
    sportelli = new Sportello[nSportelli];
    for (int i = 0; i < nSportelli; i++) {
        sportelli[i].primoUtente = nullptr;
        sportelli[i].nUtenti = 0;
        sportelli[i].nUtentiPrioritari = 0;
    }
}

ostream &operator<<(ostream &os, const UfficioPostale &up) {
    unsigned long long int nUtenti = 0;
    unsigned long long int nUtentiPrioritari = 0;
    for (int i = 0; i < up.nSportelli; i++) {
        nUtenti += up.sportelli[i].nUtenti;
        nUtentiPrioritari += up.sportelli[i].nUtentiPrioritari;
    }
    os << "Utenti totali: " << nUtenti << endl;
    os << "Prioritari: " << nUtentiPrioritari << endl;
    for (int i = 0; i < up.nSportelli; i++) {
        os << "- Sportello " << i+1 << ":";
        Utente* utente = up.sportelli[i].primoUtente;
        while (utente != nullptr) {
            os << " " << utente->nome;
            if (utente->prioritario) {
                os << "(P)";
            }
            if (utente->prossimoUtente != nullptr) {
                os << ",";
            }
            utente = utente->prossimoUtente;
        }
        os << endl;
    }
    return os;
}

void UfficioPostale::accodaUtente(const char nome[], int numeroSportello) {
    if (numeroSportello < 1 || numeroSportello > nSportelli) {
        return;
    }
    if (sportelli[numeroSportello-1].primoUtente == nullptr) {
        sportelli[numeroSportello-1].primoUtente = new Utente;
        sportelli[numeroSportello-1].primoUtente->prioritario = false;
        sportelli[numeroSportello-1].primoUtente->prossimoUtente = nullptr;
        if (strlen(nome) > 25) {
            strncpy(sportelli[numeroSportello-1].primoUtente->nome,nome, 25);
            sportelli[numeroSportello-1].primoUtente->nome[25] = '\0';
        } else {
            strcpy(sportelli[numeroSportello-1].primoUtente->nome,nome);
        }
        return;
    }
    Utente* utente = sportelli[numeroSportello-1].primoUtente;
    while (utente->prossimoUtente != nullptr) {
        if (strcmp(nome,utente->nome) == 0) return;
        utente = utente->prossimoUtente;
    }
    utente->prossimoUtente = new Utente;
    utente = utente->prossimoUtente;
    utente->prioritario = false;
    utente->prossimoUtente = nullptr;
    if (strlen(nome) > 25) {
        strncpy(utente->nome,nome, 25);
        utente->nome[25] = '\0';
    } else {
        strcpy(utente->nome,nome);
    }
    sportelli[numeroSportello-1].nUtenti++;
}

void UfficioPostale::serviUtente(int numeroSportello) {
    if (numeroSportello < 1 || numeroSportello > nSportelli) {
        return;
    }
    if (sportelli[numeroSportello-1].primoUtente == nullptr) {
        return;
    }
    Utente* user = sportelli[numeroSportello-1].primoUtente->prossimoUtente;
    delete sportelli[numeroSportello-1].primoUtente;
    sportelli[numeroSportello-1].primoUtente = user;
    sportelli[numeroSportello-1].nUtenti--;
}

UfficioPostale::~UfficioPostale() {
    Utente* utente;
    for (int i = 0; i < nSportelli; i++) {
        while (sportelli[i].primoUtente != nullptr) {
            utente = sportelli[i].primoUtente->prossimoUtente;
            delete sportelli[i].primoUtente;
            sportelli[i].primoUtente = utente;
        }
    }
    delete[] sportelli;
}

void UfficioPostale::accodaPrioritario(const char nome[]) {
    int numeroSportello = 0;
    int minPrioritari = sportelli[0].nUtentiPrioritari;
    for (int i = 0; i < nSportelli; i++) {
        if (minPrioritari > sportelli[i].nUtentiPrioritari) {
            minPrioritari = sportelli[i].nUtentiPrioritari;
        }
    }
    if (sportelli[numeroSportello].primoUtente == nullptr) {
        sportelli[numeroSportello].primoUtente = new Utente;
        sportelli[numeroSportello].primoUtente->prioritario = false;
        sportelli[numeroSportello].primoUtente->prossimoUtente = nullptr;
        if (strlen(nome) > 25) {
            strncpy(sportelli[numeroSportello].primoUtente->nome,nome, 25);
            sportelli[numeroSportello].primoUtente->nome[25] = '\0';
        } else {
            strcpy(sportelli[numeroSportello].primoUtente->nome,nome);
        }
        return;
    }
    Utente* utente = sportelli[numeroSportello].primoUtente;
    while (utente->prossimoUtente != nullptr) {
        if (strcmp(nome,utente->nome) == 0) return;
        utente = utente->prossimoUtente;
    }
    utente->prossimoUtente = new Utente;
    utente = utente->prossimoUtente;
    utente->prioritario = false;
    utente->prossimoUtente = nullptr;
    if (strlen(nome) > 25) {
        strncpy(utente->nome,nome, 25);
        utente->nome[25] = '\0';
    } else {
        strcpy(utente->nome,nome);
    }
    sportelli[numeroSportello].nUtenti++;
}






