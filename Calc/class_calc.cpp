// class_calc.cpp
//---------------------------------------------------------------------
// Plik Ÿród³owy z klas¹ interpretuj¹c¹ wyra¿enie i obliczaj¹c¹ wynik
//---------------------------------------------------------------------

#include "stdafx.h"
#include "class_calc.h"
#include <iostream>

using namespace std;

// Konstruktor klasy Calc - wstawia wyra¿enie, jeœli podano
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

// Metoda oblicza zadane wyra¿enie. W przypadku niepoprawnej sk³adni zwraca false
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

	 // Pobierz pierwsz¹ liczbê
	dVal1 = checkSign() * getVal();
	if (eError != CalcError::NO)
		return eError;

	 // Powtarzaj pêtlê a¿ do koñca
	while (uiIdx != sCurrentExp.length())
	{
		switch (getChar())
		{
		case '+':
			 // Jeœli wyst¹pi³ po mno¿eniu (dzieleniu) zakoñcz mno¿enie
			if (eStatus == P_STAT_MULTI)
				eStatus = P_STAT_ADD;

			 // Dodaj poprzedni¹ liczbê/iloczyn do wyniku;
			dResult += dVal1;

			 // Pobierz nastêpny sk³adnik
			dVal1 = getVal();
			if (eError != CalcError::NO)
				return eError;

			break;

		case '-':
			 // Jeœli wyst¹pi³ po mno¿eniu (dzieleniu) zakoñcz mno¿enie i dodaj iloczyn do wyniku
			if (eStatus == P_STAT_MULTI)
				eStatus = P_STAT_ADD;

			 // Dodaj poprzedni¹ liczbê/iloczyn do wyniku;
			dResult += dVal1;

			 // Pobierz nastêpny sk³adnik
			dVal1 = -getVal();
			if (eError != CalcError::NO)
				return eError;

			break;

		case '*':
			eStatus = P_STAT_MULTI;

			 // Domnó¿ nastêpn¹ liczbê do iloczynu
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
				sErrMsg = "dzielenie przez 0... jesteœ debilem xD";
				uiErrBegIdx = uiIdxSave;
				uiErrEndIdx = sCurrentExp.find_last_not_of(" \t\f\v\n\r", uiIdx-1);		// Podaj zakres wyra¿enia równego 0, pomijaj¹c spacje
				return eError;
			}

			// Podziel iloczyn przez nastêpn¹ liczbê
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

// Metoda wstawia do obiektu nowe wyra¿enie do obliczenia
//-------------------------------------------------------
// sExp - wyra¿enie do wstawienia
//-------------------------------------------------------
void Calc::insertExp(string sExp)
{
	reset();

	 // Wyczyœæ niepotrzebne spacje na pocz¹tku i na koñcu
	size_t	uiFirstAfterSpace = sExp.find_first_not_of(" \t\f\v\n\r");
	size_t	uiLastBeforeSpace = sExp.find_last_not_of(" \t\f\v\n\r");
	if (uiFirstAfterSpace != string::npos)
		sExp = sExp.substr(uiFirstAfterSpace, uiLastBeforeSpace - uiFirstAfterSpace + 1);

	sCurrentExp = sExp;
}

// Metoda pobiera obecnie przetwarzane wyra¿enie
//----------------------------------------------
string Calc::getExp()
{
	return sCurrentExp;
}

// Metoda pobiera bie¿¹cy wynik obliczeñ
//-------------------------------------------------------------------------------------------------------
double Calc::getResult()
{
	return dResult;
}

// Metoda pobiera bie¿¹c¹ wiadomoœæ b³êdu
//-------------------------------------------------------------------------------------------------------
string Calc::getErrMsg()
{
	return sErrMsg;
}

// Metoda zwraca indeks, na którym siê zatrzyma³a metoda count
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

// Metoda pobiera nastêpn¹ liczbê (wyra¿enie w nawiasie) z wyra¿enia, zwraca wartoœæ i przesuwa wskaŸnik
//------------------------------------------------------------------------------------------------------
double Calc::getVal()
{
	 // Jeœli ju¿ wyst¹pi³ b³¹d, nie rób nic
	if (eError != CalcError::NO)
		return 0.0;

	 // Jeœli napotkano koniec, zg³oœæ niespodziewany koniec wyra¿enia
	if (uiIdx >= sCurrentExp.length())
	{
		eError = CalcError::UNEXPECTED_END;
		sErrMsg = "niespodziewany koniec wyrazenia";
		uiErrEndIdx = uiErrBegIdx = uiIdx;
		return 0.0;
	}

	double				dVal = 0.0;
	char				cChar;
	bool				bWasDigit = false;			// czy pojawi³a siê czêœæ dziesiêtna?
	double				dFracFactor = 1.0;			// czynnik czêœci dziesiêtnej
	CALC_GETVAL_STATUS	eStat = GV_STAT_INIT;

	 // Wczytaj kolejne znaki
	while (true)
	{
		cChar = sCurrentExp[uiIdx];

		 // Jeœli spacje przed liczb¹, pomiñ; jeœli po, przeskocz i zakoñcz pobieranie
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
		 // Pierwszy niebia³y znak po przeskakiwaniu spacji po liczbie - przerwij
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
		 // Pojawi³ siê separator dziesiêtny - zmieñ status na napotkanie separatora
		else if (cChar == '.' || cChar == ',')
		{
			 // Mo¿liwe tylko na pocz¹tku (.56) jub po czêœci dziesiêtnej (7.1, 56.)
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

			 // Jeœli napotkano cyfrê na pocz¹tku (lub po spacjach) zmieñ status na pobór czêœci dziesiêtnej i zaznacz, ¿e podano czêœæ dziesiêtn¹
			if (eStat == GV_STAT_INIT)
				eStat = GV_STAT_DEC;
			 // Jeœli poprzednio by³ separator, zmieñ tryb na pobór czêœci u³amkowej
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
				 // Napotkano na wyra¿enie w nawiasie zamiast liczby
				if (cChar == '(')
				{
					uiIdx++;
					size_t			uiIdxStart = uiIdx;
					unsigned short	usLvl = 1;

					 // Szukaj koñca wyra¿enia w nawiasie
					while (true)
					{
						switch (sCurrentExp[uiIdx])
						{
						case '(':
							usLvl++;
							break;

						case ')':
							usLvl--;

							 // Domkniêto wyra¿enie
							if (!usLvl)
							{
								 // Wydziel wyra¿enie w nawiasie
								string	sBracketExp = sCurrentExp.substr(uiIdxStart, uiIdx - uiIdxStart);

								 // SprawdŸ, czy wyra¿enie nie jest puste (ignoruj¹c spacje)
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

								 // Pomiñ spacje po nawiasie
								while (isspace(sCurrentExp[++uiIdx])) { }

								 // Oblicz wyra¿enie w nawiasie i zwróæ
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

						 // Napotkano na koniec przed domkniêciem nawiasu
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
				 // Napotkano na niedozwolony znak na pocz¹tku liczby
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

// Metoda pobiera znak z wyra¿enia ('*' w przypadku napotkania nawiasu) i przesuwa wskaŸnik
//-----------------------------------------------------------------------------------------
char Calc::getChar()
{
	// Jeœli ju¿ wyst¹pi³ b³¹d, nie rób nic
	if (eError != CalcError::NO)
		return 0;

	// Jeœli napotkano koniec, zg³oœæ niespodziewany koniec wyra¿enia
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

		 // Napotkanie plusa: jeœli wyst¹pi³ po * lub / przerwij; ustaw + b¹dŸ nie zmieñ znaku
		case '+':
			if (!cChar)
				cChar = '+';
			else if (cChar == '*' || cChar == '/')
				return cChar;

			uiIdx++;
			break;

		 // Napotkanie minusa: jeœli wyst¹pi³ po * lub / przerwij; ustaw - lub zmieñ znak
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

		 // Napotkanie mno¿enia: jeœli wyst¹pi³o po innym znaku, zg³oœ b³¹d
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

		 // Napotkanie dzielenia: jeœli wyst¹pi³o po innym znaku, zg³oœ b³¹d
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

		 // Napotkanie nawiasu: zwróc domyœlny '*' i nie przesuwaj indeksu
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

				 // Brak³o znaku miêdzy poprawnymi liczbami
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
			 // Napotkano na inny znak po sczytaniu dzia³ania - przerwij
			else
			{
				return cChar;
			}
			break;
		}
	}
}

// Metoda sprawdza, czy przed liczb¹ nie stoi znak (+, -, kombinacje). Zwraca 1 lub -1 w zale¿noœci od kombinacji
//---------------------------------------------------------------------------------------------------------------
char Calc::checkSign()
{
	// Jeœli ju¿ wyst¹pi³ b³¹d, nie rób nic
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