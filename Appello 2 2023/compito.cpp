#include "compito.h"
#include <iostream>
#include <cstring>

using namespace std;

Prodotto* cerca_prodotto(Prodotto* &listaProdotti, const char *nome_prodotto) {
    Prodotto* p = listaProdotti;
    while(p != nullptr && strcmp(nome_prodotto, p->nome) > 0)
        p = p->next;

    if(p == nullptr || strcmp(nome_prodotto, p->nome))
        return nullptr;

    return p;
}


// Prima parte

Supermercato::Supermercato() {
    for (int i = 0; i < 5; i++) {
        carrelli[i] = nullptr;
    }
    listaProdotti = nullptr;
}

Prodotto::~Prodotto() {
    delete[] nome;
}

Carrello::~Carrello() {
    Prodotto *ptr = listaProdotti, *q = nullptr;
    while (ptr != nullptr) {
        q = ptr;
        ptr = ptr->next;
        delete q;
    }
}

void Supermercato::crea_prodotto(const char *nome_prodotto, float pu) {
    if (pu <= 0) return;
    if (cerca_prodotto(listaProdotti,nome_prodotto)) return;

    Prodotto* newProduct = new Prodotto{pu, 0, nullptr};
    newProduct->nome = new char[strlen(nome_prodotto)+1];
    strcpy(newProduct->nome, nome_prodotto);

    if (listaProdotti == nullptr || strcmp(listaProdotti->nome, nome_prodotto) > 0) {
        newProduct->next = listaProdotti;
        listaProdotti = newProduct;
        return;
    }

    Prodotto* elem = listaProdotti;
    while (elem->next != nullptr && strcmp(elem->next->nome, nome_prodotto) < 0) {
        elem = elem->next;
    }
    newProduct->next = elem->next;
    elem->next = newProduct;
}

void Supermercato::esponi(const char *p, int quantita, float pu) {
    if (pu < 0) return;
    if (quantita < 0) return;
    Prodotto* pun = listaProdotti;
    while (pun != nullptr) {
        if (strcmp(p,pun->nome) == 0) {
            if (pu > 0) {
                pun->prezzoUnitario = pu;
            }
            pun->quantita += quantita;
            return;
        }
        pun = pun->next;
    }
}

ostream &operator<<(ostream &os, const Supermercato &supermercato) {
    os << "prodotti:\n";
    Prodotto* pun = supermercato.listaProdotti;
    while (pun != nullptr) {
        os << pun->nome << " " << pun->quantita << " " << pun->prezzoUnitario << endl;
        pun = pun->next;
    }
    os << endl << "clienti:\n";
    bool supermercatoVuoto = true;
    for (int i = 0; i < 5; i++) {
        if (supermercato.carrelli[i] != nullptr) {
            supermercatoVuoto = false;
            os << "[" << i+1 << "-" << supermercato.carrelli[i]->cliente << "]:";
            pun = supermercato.carrelli[i]->listaProdotti;
            while (pun != nullptr) {
                os << " " << pun->nome << " (" << pun->quantita << ") |";
                pun = pun->next;
            }
            os << endl;
        }
    }
    if (supermercatoVuoto) {
        os << "nessun cliente presente." << endl;
    }
    return os;
}

Supermercato::~Supermercato() {
    for (int i = 0; i < 5; i++) {
        if (carrelli[i] != nullptr) {
            delete carrelli[i];
        }
    }
    Prodotto* ptr = listaProdotti;
    while (ptr != nullptr) {
        Prodotto* temp = ptr;
        ptr = ptr->next;
        delete temp;
    }
}

// Seconda parte

void Supermercato::operator+=(long long int idCliente) {
    if (idCliente < 0) return;
    for (int i = 0; i < 5; i++) {
        if (carrelli[i] == nullptr) {
            carrelli[i] = new Carrello{nullptr, (unsigned long long int)idCliente};
            break;
        }
    }
}

void Supermercato::metti_nel_carrello(long long int c, const char *p, int q) {
    if (c < 0 || q <= 0) return;

    // Cerca il prodotto nel supermercato
    Prodotto* pun = listaProdotti;
    while (pun != nullptr) {
        if (strcmp(p, pun->nome) == 0) {
            break;
        }
        pun = pun->next;
    }
    if (pun == nullptr) return;

    // Trova il carrello del cliente
    for (int i = 0; i < 5; i++) {
        if (carrelli[i] == nullptr) continue;
        if (((unsigned int)c) == carrelli[i]->cliente) {

            // Quantità effettiva da aggiungere
            unsigned int quantita_da_aggiungere = (((unsigned int)q) < pun->quantita ? ((unsigned int)q) : pun->quantita);
            if (quantita_da_aggiungere == 0) return;

            pun->quantita -= quantita_da_aggiungere;

            Prodotto* temp = carrelli[i]->listaProdotti;

            while (temp != nullptr) {
                if (strcmp(p, temp->nome) == 0) {
                    // Prodotto già presente, aggiorna quantità
                    temp->quantita += quantita_da_aggiungere;
                    temp->prezzoUnitario = pun->prezzoUnitario;  // Aggiorna prezzo
                    return;
                }
                temp = temp->next;
            }

            Prodotto* nuovoProdotto = new Prodotto{
                pun->prezzoUnitario,
                quantita_da_aggiungere,
                nullptr
            };
            nuovoProdotto->nome = new char[strlen(p)+1];
            strcpy(nuovoProdotto->nome, p);

            // Inserimento ordinato nel carrello
            if (carrelli[i]->listaProdotti == nullptr ||
                strcmp(carrelli[i]->listaProdotti->nome, p) > 0) {
                // Inserimento in testa
                nuovoProdotto->next = carrelli[i]->listaProdotti;
                carrelli[i]->listaProdotti = nuovoProdotto;
            } else {
                Prodotto* temp = nullptr;
                // Inserimento in mezzo/coda
                temp = carrelli[i]->listaProdotti;
                while (temp->next != nullptr && strcmp(temp->next->nome, p) < 0) {
                    temp = temp->next;
                }
                nuovoProdotto->next = temp->next;
                temp->next = nuovoProdotto;
            }
            return;
        }
    }
}

float Supermercato::acquista(long long int idCliente) {
    if (idCliente < 0) return -1.0;
    for (int i = 0; i < 5; i++) {
        if (carrelli[i] == nullptr) continue;
        if (((unsigned long long int)idCliente) == carrelli[i]->cliente) {
            float somma = 0.0f;
            Prodotto* pun = carrelli[i]->listaProdotti, *q = nullptr;
            while (pun != nullptr) {
                somma += (float(pun->quantita)*(pun->prezzoUnitario));
                q = pun;
                pun = pun->next;
                delete q;
            }
            carrelli[i]->listaProdotti = nullptr;
            delete carrelli[i];
            carrelli[i] = nullptr;
            return somma;
        }
    }
    return -1.0;
}


