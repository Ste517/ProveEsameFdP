#include "compito.h"
#include <cstring>
#include <fstream>

const Biblioteca::Libro Biblioteca::LibroVuoto = {.ISBN = {'\0'}, .titolo = {'\0'}, .anno_pubblicazione = -1, .stato_prestito = true};

Biblioteca::Biblioteca(int N) {
    N = N < 1 ? 1 : N;
    libri = new Libro[N];
    max_libri = N;
    for (int i = 0; i < max_libri; i++) {
        libri[i] = LibroVuoto;
    }
}

bool Biblioteca::aggiungi(const char ISBN[], const char titolo[], int anno) {
    if (strlen(ISBN) != 13 || strlen(titolo) == 0 || strlen(titolo) > 50 || anno > 2026 || anno < 1900) return false;
    for (int i = 0; i < max_libri; i++) {
        if (strcmp(ISBN, libri[i].ISBN) == 0) return false;
        if (libri[i].ISBN[0] == '\0') {
            strcpy(libri[i].ISBN, ISBN);
            strcpy(libri[i].titolo, titolo);
            libri[i].anno_pubblicazione = anno;
            libri[i].stato_prestito = false;
            return true;
        }
    }
    return false;
}

int Biblioteca::disponibili() {
    int count = 0;
    for (int i = 0; i < max_libri; i++) {
        if (!libri[i].stato_prestito) count++;
    }
    return count;
}

bool Biblioteca::prestito(const char ISBN[])
{
    for (int i = 0; i < max_libri; i++) {
        if (strcmp(libri[i].ISBN,ISBN) == 0) {
            if (libri[i].stato_prestito) {
                return false;
            } else {
                libri[i].stato_prestito = true;
                return true;
            }
        }
    }
    return false;
}

bool Biblioteca::restituzione(const char ISBN[])
{
    for (int i = 0; i < max_libri; i++) {
        if (strcmp(libri[i].ISBN,ISBN) == 0) {
            if (libri[i].stato_prestito) {
                libri[i].stato_prestito = false;
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

ostream& operator<<(ostream& os, const Biblioteca& bb) {
    for (int i = 0; i < bb.max_libri; i++) {
        os << "LIBRO" << i+1 << ": <";
        if (bb.libri[i].anno_pubblicazione != -1) {
            os << bb.libri[i].ISBN << ",";
            os << bb.libri[i].titolo << ",";
            os << bb.libri[i].anno_pubblicazione << ",";
            if (bb.libri[i].stato_prestito) {
                os << "P";
            } else {
                os << "D";
            }
        }
        os << ">" << endl;
    }
    return os;
}

bool Biblioteca::salvaFile(const char nomeFile[]) {
    ofstream file;
    file.open(nomeFile);
    if (file.is_open()) {
        for (int i = 0; i < max_libri; i++) {
            if (libri[i].anno_pubblicazione != -1) {
                file << libri[i].ISBN << ' ' << libri[i].anno_pubblicazione << " " << libri[i].stato_prestito << " " << libri[i].titolo << endl;
            }
        }
        file.close();
        return true;
    } else {
        return false;
    }
}

int Biblioteca::caricaFile(const char nomeFile[]) {
    ifstream file;
    int ret_value=-1;
    file.open(nomeFile);
    if (file.is_open()) {
        delete[] libri;
        ret_value++;
        libri = new Libro[max_libri];
        for (int i = 0; i < max_libri; i++) {
            if (file.eof()) break;
            file >> libri[i].ISBN;
            file >> libri[i].anno_pubblicazione;
            file >> libri[i].stato_prestito;
            file.get();
            file.getline(libri[i].titolo, 50, '\n');
            ret_value++;
        }
        for (int i = ret_value-1; i < max_libri; i++) {
            libri[i] = LibroVuoto;
        }
    }
    return ret_value-1;
}

Biblioteca::~Biblioteca() {
    delete[] libri;
}

Biblioteca::Biblioteca(const Biblioteca &bb) {
    max_libri = bb.max_libri;
    libri = new Libro[max_libri];
    for (int i = 0; i < max_libri; i++) {
        strcpy(libri[i].titolo, bb.libri[i].titolo);
        strcpy(libri[i].ISBN, bb.libri[i].ISBN);
        libri[i].anno_pubblicazione = bb.libri[i].anno_pubblicazione;
        libri[i].stato_prestito = bb.libri[i].stato_prestito;
    }
}
