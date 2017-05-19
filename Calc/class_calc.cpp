// class_calc.cpp
//---------------------------------------------------------------------
// Plik �r�d�owy z klas� interpretuj�c� wyra�enie i obliczaj�c� wynik
//---------------------------------------------------------------------

#include "stdafx.h"
#include "class_calc.h"
#include <iostream>

using namespace std;

// Konstruktor klasy Calc - wstawia wyra�enie, je�li podano
//---------------------------------------------------------
Calc::Calc(string sExp)
{
	sCurrentExp = sExp;
}

// Destruktor klasy Calc
//-----------------------------------------
Calc::~Calc()
{

}

// Metoda oblicza zadane wyra�enie. W przypadku niepoprawnej sk�adni zwraca false
//-------------------------------------------------------------------------------
CalcErrType Calc::count()
{
	dResult = 0.0;
	uiIdx = 0;
	eError = CalcError::NO;

	double				dFactor = 1.0;
	bool				dMulti = true;
	double				dVal1, dVal2;
	CALC_PARSE_STATUS	eStatus = P_STAT_ADD;
	size_t				uiIdxSave;

	 // Pobierz pierwsz� liczb�
	dVal1 = checkSign() * getVal();
	if (eError != CalcError::NO)
		return eError;

	 // Powtarzaj p�tl� a� do ko�ca
	while (uiIdx != sCurrentExp.length())
	{
		switch (getChar())
		{
		case '+':
			 // Je�li wyst�pi� po mno�eniu (dzieleniu) zako�cz mno�enie
			if (eStatus == P_STAT_MULTI)
				eStatus = P_STAT_ADD;

			 // Dodaj poprzedni� liczb�/iloczyn do wyniku;
			dResult += dVal1;

			 // Pobierz nast�pny sk�adnik
			dVal1 = getVal();
			if (eError != CalcError::NO)
				return eError;

			break;

		case '-':
			 // Je�li wyst�pi� po mno�eniu (dzieleniu) zako�cz mno�enie i dodaj iloczyn do wyniku
			if (eStatus == P_STAT_MULTI)
				eStatus = P_STAT_ADD;

			 // Dodaj poprzedni� liczb�/iloczyn do wyniku;
			dResult += dVal1;

			 // Pobierz nast�pny sk�adnik
			dVal1 = -getVal();
			if (eError != CalcError::NO)
				return eError;

			break;

		case '*':
			eStatus = P_STAT_MULTI;

			 // Domn� nast�pn� liczb� do iloczynu
			dVal1 *= checkSign() * getVal();
			if (eError != CalcError::NO)
				return eError;

			break;

		case '/':
			eStatus = P_STAT_MULTI;

			uiIdxSave = uiIdx;
			dVal2 = checkSign() * getVal();
			if (eError != CalcError::NO)
				return eError;

			 // Zabezpiecz przed dzieleniem przez 0
			if (abs(dVal2) < std::numeric_limits<double>::epsilon())
			{
				eError = CalcError::DIV_ZERO;
				sErrMsg = "dzielenie przez 0... jeste� debilem xD";
				uiErrBegIdx = uiIdxSave;
				uiErrEndIdx = sCurrentExp.find_last_not_of(" \t\f\v\n\r", uiIdx-1);		// Podaj zakres wyra�enia r�wnego 0, pomijaj�c spacje
				return eError;
			}

			// Podziel iloczyn przez nast�pn� liczb�
			dVal1 /= dVal2;

			break;

		default:
			return eError;
			break;
		}
	}

	dResult += dVal1;
	return eError;
}

// Funkcja resetuje zmienne obiektu
//--------------------------------------------------------
void Calc::reset()
{
	sCurrentExp = "";
	uiIdx = 0;
	uiErrBegIdx = 0;
	uiErrEndIdx = 0;
	dResult = 0.0;
	cSign = 1;
	bMulti = true;
	eError = CalcError::NO;
	sErrMsg = "";
	uiOffset = 0;
}

// Metoda wstawia do obiektu nowe wyra�enie do obliczenia
//-------------------------------------------------------
// sExp - wyra�enie do wstawienia
//-------------------------------------------------------
void Calc::insertExp(string sExp)
{
	reset();

	 // Wyczy�� niepotrzebne spacje na pocz�tku i na ko�cu
	size_t	uiFirstAfterSpace = sExp.find_first_not_of(" \t\f\v\n\r");
	size_t	uiLastBeforeSpace = sExp.find_last_not_of(" \t\f\v\n\r");
	if (uiFirstAfterSpace != string::npos)
		sExp = sExp.substr(uiFirstAfterSpace, uiLastBeforeSpace - uiFirstAfterSpace + 1);

	sCurrentExp = sExp;
}

