#include "compito.h"

int SpaceAsteroids::punteggio_record = 0;

void SpaceAsteroids::riavvia_gioco() {
    for (int i = 0; i < altezza; i++) {
        for (int j = 0; j < larghezza; j++) {
            campo_di_gioco[i][j] = ' ';
        }
    }
    campo_di_gioco[altezza-1][larghezza/2] = 'A';
    posizione_astronave = larghezza/2;
    punteggio = 0;
    energia = energia_massima;
    can_move = true;
}

SpaceAsteroids::SpaceAsteroids(int altezza, int larghezza, int energia_massima) {
    altezza = altezza < 3 ? 3 : altezza;
    altezza = altezza > 7 ? 7 : altezza;
    larghezza = larghezza < 3 ? 3 : larghezza;
    larghezza = larghezza > 9 ? 9 : larghezza;
    larghezza = larghezza & 1 ? larghezza : larghezza+1;
    energia_massima = energia_massima < 1 ? 5 : energia_massima;
    this->altezza = altezza;
    this->larghezza = larghezza;
    this->energia_massima = energia_massima;
    campo_di_gioco = new char*[altezza];
    for (int i = 0; i < altezza; i++) {
        campo_di_gioco[i] = new char[larghezza];
    }
    riavvia_gioco();
}

SpaceAsteroids::~SpaceAsteroids() {
    for (int i = 0; i < altezza; i++) {
        delete[] campo_di_gioco[i];
    }
    delete[] campo_di_gioco;
}

bool SpaceAsteroids::colloca_asteroide(int col) {
    if (col < 1 || col > larghezza) return false;
    if (campo_di_gioco[0][col-1] != ' ') return false;
    campo_di_gioco[0][col-1] = 'X';
    return true;
}

void SpaceAsteroids::avanza_asteroidi() {
    if (campo_di_gioco[altezza-2][posizione_astronave] == 'X') {
        riavvia_gioco();
        return;
    }
    for (int j = 0; j < larghezza; j++) {
        if (j != posizione_astronave) {
            campo_di_gioco[altezza-1][j] = campo_di_gioco[altezza-2][j];
        }
    }
    for (int i = altezza-2; i > 0; i--) {
        for (int j = 0; j < larghezza; j++) {
            campo_di_gioco[i][j] = campo_di_gioco[i-1][j];
        }
    }
    for (int j = 0; j < larghezza; j++) {
        campo_di_gioco[0][j] = ' ';
    }
}

void SpaceAsteroids::avanza() {
    can_move = true;
    for (int i = 0; i < altezza-2; i++) {
        for (int j = 0; j < larghezza; j++) {
            if (campo_di_gioco[i+1][j] <= '9' && campo_di_gioco[i+1][j] >= '0') {
                if (campo_di_gioco[i][j] == 'X') {
                    if (campo_di_gioco[i+1][j] == '0') {
                        campo_di_gioco[i+1][j] = ' ';
                    }
                    else campo_di_gioco[i+1][j] -= 1;
                }
                campo_di_gioco[i][j] = campo_di_gioco[i+1][j];
                campo_di_gioco[i+1][j] = ' ';
            }
        }
    }
    avanza_asteroidi();
}

void SpaceAsteroids::operator<<=(int n) {
    if (!can_move) return;
    if (n < 0) return operator>>=(-n);
    if (n == 0) return;
    int new_position = (posizione_astronave-n) < 0 ? 0 : (posizione_astronave-n);
    for (int j = posizione_astronave-1; j >= new_position; j--) {
        if (campo_di_gioco[altezza-1][j] != ' ') {
            riavvia_gioco();
            return;
        }
    }
    campo_di_gioco[altezza-1][posizione_astronave] = ' ';
    campo_di_gioco[altezza-1][new_position] = 'A';
    posizione_astronave = new_position;
    can_move = false;
}

void SpaceAsteroids::operator>>=(int n) {
    if (!can_move) return;
    if (n < 0) return operator<<=(-n);
    if (n == 0) return;
    int new_position = (posizione_astronave+n) > (larghezza-1) ? (larghezza-1) : (posizione_astronave+n);
    for (int j = posizione_astronave+1; j <= new_position; j++) {
        if (campo_di_gioco[altezza-1][j] != ' ') {
            riavvia_gioco();
            return;
        }
    }
    campo_di_gioco[altezza-1][posizione_astronave] = ' ';
    campo_di_gioco[altezza-1][new_position] = 'A';
    posizione_astronave = new_position;
    can_move = false;
}

void SpaceAsteroids::operator|=(int n) {
    n = n < 0 ? 0 : n;
    n = n > energia ? energia : n;
    n = n > 10 ? 10 : n; // Per limitazioni riguardanti la mia rappresentazione dei raggi laser come char da '0' a '9'
    energia -= n;
    if (n == 0) return;
    char raggio = '0' + n - 1; // Il raggio verra' rappresentato come un carattere da '0' (intensita' 1) a '9' (intensita' 10)
    if (campo_di_gioco[altezza-2][posizione_astronave] != ' ') {
        if (raggio == '0') raggio = ' ';
        else raggio -= 1;
    }
    campo_di_gioco[altezza-2][posizione_astronave] = raggio;
}

ostream &operator<<(ostream &os, const SpaceAsteroids &sa) {
    os << "Punteggio: " << sa.punteggio << endl;
    os << "Record: " << SpaceAsteroids::punteggio_record << endl;
    os << "Energia: " << sa.energia << endl;
    for (int j = 0; j < sa.larghezza; j++) {
        os << "_";
    }
    os << endl;
    for (int i = 0; i < sa.altezza-1; i++) {
        for (int j = 0; j < sa.larghezza; j++) {
            if (sa.campo_di_gioco[i][j] <= '9' && sa.campo_di_gioco[i][j] >= '0') os << '|';
            else os << sa.campo_di_gioco[i][j];
        }
        os << endl;
    }
    for (int j = 0; j < sa.larghezza; j++) {
        os << (sa.campo_di_gioco[sa.altezza-1][j] == ' ' ? '_' : sa.campo_di_gioco[sa.altezza-1][j]);
    }
    return os;
}
