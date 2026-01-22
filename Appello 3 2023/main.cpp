#include <iostream>
#include <cstring>
#include "compito.h"
using namespace std;

int main() {

    cout << "--- PRIMA PARTE ---" << endl<<endl;

    cout<<"Test del costruttore ed operatore di uscita:"<<endl;
    ExchangeClient e1;
    ExchangeClient e2;
    ExchangeClient e3;

    cout << e1 << endl;

    cout << "Test della scrivi" << endl;
    cout << e1.scrivi("dato 1", 2) << " ";
    cout << e1.scrivi("dato 2", 0) << " ";
    cout << e2.scrivi("dato 3", 1) << " ";
    cout << e2.scrivi("dato 4", 1) << " ";
    cout << e3.scrivi("dato 5", 1)  << " ";
    cout << e3.scrivi("dato 6", 2) << endl << endl; // deve fallire, buffer pieni
    cout << e2 << endl;

    cout << "--- SECONDA PARTE ---" << endl<<endl;

    cout << "Test operatore di conversione ad intero" << endl;
    cout << (int)e1 << " ";
    cout << (int)e2 << " ";
    cout << (int)e3 << endl << endl;

    cout << "Test della leggi" << endl;
    char dato3[20];
    dato3[19] = '\0';
    cout << e1.leggi(dato3, 2) << endl;
    cout << dato3 << endl;
    cout << e1.leggi(dato3, 3) << endl;
    cout << dato3 << endl;
    cout << e2.leggi(dato3, 1) << endl;
    cout << dato3 << endl;
    cout << e3.leggi(dato3, 1) << endl;
    cout << dato3 << endl << endl;
    cout << e1 << endl << endl;

	cout << "Test della cifra" << endl;	
	char dato1[15];
	strcpy(dato1, "uvwxyzUVXYZ56789");
	ExchangeClient::cifra(dato1, 3, 5);
	cout << dato1  << endl << endl; 

    // PER TESTARE L'EVENTUALE IMPLEMENTAZIONE DEL VOSTRO DISTRUTTORE DECOMMENTARE IL SEGUENTE CODICE
    /*
    {
        cout << "Ultima registrazione" << endl;
        ExchangeClient e5;
        cout << "Invocazione del distruttore" << endl;
    }

    cout << "Distruzione avvenuta (apparentemente) con successo" << endl;
    */

    // TERZA PARTE RIMOSSA AUTOMATICAMENTE
}