// Metoda pobiera obecnie przetwarzane wyra�enie
//----------------------------------------------
string Calc::getExp()
{
	return sCurrentExp;
}

// Metoda pobiera bie��cy wynik oblicze�
//-------------------------------------------------------------------------------------------------------
double Calc::getResult()
{
	return dResult;
}

// Metoda pobiera bie��c� wiadomo�� b��du
//-------------------------------------------------------------------------------------------------------
string Calc::getErrMsg()
{
	return sErrMsg;
}

// Metoda zwraca indeks, na kt�rym si� zatrzyma�a metoda count
//-------------------------------------------------------------
size_t Calc::getIdx()
{
	return uiIdx;
}

// Metoda zwraca indeks napotkanego bledu (lub string::npos, gdy blad nie wystapil)
//------------------------------------------------------------------------------------------------------
size_t Calc::getErrBegIdx()
{
	return eError == CalcError::NO ? string::npos : uiErrBegIdx;
}

// Metoda zwraca dodatkowy indeks napotkanego bledu (lub string::npos, gdy blad nie wystapil)
//------------------------------------------------------------------------------------------------------
size_t Calc::getErrEndIdx()
{
	return eError == CalcError::NO ? string::npos : uiErrEndIdx;
}

// Metoda pobiera nast�pn� liczb� (wyra�enie w nawiasie) z wyra�enia, zwraca warto�� i przesuwa wska�nik
//------------------------------------------------------------------------------------------------------
double Calc::getVal()
{
	 // Je�li ju� wyst�pi� b��d, nie r�b nic
	if (eError != CalcError::NO)
		return 0.0;

	 // Je�li napotkano koniec, zg�o�� niespodziewany koniec wyra�enia
	if (uiIdx >= sCurrentExp.length())
	{
		eError = CalcError::UNEXPECTED_END;
		sErrMsg = "niespodziewany koniec wyrazenia";
		uiErrEndIdx = uiErrBegIdx = uiIdx;
		return 0.0;
	}

	double				dVal = 0.0;
	char				cChar;
	bool				bWasDigit = false;			// czy pojawi�a si� cz�� dziesi�tna?
	double				dFracFactor = 1.0;			// czynnik cz�ci dziesi�tnej
	CALC_GETVAL_STATUS	eStat = GV_STAT_INIT;

	 // Wczytaj kolejne znaki
	while (true)
	{
		cChar = sCurrentExp[uiIdx];

		 // Je�li spacje przed liczb�, pomi�; je�li po, przeskocz i zako�cz pobieranie
		if (isspace (cChar))
		{
			if (eStat != GV_STAT_INIT)
			{
				eStat = GV_STAT_EXIT_SPACE;

				 // Wpisanie samej kropki
				if (!bWasDigit)
				{
					eError = CalcError::UNEXPECTED_SIGN;
					sErrMsg = "niespodziewany znak \'" + sCurrentExp.substr(uiIdx - 1, 1) + "\'";
					uiErrEndIdx = uiErrBegIdx = uiIdx - 1;
					return 0.0;
				}
			}

			uiIdx++;
		}
		 // Pierwszy niebia�y znak po przeskakiwaniu spacji po liczbie - przerwij
		else if (eStat == GV_STAT_EXIT_SPACE)
		{
			if (!bWasDigit)
			{
				eError = CalcError::UNEXPECTED_SIGN;
				sErrMsg = "niespodziewany znak \'.\'";
				uiErrEndIdx = uiErrBegIdx = uiIdx;
				return 0.0;
			}
			return dVal;
		}
		 // Pojawi� si� separator dziesi�tny - zmie� status na napotkanie separatora
		else if (cChar == '.' || cChar == ',')
		{
			 // Mo�liwe tylko na pocz�tku (.56) jub po cz�ci dziesi�tnej (7.1, 56.)
			if (eStat != GV_STAT_INIT && eStat != GV_STAT_DEC)
			{
				eError = CalcError::UNEXPECTED_SIGN;
				sErrMsg = "niespodziewany znak \'" + sCurrentExp.substr(uiIdx, 1) + "\'";
				uiErrEndIdx = uiErrBegIdx = uiIdx;
				return 0.0;
			}

			eStat = GV_STAT_DELIM;
			uiIdx++;
		}
		else if (cChar >= 48 && cChar <= 57)
		{
			bWasDigit = true;

			 // Je�li napotkano cyfr� na pocz�tku (lub po spacjach) zmie� status na pob�r cz�ci dziesi�tnej i zaznacz, �e podano cz�� dziesi�tn�
			if (eStat == GV_STAT_INIT)
				eStat = GV_STAT_DEC;
			 // Je�li poprzednio by� separator, zmie� tryb na pob�r cz�ci u�amkowej
			else if (eStat == GV_STAT_DELIM)
				eStat = GV_STAT_FRAC;

			 // Pobieranie dziesietnej
			if (eStat == GV_STAT_DEC)
				dVal = dVal * 10.0 + (double)(cChar - 48);
			 // Pobieranie ulamkowej
			else if (eStat == GV_STAT_FRAC)
				dVal += (double)(cChar - 48) * (dFracFactor /= 10.0);
			else
			{
				eError = CalcError::UNKNOWN;
				sErrMsg = "wystapil nieznany blad; zglos programiscie podajac wpisane wyrazenie (z uwzglednieniem spacji)";
				uiErrEndIdx = uiErrBegIdx = uiIdx;
				return 0.0;
			}

			uiIdx++;
		}
		else
		{
			if (eStat == GV_STAT_INIT)
			{
				 // Napotkano na wyra�enie w nawiasie zamiast liczby
				if (cChar == '(')
				{
					uiIdx++;
					size_t			uiIdxStart = uiIdx;
					unsigned short	usLvl = 1;

					 // Szukaj ko�ca wyra�enia w nawiasie
					while (true)
					{
						switch (sCurrentExp[uiIdx])
						{
						case '(':
							usLvl++;
							break;

						case ')':
							usLvl--;

							 // Domkni�to wyra�enie
							if (!usLvl)
							{
								 // Wydziel wyra�enie w nawiasie
								string	sBracketExp = sCurrentExp.substr(uiIdxStart, uiIdx - uiIdxStart);

								 // Sprawd�, czy wyra�enie nie jest puste (ignoruj�c spacje)
								size_t	uiFirstAfterSpace = sBracketExp.find_first_not_of(" \t\f\v\n\r");
								size_t	uiLastBeforeSpace = sBracketExp.find_last_not_of(" \t\f\v\n\r");
								if (uiFirstAfterSpace == string::npos || sBracketExp.substr(uiFirstAfterSpace, uiLastBeforeSpace - uiFirstAfterSpace + 1).empty())
								{
									uiErrBegIdx = uiIdxStart - 1;
									uiErrEndIdx = uiIdx;
									eError = CalcError::EMPTY_BRACKETS;
									sErrMsg = "puste nawiasy";
									return 0.0;
								}

								 // Pomi� spacje po nawiasie
								while (isspace(sCurrentExp[++uiIdx])) { }

								 // Oblicz wyra�enie w nawiasie i zwr��
								Calc	oBracketExp(sBracketExp);
								oBracketExp.uiOffset = uiIdxStart + uiOffset;
								oBracketExp.count();

								eError = oBracketExp.eError;
								sErrMsg = oBracketExp.sErrMsg;

								if (eError != CalcError::NO)
								{
									uiErrBegIdx = oBracketExp.uiErrBegIdx + uiIdxStart;
									uiErrEndIdx = oBracketExp.uiErrEndIdx + uiIdxStart;
								}
								return oBracketExp.eError == CalcError::NO ? oBracketExp.dResult : 0.0;
							}

							break;

						 // Napotkano na koniec przed domkni�ciem nawiasu
						case 0:
							eError = CalcError::NOT_CLOSED_BRACKET;
							sErrMsg = "niedomkniete nawiasy";
							uiErrBegIdx = uiIdxStart - 1;
							uiErrEndIdx = uiIdx - 1;
							return 0.0;

							break;
						}
						uiIdx++;
					}
				}
				 // Napotkano na niedozwolony znak na pocz�tku liczby
				else
				{
					eError = CalcError::UNEXPECTED_SIGN;
					sErrMsg = "niespodziewany znak \'" + sCurrentExp.substr(uiIdx, 1) + "\'";
					uiErrEndIdx = uiErrBegIdx = uiIdx;
					return 0.0;
				}

			}
			 // Wpisanie samej kropki
			else if (!bWasDigit)
			{
				eError = CalcError::UNEXPECTED_SIGN;
				sErrMsg = "niespodziewany znak \'" + sCurrentExp.substr(uiIdx - 1, 1) + "\'";
				uiErrEndIdx = uiErrBegIdx = uiIdx - 1;
				return 0.0;
			}
			else
			{
				return dVal;
			}
		}
	}
}

