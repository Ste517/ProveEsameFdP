#include "compito.h"

#include <cstring>
#include <iostream>

using namespace std;

UfficioPostale::UfficioPostale() {
    nSportelli = 2;
    sportelli = new Sportello[nSportelli];
    for (unsigned int i = 0; i < nSportelli; i++) {
        sportelli[i].primoUtente = nullptr;
        sportelli[i].nUtenti = 0;
        sportelli[i].nUtentiPrioritari = 0;
    }
}

UfficioPostale::UfficioPostale(int M) {
    if (M < 2) {
        M = 2;
    }
    nSportelli = M;
    sportelli = new Sportello[nSportelli];
    for (unsigned int i = 0; i < nSportelli; i++) {
        sportelli[i].primoUtente = nullptr;
        sportelli[i].nUtenti = 0;
        sportelli[i].nUtentiPrioritari = 0;
    }
}

ostream &operator<<(ostream &os, const UfficioPostale &up) {
    unsigned long long int nUtenti = 0;
    unsigned long long int nUtentiPrioritari = 0;
    for (unsigned int i = 0; i < up.nSportelli; i++) {
        nUtenti += up.sportelli[i].nUtenti;
        nUtentiPrioritari += up.sportelli[i].nUtentiPrioritari;
    }
    os << "Utenti totali: " << nUtenti << endl;
    os << "Prioritari: " << nUtentiPrioritari << endl;
    for (unsigned int i = 0; i < up.nSportelli; i++) {
        os << "- Sportello " << i+1 << ":";
        Utente* utente = up.sportelli[i].primoUtente;
        while (utente != nullptr) {
            os << " " << utente->nome;
            if (utente->prioritario) {
                os << " (P)";
            }
            if (utente->prossimoUtente != nullptr) {
                os << ",";
            }
            utente = utente->prossimoUtente;
        }
        if (i < up.nSportelli-1) {
            os << endl;
        }
    }
    return os;
}

