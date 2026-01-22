#include <iostream>
#include "compito.h"
using namespace std;

int main() {

    cout << "--- PRIMA PARTE ---" << endl<<endl;

    cout<<"Test del costruttore:"<<endl;
    CampoMinato c(3);
    cout << c << endl;

    cout<<"Test della aggiungi_mina:"<<endl;
    c.aggiungi_mina(1, 2);
    c.aggiungi_mina(2, 1);
    cout << c << endl;

    cout<<"Test della scopri:"<<endl;
    c.scopri(2, 2);
    cout << c << endl;

    c.scopri(0, 1);
    cout << c << endl;

    c.scopri(1,2);
    cout << c << endl;

    cout << endl << "--- SECONDA PARTE ---" << endl<<endl;

    cout<<"Test del costruttore di copia:"<<endl;
    CampoMinato c2(c);
    cout << c2 << endl;

    cout<<"Altri Test della scopri:"<<endl;
    c2.scopri(2, 2);
    c2.scopri(0, 0);
	cout << c2 << endl;
    c2.scopri(2, 0);
    c2.scopri(0, 2);
    cout << c2 << endl;

    cout<<"Test dell'operatore +:"<<endl;
    CampoMinato c6 = c + c2;
    c6.scopri(5, 5);
    c6.scopri(2, 2);
    cout << c6 << endl;

    {
        cout<<"Test del distruttore:"<<endl;
        CampoMinato c7(9);
    }



    // TERZA PARTE RIMOSSA AUTOMATICAMENTE
}