// Metoda pobiera znak z wyra�enia ('*' w przypadku napotkania nawiasu) i przesuwa wska�nik
//-----------------------------------------------------------------------------------------
char Calc::getChar()
{
	// Je�li ju� wyst�pi� b��d, nie r�b nic
	if (eError != CalcError::NO)
		return 0;

	// Je�li napotkano koniec, zg�o�� niespodziewany koniec wyra�enia
	if (uiIdx >= sCurrentExp.length())
	{
		eError = CalcError::UNEXPECTED_END;
		sErrMsg = "niespodziewany koniec wyrazenia";
		uiErrEndIdx = uiErrBegIdx = uiIdx;		
		return 0;
	}
	
	char cChar = 0;

	while (true)
	{
		switch (sCurrentExp[uiIdx])
		{
		 // Opuszczaj spacje
		case ' ':
		case '\t':
		case '\f':
		case '\v':
		case '\n':
		case '\r':
			uiIdx++;
			break;

		 // Napotkanie plusa: je�li wyst�pi� po * lub / przerwij; ustaw + b�d� nie zmie� znaku
		case '+':
			if (!cChar)
				cChar = '+';
			else if (cChar == '*' || cChar == '/')
				return cChar;

			uiIdx++;
			break;

		 // Napotkanie minusa: je�li wyst�pi� po * lub / przerwij; ustaw - lub zmie� znak
		case '-':
			if (!cChar)
				cChar = '-';
			else if (cChar == '+')
				cChar = '-';
			else if (cChar == '-')
				cChar = '+';
			else
				return cChar;

			uiIdx++;
			break;

		 // Napotkanie mno�enia: je�li wyst�pi�o po innym znaku, zg�o� b��d
		case '*':
			if (cChar)
			{
				eError = CalcError::UNEXPECTED_SIGN;
				sErrMsg = "niespodziewany znak '*'";
				uiErrEndIdx = uiErrBegIdx = uiIdx;
				return 0;
			}

			cChar = '*';
			uiIdx++;
			break;

		 // Napotkanie dzielenia: je�li wyst�pi�o po innym znaku, zg�o� b��d
		case '/':
			if (cChar)
			{
				eError = CalcError::UNEXPECTED_SIGN;
				sErrMsg = "niespodziewany znak '/'";
				uiErrEndIdx = uiErrBegIdx = uiIdx;
				return 0;
			}

			cChar = '/';
			uiIdx++;
			break;

		 // Napotkanie nawiasu: zwr�c domy�lny '*' i nie przesuwaj indeksu
		case '(':
			if (cChar)
				return cChar;

			cChar = '*';
			return cChar;
			break;

		 // Inny znak
		default:
			if (!cChar)
			{
				size_t uiIdxMem = uiIdx;
				getVal();
				uiIdx = uiIdxMem;

				 // Brak�o znaku mi�dzy poprawnymi liczbami
				if (eError == CalcError::NO)
				{
					eError = CalcError::EXPECTED_SIGN;
					sErrMsg = "spodziewany jakis znak dzialania";
					uiErrEndIdx = uiErrBegIdx = uiIdx;
					return 0;
				}
				 // Napotkano niepoprawny znak
				else
				{
					eError = CalcError::UNEXPECTED_SIGN;
					sErrMsg = "niespodziewany znak \'" + sCurrentExp.substr(uiIdx, 1) + "\'";
					uiErrEndIdx = uiErrBegIdx = uiIdx;
					return 0;
				}
			}
			 // Napotkano na inny znak po sczytaniu dzia�ania - przerwij
			else
			{
				return cChar;
			}
			break;
		}
	}
}

// Metoda sprawdza, czy przed liczb� nie stoi znak (+, -, kombinacje). Zwraca 1 lub -1 w zale�no�ci od kombinacji
//---------------------------------------------------------------------------------------------------------------
char Calc::checkSign()
{
	// Je�li ju� wyst�pi� b��d, nie r�b nic
	if (eError != CalcError::NO)
		return 0;

	if (uiIdx >= sCurrentExp.length())
		return 1;

	char cSign = 1;
	char cChar;
	bool bRun = true;

	while (bRun)
	{
		cChar = sCurrentExp[uiIdx];

		switch (cChar)
		{
		case '-':
			cSign *= -1;
		case '+':
		case ' ':
		case '\t':
		case '\f':
		case '\v':
		case '\n':
		case '\r':
			uiIdx++;
			break;
		default:
			bRun = false;
			break;
		}
	}

	return cSign;
}