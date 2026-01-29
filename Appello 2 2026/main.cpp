#include <iostream>
#include "compito.h"
using namespace std;

// Usare il seguente enumerato, da inserire nel vostro file compito.h
// enum Stato{MODIFICA, AVVIATO, VINTO, PERSO};

int main() {

    cout << endl << "--- PRIMA PARTE ---" << endl;
    cout << boolalpha; // serve a stampare "true"/"false" invece di 1/0
    // 4x4. Start(0,0), Exit(0,3).
    TiltMaze t(4, 4, 0, 0, 0, 3);
    cout << "Gioco in modifica? " << (t.stato() == MODIFICA) << endl;  // deve restituire "true"
    cout << t << endl;
    t.aggiungiMuro(0, 2, 1, 2);
    t.aggiungiMuro(3, 1, 3, 3);
    t.aggiungiBuca(3, 0);

    cout << t << endl; // Stato: MODIFICA

    t.avvia();
    cout << "Gioco avviato? " << (t.stato() == AVVIATO) << endl;  // deve restituire "true"
    cout << t;


    cout << endl << endl << "--- SECONDA PARTE ---" << endl;
    t.inclina(0, 1);
    cout << t << endl;
    t.inclina(1, 0);
    cout << t << endl;
    t.inclina(0, 1);
    cout << t << endl;
    t.inclina(-1, 0);

    cout << t << endl; // Stato: VINTO
    cout << "Gioco vinto? " << (t.stato() == VINTO) << endl; // deve restituire "true"

    t.avvia();  // restart
    t.inclina(1, 0);
    cout << t << endl; // Stato: PERSO

    TiltMaze t2(t);
    t2.aggiungiBuca(1, 3);
    TiltMaze vuoto(4, 6, 0, 0, 3, 5);

    TiltMaze t3(t2 + vuoto);
    t3.aggiungiBuca(0, 6);
    cout << t3 << endl;


    cout << endl << endl << "--- TERZA PARTE ---" << endl;
    // =============================================================
    // SCENARIO DI GIOCO (Happy Path & Logica Movimento)
    // =============================================================

    // 1. Inizializzazione: 5 Righe, 6 Colonne. Start(0,0), Exit(4,5)
    TiltMaze snakeMaze(5, 6, 0, 0, 4, 5);

    // 2. Configurazione Ostacoli "Snake"
    snakeMaze.aggiungiMuro(1, 1, 4, 1); // Muro Verticale (blocca colonna 1 tranne base)
    snakeMaze.aggiungiMuro(2, 3, 2, 5); // Muro Orizzontale (a metà destra)
    snakeMaze.aggiungiBuca(3, 0);       // Trappola sulla colonna 0

    cout << "--- Configurazione Iniziale (MODIFICA) ---" << endl;
    cout << snakeMaze << endl;

    // 3. Avvio
    snakeMaze.avvia();
    cout << "--> Gioco Avviato. Obiettivo: Raggiungere 'E' in alto a destra." << endl;

    // 4. Sequenza di VITTORIA
    cout << "\n[MOSSA 1] Inclino a DESTRA (0, 1)..." << endl;
    snakeMaze.inclina(0, 1); // Scorre base
    cout << snakeMaze;

    cout << "\n[MOSSA 2] Inclino in ALTO (1, 0)..." << endl;
    snakeMaze.inclina(1, 0); // Sale fino al muro orizzontale
    cout << snakeMaze;

    cout << "\n[MOSSA 3] Inclino a SINISTRA (0, -1)..." << endl;
    snakeMaze.inclina(0, -1); // Scorre a sinistra fino al muro verticale
    cout << snakeMaze;

    cout << "\n[MOSSA 4] Inclino in ALTO (1, 0)..." << endl;
    snakeMaze.inclina(1, 0); // Sale libero fino al soffitto
    cout << snakeMaze;

    cout << "\n[MOSSA 5] Inclino a DESTRA (0, 1) -> VITTORIA..." << endl;
    snakeMaze.inclina(0, 1); // Raggiunge l'uscita
    cout << snakeMaze; // Deve stampare Stato: VINTO

    // 5. Sequenza di SCONFITTA (Trappola)
    cout << "\n--- RESTART (Test Buca) ---" << endl;
    snakeMaze.avvia();
    cout << "Pallina riportata su Start (0,0)." << endl;

    cout << "[MOSSA SUICIDA] Inclino subito in ALTO (1, 0)..." << endl;
    snakeMaze.inclina(1, 0); // Finisce nella buca a (3,0)
    cout << snakeMaze; // Deve stampare Stato: PERSO


    // =============================================================
    // STRESS TEST, EDGE CASES E GESTIONE MEMORIA
    // =============================================================

    // Validazione costruttore
    cout << "\n--- VALIDAZIONE input costruttore" << endl;
    TiltMaze bad(-1, -1, 1, -1, 7, -4);
    cout << bad << endl;

    // Usiamo una nuova istanza per pulizia
    TiltMaze t1(5, 5, 0, 0, 0, 4);
    cout << "Creato Labirinto T1 (5x5) per stress test." << endl;
    cout << t1 << endl;

    cout << "\n--- VALIDAZIONE COORDINATE (Atteso: FAILURE/false) ---" << endl;
    cout << "Muro fuori scala (riga negativa): " << t1.aggiungiMuro(-1, 0, 0, 0) << endl;
    cout << "Muro fuori scala (colonna eccessiva): " << t1.aggiungiMuro(0, 0, 0, 10) << endl;
    cout << "Muro diagonale (0,0 -> 1,1): " << t1.aggiungiMuro(0, 0, 1, 1) << endl;
    cout << "Muro sopra lo Start: " << t1.aggiungiMuro(0, 0, 0, 1) << endl;
    cout << "Buca sopra l'Exit: " << t1.aggiungiBuca(0, 4) << endl;

    cout << "\n--- INSERIMENTI VALIDI (Atteso: SUCCESS/true) ---" << endl;
    cout << "Muro valido (2,2 -> 2,3): " << t1.aggiungiMuro(2, 2, 2, 3) << endl;
    cout << "Buca valida (1,1): " << t1.aggiungiBuca(1, 1) << endl;

    cout << "\n--- STATI E AZIONI ILLEGALI ---" << endl;
    t1.avvia();
    cout << "T1 Avviato." << endl;

    cout << "Aggiunta muro a gioco avviato: " << t1.aggiungiMuro(3, 0, 3, 3) << endl; // Deve fallire

    cout << "   [TEST] Inclinazione obliqua (1, 1)... ";
    t1.inclina(1, 1);
    cout << "Eseguita (Non deve aver mosso la pallina)" << endl;

    cout << "   [TEST] Inclinazione nulla (0, 0)... ";
    t1.inclina(0, 0);
    cout << "Eseguita (Non deve aver mosso la pallina)" << endl;

    // Controllo visivo che la pallina sia ancora ferma su Start
    // Nota: Se la logica è corretta, la 'x' deve essere ancora su (0,0)
    cout << t1 << endl;


    cout << "--- COPIA PROFONDA (DEEP COPY) e vittoria ---" << endl;
    // Spostiamo la pallina in T1 per differenziarlo
    t1.inclina(0, 1); // Destra

    cout << "Creazione T4 copia di T1..." << endl;
    TiltMaze t4(t1);

    cout << "   Verifica Stato T4 (Deve essere MODIFICA): " << (t4.stato() == MODIFICA ? "OK" : "ERRORE") << endl;

    cout << "Modifica T4: Aggiungo buca in (4,4)..." << endl;
    t4.aggiungiBuca(4, 4);

    cout << "VERIFICA INDIPENDENZA:" << endl;
    // Controllo brutale stampando: T1 non deve avere la buca O in (4,4)
    cout << "-> T1 (4,4) deve essere vuoto (.):" << endl;
    cout << t1 << endl; // Guardo visivamente
    cout << "-> T4 (4,4) deve avere buca (O):" << endl;
    cout << t4 << endl; // Guardo visivamente


    cout << "--- OPERATORE SOMMA (+) ---" << endl;

    // Caso 1: Dimensioni Diverse
    TiltMaze small(2, 2, 0, 0, 1, 1);
    cout << "Somma dimensioni diverse (T1 5 righe + Small 2 righe):" << endl;
    TiltMaze tError(t1 + small);
    cout << "   Risultato deve essere copia di T1 (5 righe): " << (tError.stato() == MODIFICA ? "OK" : "ERRORE") << endl;
    cout << tError << endl; // Opzionale

    // Caso 2: Dimensioni Uguali
    cout << "Somma dimensioni uguali (T1 + T4):" << endl;
    TiltMaze tSum(t1 + t4);

    cout << "   Dimensioni attese: 5 righe x 10 colonne." << endl;
    tSum.avvia(); // Test avvio su labirinto somma
    cout << "   Stato tSum avviato: " << (tSum.stato() == AVVIATO ? "OK" : "NO") << endl;
    cout << tSum << endl; // Visualizzazione finale

    return 0;
}