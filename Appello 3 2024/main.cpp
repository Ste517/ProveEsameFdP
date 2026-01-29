#include <iostream>
#include "compito.h"

using namespace std;

int main() {

    cout<<endl<<"--- PRIMA PARTE ---" <<endl<<endl;

    cout<<"Test del costruttore:" << endl;
    SpaceAsteroids s(6, 7, 3);
    cout << s << endl;

    cout << "Test della colloca_asteroide:" << endl;
    cout << s.colloca_asteroide(1);
    cout << s.colloca_asteroide(4);
    cout << s.colloca_asteroide(7);
    cout << endl << endl;
    cout << s << endl;

    cout << "Test della avanzamento_asteroidi:" << endl;
    s.avanza();
    cout << s << endl;

    cout<<endl<<"--- SECONDA PARTE ---" <<endl<<endl;

    cout << "Test operatore <<=:" << endl;
    s <<= 5;
    cout << s << endl;
    
    cout << "Test operatore |=:" << endl;
    s |= 2;
    cout << s << endl;

    cout << "Test della avanza:" << endl;
    s.avanza();
    cout << s << endl;

    cout << "Altro test della avanza:" << endl;
    s.avanza();
    cout << s << endl;
    // TERZA PARTE RIMOSSA AUTOMATICAMENTE
}
