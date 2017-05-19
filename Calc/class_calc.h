// class_calc.h
//---------------------------------------------------------------------
// Plik nag³ówkowy z klas¹ interpretuj¹c¹ wyra¿enie i obliczaj¹c¹ wynik
//---------------------------------------------------------------------

#pragma once

#include "class_calcerror.h"
#include <string>
#include <math.h>

using namespace std;

 // Wyliczenie statusu poboru liczby
enum CALC_GETVAL_STATUS
{
	GV_STAT_INIT,
	GV_STAT_DEC,
	GV_STAT_DELIM,
	GV_STAT_FRAC,
	GV_STAT_EXP,
	GV_STAT_EXIT_SPACE
};

 // Wyliczenie statusu parsingu wyra¿enia
enum CALC_PARSE_STATUS
{
	P_STAT_INIT,
	P_STAT_ADD,
	P_STAT_MULTI
};

class Calc
{
public:
	Calc(string sExp = "");
	~Calc();

	CalcErrType		count();
	void			reset();
	void			insertExp(string sExp);
	string			getExp();
	double			getResult();
	string			getErrMsg();
	size_t			getIdx();
	size_t			getErrBegIdx();
	size_t			getErrEndIdx();

private:
	 // Zmienne
	string			sCurrentExp;			// Bie¿¹ce wyra¿enie
	size_t			uiIdx = 0;				// Bie¿¹cy indeks
	size_t			uiErrBegIdx = 0;		// Indeks pocz¹tku b³êdu
	size_t			uiErrEndIdx = 0;		// Indeks koñca
	size_t			uiOffset = 1;			// Przeuniêcie indeksu przy wyœwietlaniu - u¿ywane przy zagnie¿d¿onym w nawiasach wyra¿eniu
	double			dResult = 0.0;			// Bie¿¹cy wynik
	char			cSign = 1;				// Bie¿¹cy znak liczby
	bool			bMulti = true;			// true - dokonujemy mno¿enia; false - dzielenia
	CalcErrType		eError = CalcError::NO;		// Biê¿¹cy kod b³êdu
	string			sErrMsg = "";			// Bie¿¹ca wiadomoœæ b³êdu

	 // Metody
	double			getVal();
	char			getChar();
	char			checkSign();
};