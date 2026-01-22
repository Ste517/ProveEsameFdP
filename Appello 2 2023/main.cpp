#include <iostream>
#include "compito.h"

using namespace std;

int main() {

    cout << "--- PRIMA PARTE ---" << endl<<endl;

    cout<<"Test del costruttore:"<<endl;
    Supermercato s;
    cout<<s<<endl;

    cout<<"Test della crea_prodotto:"<<endl;
    s.crea_prodotto("pere", 2.3);
    s.crea_prodotto("sale", 1.3);
    s.crea_prodotto("ananas", 2.1);
    cout<<s<<endl;

    cout<<"Test della esponi:"<<endl;
    s.esponi("pere", 10);
    s.esponi("sale", 4);
    s.esponi("ananas", 3);
    cout<<s<<endl;
    {
        cout<<"Test dell'eventuale distruttore"<<endl<<endl;

        Supermercato ss;
        ss.crea_prodotto("pere", 2.3);
        ss.crea_prodotto("sale", 1.3);
        ss.crea_prodotto("ananas", 2.1);
        ss.esponi("pere", 10);
        ss.esponi("sale", 4);
        ss.esponi("ananas", 4);
    }

    

    cout << endl << "--- SECONDA PARTE ---" << endl<<endl;

    cout<<"Test dell'operatore +=:"<<endl;
    s += 42940;
    s += 42950;
    s += 42951;
    cout<<s<<endl;

    cout<<"Test della metti_nel_carrello:"<<endl;
    s.metti_nel_carrello(42950, "pere", 2);
    s.metti_nel_carrello(42950, "sale", 1);
    s.metti_nel_carrello(42950, "ananas", 1);
    cout<<s<<endl;

    cout<<"Altro test della esponi:"<<endl;
    s.esponi("ananas", 2);
    s.esponi("pere", 1);
    cout<<s<<endl;

    cout<<"Altro test della metti_nel_carrello:"<<endl;
    s.metti_nel_carrello(42940, "ananas", 2);
    s.metti_nel_carrello(42950, "ananas", 1);
    s.metti_nel_carrello(42950, "sale", 1);
    s.metti_nel_carrello(42951, "sale", 1);
    cout<<s<<endl;

    cout<<"Test della acquista:"<<endl;
    cout<<"Spesa sostenuta: "<<s.acquista(42950)<<endl<<endl;
    cout<<s<<endl;

    {
        cout<<"Altro test dell'eventuale distruttore"<<endl<<endl;

        Supermercato ss;
        ss.crea_prodotto("pere", 2.3);
        ss.crea_prodotto("sale", 1.3);
        ss.crea_prodotto("ananas", 2.1);
        ss += 42950;
        ss += 42951;
        ss += 42940;
        ss.esponi("pere", 10);
        ss.esponi("sale", 4);
        ss.esponi("ananas", 4);
        ss.metti_nel_carrello(42950, "pere", 2);
        ss.metti_nel_carrello(42950, "sale", 1);
        ss.metti_nel_carrello(42950, "ananas", 1);
    }
    // TERZA PARTE RIMOSSA AUTOMATICAMENTE
}
