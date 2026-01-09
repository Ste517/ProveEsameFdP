#include "compito.h"
ListaCaratteri::ListaCaratteri() {
    testa = nullptr;
}

void ListaCaratteri::inserisciCoda(const char carattere) {
    cell* elem = new cell;
    elem->valore = carattere;
    if (testa == nullptr) {
        testa = elem;
        return;
    }
    cell* ptr = testa;
    while (ptr->ptr != nullptr) {
        ptr = ptr->ptr;
    }
    ptr->ptr = elem;
}

void ListaCaratteri::inserisciTesta(const char carattere) {
    cell* elem = new cell;
    elem->valore = carattere;
    cell* ptr = testa;
    elem->ptr = ptr;
    testa = elem;
}

void ListaCaratteri::inserisci(const char carattere, bool fine) {
    if (fine) {
        inserisciCoda(carattere);
    } else {
        inserisciTesta(carattere);
    }
}

bool ListaCaratteri::rimuoviCarattere(const char carattere) {
    if (testa == nullptr) {
        return false;
    }
    cell* elem = testa;
    cell* last = nullptr;
    do {
        if (testa->valore == carattere) {
            elem = testa->ptr;
            delete testa;
            testa = elem;
            return true;
        }
        if (elem->valore == carattere) {
            last->ptr = elem->ptr;
            delete elem;
            return true;
        }
        last = elem;
        elem = last->ptr;
    } while (last->ptr != nullptr);
    return false;
}

bool ListaCaratteri::rimuoviTutti(const char carattere) {
    if (testa == nullptr) {
        return false;
    }
    bool returnValue = false;
    cell* elem = testa;
    cell* last = nullptr;
    do {
        if (testa->valore == carattere) {
            elem = testa->ptr;
            delete testa;
            testa = elem;
            returnValue = true;
        } else if (elem->valore == carattere) {
            last->ptr = elem->ptr;
            delete elem;
            elem = last->ptr;
            returnValue = true;
        }
        last = elem;
        elem = last->ptr;
    } while (last->ptr != nullptr);
    return returnValue;
}


bool ListaCaratteri::rimuovi(const char carattere, bool tutti) {
    if (tutti) {
        return rimuoviTutti(carattere);
    }
    return rimuoviCarattere(carattere);
}


ostream& operator<<(ostream &os, ListaCaratteri lista) {
    const cell* ptr = lista.testa;
    while (ptr != nullptr) {
        os << ptr->valore << " -> ";
        ptr = ptr->ptr;
    }
    os << "@\n";
    return os;
}

// Seconda parte

void ListaCaratteri::operator~() {
    cell* prev = nullptr;
    cell* current = testa;
    cell* next = nullptr;

    while (current != nullptr) {
        next = current->ptr;
        current->ptr = prev;
        prev = current;
        current = next;
    }
    testa = prev;
}

bool ListaCaratteri::operator==(ListaCaratteri lista) {
    bool isEqual = true;
    cell* ptr1 = testa;
    cell* ptr2 = lista.testa;
    while ((ptr1 != nullptr) && (ptr2 != nullptr)) {
        if (ptr1->valore != ptr2->valore) {
            return false;
        }
        ptr1 = ptr1->ptr;
        ptr2 = ptr2->ptr;
    }
    if ((ptr1 == nullptr) && (ptr2 == nullptr)) {
        return true;
    }
    return false;
}

bool ListaCaratteri::controllaPalindroma() {
    cell* ptr = testa;

    while (ptr)
}


