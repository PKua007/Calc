// class_calc.h
//---------------------------------------------------------------------
// Plik nag��wkowy z klas� interpretuj�c� wyra�enie i obliczaj�c� wynik
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

 // Wyliczenie statusu parsingu wyra�enia
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
	string			sCurrentExp;			// Bie��ce wyra�enie
	size_t			uiIdx = 0;				// Bie��cy indeks
	size_t			uiErrBegIdx = 0;		// Indeks pocz�tku b��du
	size_t			uiErrEndIdx = 0;		// Indeks ko�ca
	size_t			uiOffset = 1;			// Przeuni�cie indeksu przy wy�wietlaniu - u�ywane przy zagnie�d�onym w nawiasach wyra�eniu
	double			dResult = 0.0;			// Bie��cy wynik
	char			cSign = 1;				// Bie��cy znak liczby
	bool			bMulti = true;			// true - dokonujemy mno�enia; false - dzielenia
	CalcErrType		eError = CalcError::NO;		// Bi꿹cy kod b��du
	string			sErrMsg = "";			// Bie��ca wiadomo�� b��du

	 // Metody
	double			getVal();
	char			getChar();
	char			checkSign();
};