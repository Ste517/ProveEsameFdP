// file main.cpp

#include "compito.h"
#include <iostream>
using namespace std;

int main(){
    
    cout<<"--- PRIMA PARTE ---" <<endl;
    cout << "Test del costruttore e operatore di uscita" <<endl;
    Biblioteca b(4);
    cout<<b<<endl;
    
    cout << "Test della aggiungi" <<endl;
    b.aggiungi("9788838668821", "Il Nome della Rosa", 1980);
    cout<<b<<endl;
    
    b.aggiungi("9788807881916", "1984", 1949);
    b.aggiungi("9788806221669", "Se questo e' un uomo", 1947);
    cout<<b<<endl;
    
    cout << "Test del metodo disponibili (deve stampare 3)" <<endl;
    cout<<b.disponibili()<<endl<<endl;
    
    cout<<"--- SECONDA PARTE ---" <<endl;
    cout << "Test del prestito" <<endl;
    b.prestito("9788807881916");
    cout<<b<<endl;
    
    cout << "Test del disponibili dopo prestito (deve stampare 2)" <<endl;
    cout<<b.disponibili()<<endl<<endl;
    
    cout << "Test della restituzione" <<endl;
    b.restituzione("9788807881916");
    cout<<b<<endl;
    
    cout << "Test salvataggio su file" <<endl;
    if(b.salvaFile("biblioteca.txt"))
        cout<<"File salvato con successo"<<endl;
    
    cout << "Test caricamento da file" <<endl;
    Biblioteca b2(10);
    int loaded = b2.caricaFile("biblioteca.txt");
    cout<<"Libri caricati: "<<loaded<<endl;
    cout<<b2<<endl;
    
    return 0;
}