void UfficioPostale::accodaUtente(const char nome[], int numeroSportello) {
    if (numeroSportello < 1 || (unsigned int)numeroSportello > nSportelli) {
        return;
    }
    if (strlen(nome) > 25 || strlen(nome) == 0) {
        return;
    }
    if (sportelli[numeroSportello-1].primoUtente == nullptr) {
        sportelli[numeroSportello-1].primoUtente = new Utente;
        sportelli[numeroSportello-1].primoUtente->prioritario = false;
        sportelli[numeroSportello-1].primoUtente->prossimoUtente = nullptr;
        strcpy(sportelli[numeroSportello-1].primoUtente->nome,nome);
        sportelli[numeroSportello-1].nUtenti++;
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
    strcpy(utente->nome,nome);
    sportelli[numeroSportello-1].nUtenti++;
}

void UfficioPostale::serviUtente(int numeroSportello) {
    if (numeroSportello < 1 || (unsigned int)numeroSportello > nSportelli) {
        return;
    }
    if (sportelli[numeroSportello-1].primoUtente == nullptr) {
        return;
    }
    Utente* user = sportelli[numeroSportello-1].primoUtente->prossimoUtente;
    if (sportelli[numeroSportello-1].primoUtente->prioritario) sportelli[numeroSportello-1].nUtentiPrioritari--;
    delete sportelli[numeroSportello-1].primoUtente;
    sportelli[numeroSportello-1].primoUtente = user;
    sportelli[numeroSportello-1].nUtenti--;
}

UfficioPostale::~UfficioPostale() {
    Utente* utente;
    for (unsigned int i = 0; i < nSportelli; i++) {
        while (sportelli[i].primoUtente != nullptr) {
            utente = sportelli[i].primoUtente->prossimoUtente;
            delete sportelli[i].primoUtente;
            sportelli[i].primoUtente = utente;
        }
    }
    delete[] sportelli;
}

void UfficioPostale::accodaPrioritario(const char nome[]) {
    if (strlen(nome) > 25) {
        return;
    }
    unsigned int numeroSportello = 0;
    unsigned long long int minPrioritari = sportelli[0].nUtentiPrioritari;
    for (unsigned int i = 1; i < nSportelli; i++) {
        if (minPrioritari > sportelli[i].nUtentiPrioritari) {
            minPrioritari = sportelli[i].nUtentiPrioritari;
            numeroSportello = i;
        }
    }
    if (sportelli[numeroSportello].primoUtente == nullptr) {
        sportelli[numeroSportello].primoUtente = new Utente;
        sportelli[numeroSportello].primoUtente->prioritario = true;
        sportelli[numeroSportello].primoUtente->prossimoUtente = nullptr;
        strcpy(sportelli[numeroSportello].primoUtente->nome,nome);
        sportelli[numeroSportello].nUtenti++;
        sportelli[numeroSportello].nUtentiPrioritari++;
        return;
    }
    Utente* utente = sportelli[numeroSportello].primoUtente;
    while (utente != nullptr) {
        if (strcmp(nome,utente->nome) == 0) return;
        utente = utente->prossimoUtente;
    }

    utente = sportelli[numeroSportello].primoUtente;

    Utente* nuovo_utente = new Utente;
    nuovo_utente->prioritario = true;
    strcpy(nuovo_utente->nome,nome);

    if (!(utente->prioritario)) {
        nuovo_utente->prossimoUtente = utente;
        sportelli[numeroSportello].primoUtente = nuovo_utente;
    } else {
        while (utente->prossimoUtente != nullptr && utente->prossimoUtente->prioritario) {
            utente = utente->prossimoUtente;
        }
        nuovo_utente->prossimoUtente = utente->prossimoUtente;
        utente->prossimoUtente = nuovo_utente;
    }
    sportelli[numeroSportello].nUtenti++;
    sportelli[numeroSportello].nUtentiPrioritari++;
}

void UfficioPostale::passaAvanti(const char nome[], long long int numeroSportello, int nPosizioni) {
    if (strlen(nome)>25) return;
    if (numeroSportello < 1 || numeroSportello > nSportelli) return;
    numeroSportello--;
    Utente* utente = sportelli[numeroSportello].primoUtente;
    unsigned long long int counter = 0;
    bool trovato = false;
    while (utente != nullptr) {
        if (strcmp(utente->nome, nome) == 0) {
            trovato = true;
        }
        utente = utente->prossimoUtente;
        counter++;
    }
    if (!trovato) return;
    if (counter-nPosizioni < 0) return;

    Utente* base_pointer = sportelli[numeroSportello].primoUtente;
    utente = sportelli[numeroSportello].primoUtente;
    for (unsigned long long int i = 0; i < counter-1; i++) {
        if (counter-nPosizioni==i+1) {
            base_pointer = utente;
        }
        utente = utente->prossimoUtente;
    }
    Utente* cella = utente->prossimoUtente;
    utente->prossimoUtente = utente->prossimoUtente->prossimoUtente;
    if (counter-nPosizioni == 0) {
        cella->prossimoUtente = sportelli[numeroSportello].primoUtente;
        sportelli[numeroSportello].primoUtente = cella;
        if (cella->prossimoUtente->prioritario) {
            cella->prioritario = true;
            sportelli[numeroSportello].nUtentiPrioritari++;
        }
        return;
    }
    cella->prossimoUtente = base_pointer->prossimoUtente;
    base_pointer->prossimoUtente = cella;
    if (cella->prossimoUtente->prioritario) {
        cella->prioritario = true;
    }
}

UfficioPostale& UfficioPostale::operator!() {
    for (unsigned int i = 0; i < nSportelli; i++) {
        Utente* current = sportelli[i].primoUtente;
        
        Utente* headPrio = nullptr;
        Utente* tailPrio = nullptr;
        Utente* headStd = nullptr;
        Utente* tailStd = nullptr;
        
        unsigned long long int countPrio = 0;

        while (current != nullptr) {
            Utente* next = current->prossimoUtente; 
            
            
            current->prioritario = !current->prioritario;
            current->prossimoUtente = nullptr; 

            if (current->prioritario) {
                
                if (headPrio == nullptr) {
                    headPrio = current;
                } else {
                    tailPrio->prossimoUtente = current;
                }
                tailPrio = current;
                countPrio++;
            } else {
                
                if (headStd == nullptr) {
                    headStd = current;
                } else {
                    tailStd->prossimoUtente = current;
                }
                tailStd = current;
            }
            
            current = next; 
        }

        
        if (headPrio != nullptr) {
            sportelli[i].primoUtente = headPrio;
            tailPrio->prossimoUtente = headStd; 
        } else {
            sportelli[i].primoUtente = headStd;
        }

        
        sportelli[i].nUtentiPrioritari = countPrio;
        
    }

    return *this;
}