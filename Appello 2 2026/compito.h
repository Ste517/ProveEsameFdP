//
// Created by Stefano Brogi on 26/01/2026.
//

#ifndef COMPITO_H
#define COMPITO_H

#include <iostream>
using namespace std;

enum Stato {MODIFICA, AVVIATO, VINTO, PERSO};

class TiltMaze {
	Stato stato_gioco;

	int R;
	int C;

	int rSt;
	int cSt;

	int rEx;
	int cEx;

	int rPalla;
	int cPalla;

	char** labirinto;

	bool controlloPalla(int currentR, int currentC);

public:

	TiltMaze(int R, int C, int rSt, int cSt, int rEx, int cEx);
	bool aggiungiMuro(int r1, int c1, int r2, int c2);
	bool aggiungiBuca(int r, int c);
	void avvia();
	friend ostream& operator<<(ostream& os, const TiltMaze& t);
	Stato stato() const;
	~TiltMaze();
	void inclina(int dirV, int dirH);
	TiltMaze(const TiltMaze& t);
	TiltMaze operator+(const TiltMaze& t);
};


#endif //COMPITO_H

// fine file