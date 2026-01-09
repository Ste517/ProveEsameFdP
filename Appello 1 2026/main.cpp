#include "compito.h"

int main() {
    cout << "--- PRIMA PARTE ---" << endl;
	
    cout << "Test costruttore e aggiungiFarmaco" << endl;
    CasaDiCura cc("Giardino felice", 2);
    cc.aggiungiFarmaco(1, "Fluifort", 10);
    cc.aggiungiFarmaco(1, "Tachiflu", 24); // orario scorretto
    cc.aggiungiFarmaco(2, "Lasonil", 7);
    cc.aggiungiFarmaco(2, "Aulin", 11);
    cc.aggiungiFarmaco(2, "Tachipirina", 18);
    cout << endl << cc;

    cout << endl << "Test assumiFarmaci" << endl;
    cc.assumiFarmaci(2, 7);
    cc.assumiFarmaci(2, 18);
    cout << endl << cc;

    
	cout << endl << "--- SECONDA PARTE ---" << endl;
	
    cout << "Test costruttore di copia" << endl;
    CasaDiCura cc2(cc);
    cout << endl << cc2;

    cout << endl << "Test eventuale distruttore" << endl;
    {
        CasaDiCura cc1("Prova", 1);
    }
    cout << "Distruttore chiamato" << endl;

    cout << endl << "Test rimuoviFarmaco" << endl;
    CasaDiCura cc3 = cc.rimuoviFarmaco(2, "Tachipirina");
    cout << endl << cc3;

    cout << endl << "Test operatore negazione" << endl;
    !cc;
    cout << endl << cc;


    cout << endl << "--- 
    // TERZA PARTE RIMOSSA AUTOMATICAMENTE
}
