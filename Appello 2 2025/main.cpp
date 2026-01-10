#include "compito.h"


int main() {


    cout << "--- PRIMA PARTE ---" << endl;

    UfficioPostale up(3);
    up.accodaUtente("Maria Rossi", 1);
    up.accodaUtente("Luigi Bianchi", 1);
    up.accodaUtente("Chiara Biondi", 1);
    up.accodaUtente("Giuseppe Verdi", 2);
    // quest'ultimo non deve accodare l'utente in quanto stringa vuota
    up.accodaUtente("", 3);


    cout << "Test costruttore e funzione accodaUtente" << endl << endl;
    cout << up << endl << endl << endl;


    //servo l'utente Maria Rossi
    up.serviUtente(1);


    cout << "Test funzione serviUtente" << endl << endl;
    cout << up << endl << endl << endl;

    cout << "--- SECONDA PARTE ---" << endl;

    cout << "Test eventuale distruttore" << endl;
    {
        UfficioPostale up1(4);
    }
    cout << "Distruttore chiamato" << endl << endl << endl;


    //accodo 4 utenti prioritari ai vari sportelli
    up.accodaPrioritario("Simona Gialli");
    up.accodaPrioritario("Daniela Neri");
    up.accodaPrioritario("Francesco Grigi");
    up.accodaPrioritario("Maurizio Violi");


    cout << "Test funzione accodaPrioritario" << endl << endl;
    cout << up << endl << endl << endl;

    //faccio passare avanti Biondi e Verdi (che diventa prioritario)
    up.passaAvanti("Chiara Biondi", 1, 1);
    up.passaAvanti("Giuseppe Verdi", 2, 1);


    cout << "Test funzione passaAvanti" << endl << endl;
    cout << up << endl << endl << endl;


    //applico l'operatore di negazione logica su up

    !up;


    cout << "Test operatore di negazione" << endl << endl;
    cout << up << endl << endl << endl;

    // TERZA PARTE RIMOSSA AUTOMATICAMENTE
}
