#include "stdafx.h"
#include "class_calcparser.h"
#include "newconio.h"

static int num = 0;

// Metoda wstawia do parsera nowe wyra�enie z przesuni�ciem
//------------------------------------------------------------------------
void CalcParser::insertExp(const std::string & szNewExp, size_t uiNewOffset)
{
	szExp = szNewExp;
	uiCurrIdx = 0;
	uiOffset = uiNewOffset;
}

// Metoda parsuje nast�pn� liczb�/wyra�enie w nawiasie/zmienn�/funkcj�
// z wyra�enia i przesuwa wska�nik. Uwzgl�dnia znak przed liczb�.
// Tworzy odpowiedni obiekt CalcTree i zwraca wska�nik. Ustawia warto��
// opInnerPointer na wska�nik do obiektu z pomini�ciem CalcTreeAddInv
//------------------------------------------------------------------------
CalcTree * CalcParser::getVal(CalcTree *& opInnerPointer)
{
	double				dVal = 0.0;
	double				dExp = 0.0;
	double				dExpSign = 1.0;
	unsigned char		cChar;
	size_t				uiExpSave;					// zapami�tana pozycja pocz�tku wyk�adnika
	bool				bWasDigit = false;			// czy pojawi�a si� jaka� cyfra?
	double				dFracFactor = 1.0;			// czynnik cz�ci dziesi�tnej
	CALC_GETVAL_STATUS	eStat = GV_STAT_INIT;
	bool				bUseAddInv = checkSign();	// sprawd�, czy wyst�puj� znaki unarne (typu -5, +5, --+-5)
	
	uiIdxSave = uiCurrIdx;	// U�ywaj pola "zapisany indeks" jako pocz�tku liczby

	 // Je�li napotkano koniec, zg�o�� niespodziewany koniec wyra�enia - musi by� po checkSign()
	if (uiCurrIdx >= szExp.length())
		throw CalcError(CalcError::UNEXPECTED_END, "niespodziewany koniec wyra�enia", uiCurrIdx + uiOffset, uiCurrIdx + uiOffset);

	 // Wczytaj kolejne znaki
	while (true)
	{
		cChar = szExp[uiCurrIdx];

		 // Je�li spacje przed liczb�, pomi�; je�li po, przeskocz i zako�cz pobieranie
		if (isspace(cChar))
		{
			if (eStat != GV_STAT_INIT && eStat != GV_STAT_EXIT_SPACE)
			{
				 // Wpisanie samej kropki
				if (!bWasDigit)
					throw CalcError(CalcError::UNEXPECTED_SIGN, "niespodziewany znak \'" + szExp.substr(uiCurrIdx - 1, 1) + "\'", uiCurrIdx - 1 + uiOffset, uiCurrIdx - 1 + uiOffset);
				 // Napisanie np 5.6e, 5.6e- lub 5.6e+ - interpretacja jako 5.6 * sta�a Eulera i ewentualnei znak dzia�ania - przerwij pobieranie
				else if (eStat == GV_STAT_E_DELIM || eStat == GV_STAT_EXP_SIGN)
				{
					uiCurrIdx = uiExpSave;  // przywr�� wska�nik na pozycj� 'e'
					opInnerPointer = new CalcTreeValue(countVal(dVal, dExpSign * dExp));
					return bUseAddInv ? (CalcTree *)new CalcTreeAddInv(opInnerPointer) : opInnerPointer;
				}

				eStat = GV_STAT_EXIT_SPACE;
			}

			uiCurrIdx++;
			if (eStat == GV_STAT_INIT)
				uiIdxSave++;
		}
		 // Pierwszy niebia�y znak po przeskakiwaniu spacji po liczbie - przerwij
		else if (eStat == GV_STAT_EXIT_SPACE)
		{
			opInnerPointer = new CalcTreeValue(countVal(dVal, dExpSign * dExp));
			return bUseAddInv ? (CalcTree *)new CalcTreeAddInv(opInnerPointer) : opInnerPointer;
		}
		 // Pojawi�a si� cyfra
		else if (cChar >= 48 && cChar <= 57)
		{
			bWasDigit = true;

			 // Je�li napotkano cyfr� na pocz�tku (lub po spacjach) zmie� status na pob�r cz�ci dziesi�tnej i zaznacz, �e podano cz�� dziesi�tn�
			if (eStat == GV_STAT_INIT)
				eStat = GV_STAT_DEC;
			 // Je�li poprzednio by� separator, zmie� tryb na pob�r cz�ci u�amkowej
			else if (eStat == GV_STAT_DELIM)
				eStat = GV_STAT_FRAC;
			 // Je�li poprzednio by� znak 'e', albo '-' lub '+' po 'e', zmie� tryb na pob�r wyk�adnika
			else if (eStat == GV_STAT_E_DELIM || eStat == GV_STAT_EXP_SIGN)
				eStat = GV_STAT_EXP;

			 // Pobieranie dziesietnej
			if (eStat == GV_STAT_DEC)
				dVal = dVal * 10.0 + (double)(cChar - 48);
			 // Pobieranie ulamkowej
			else if (eStat == GV_STAT_FRAC)
				dVal += (double)(cChar - 48) * (dFracFactor /= 10.0);
			 // Pobieranie wyk�adnika
			else if (eStat == GV_STAT_EXP)
				dExp = dExp * 10.0 + (double)(cChar - 48);
			else
				throw CalcError(CalcError::UNKNOWN, "wystapi� nieznany b��d; zg�o� programi�cie i poka� screenshot", uiCurrIdx + uiOffset, uiCurrIdx + uiOffset);

			uiCurrIdx++;
		}
		 // Pojawi� si� separator dziesi�tny - zmie� status na napotkanie separatora
		else if (cChar == '.' || cChar == ',')
		{
			// Mo�liwe tylko na pocz�tku (.56) jub po cz�ci dziesi�tnej (7.1, 56.)
			if (eStat != GV_STAT_INIT && eStat != GV_STAT_DEC)
				throw CalcError(CalcError::UNEXPECTED_SIGN, "niespodziewany znak \'" + szExp.substr(uiCurrIdx, 1) + "\'", uiCurrIdx + uiOffset, uiCurrIdx + uiOffset);

			eStat = GV_STAT_DELIM;
			uiCurrIdx++;
		}
		 // Pojawi� si� znak 'e'
		else if (cChar == 'e' || cChar == 'E')
		{
			 // Je�eli postawiono znaku 'e' po wpisaniu liczby
			if (eStat == GV_STAT_DEC || eStat == GV_STAT_DELIM || eStat == GV_STAT_FRAC)
			{
				 // Wpisanie samej kropki w cz�ci mantysty
				if (!bWasDigit)
					throw CalcError(CalcError::UNEXPECTED_SIGN, "niespodziewany znak \'" + szExp.substr(uiCurrIdx - 1, 1) + "\'", uiCurrIdx - 1 + uiOffset, uiCurrIdx - 1 + uiOffset);

				uiExpSave = uiCurrIdx;  // Zapisz pozycj� znaku 'e' - gdyby dalsza cz�� si� nie zgadza�a, e zostanie zinterpretowane jako sta�a Eulera i pozycja zostanie przywr�cona
				eStat = GV_STAT_E_DELIM;
			}
			 // Ju� podano znak "e" lub "e-" lub "e+" - zinterpretuj jako pocz�tek zmiennej "ee..." lub sta�� Eulera i znak dzia�ania
			else if (eStat == GV_STAT_E_DELIM || eStat == GV_STAT_EXP_SIGN)
			{
				uiCurrIdx = uiExpSave;  // przywr�� wska�nik na pozycj� pierwszego 'e'
				opInnerPointer = new CalcTreeValue(countVal(dVal, dExpSign * dExp));
				return bUseAddInv ? (CalcTree *)new CalcTreeAddInv(opInnerPointer) : opInnerPointer;
			}
			 // Podano znak 'e' ju� po wpisaniu wyk�adnika (5e5e) - zinterpretuj jako 5e5 * e - zwr�� liczb�
			else if (eStat == GV_STAT_EXP)
			{
				opInnerPointer = new CalcTreeValue(countVal(dVal, dExpSign * dExp));
				return bUseAddInv ? (CalcTree *)new CalcTreeAddInv(opInnerPointer) : opInnerPointer;
			}
			 // Podano znak 'e' na pocz�tku liczby - interpretacja jako pocz�tek zmiennej/funkcji - pobierz j�
			else
			{
				opInnerPointer = getVarFunct();
				return bUseAddInv ? (CalcTree *)new CalcTreeAddInv(opInnerPointer) : opInnerPointer;
			}

			uiCurrIdx++;
		}
		 // Pojawi� si� znak '+' lub '-' - sprawd�, czy to po znaku 'e'
		else if (cChar == '+' || cChar == '-')
		{
			if (eStat == GV_STAT_E_DELIM)
			{
				eStat = GV_STAT_EXP_SIGN;
				if (cChar == '-')
					dExpSign = -1.0;

				uiCurrIdx++;
			}
			 // Ju� podano znak wyk�adnika np. 5e-- - zinterpretuj jako 5*e +
			else if (eStat == GV_STAT_EXP_SIGN)
			{
				uiCurrIdx = uiExpSave;  // Przywr�� indeks 'e'
				opInnerPointer = new CalcTreeValue(countVal(dVal, dExpSign * dExp));
				return bUseAddInv ? (CalcTree *)new CalcTreeAddInv(opInnerPointer) : opInnerPointer;
			}
			 // Postawienie samej kropki
			else if (!bWasDigit)
				throw CalcError(CalcError::UNEXPECTED_SIGN, "niespodziewany znak \'" + szExp.substr(uiCurrIdx - 1, 1) + "\'", uiCurrIdx - 1 + uiOffset, uiCurrIdx - 1 + uiOffset);
			else
			{
				opInnerPointer = new CalcTreeValue(countVal(dVal, dExpSign * dExp));
				return bUseAddInv ? (CalcTree *)new CalcTreeAddInv(opInnerPointer) : opInnerPointer;
			}
		}
		else
		{
			if (eStat == GV_STAT_INIT)
			{
				 // Napotkano na wyra�enie w nawiasie zamiast liczby
				if (cChar == '(')
				{
					size_t uiIdxStart = uiCurrIdx;
					
					 // Wydziel wyra�enie w nawiasie
					std::string	szBracketExp = fetchBracketExp();

					 // Sprawd�, czy wyra�enie nie jest puste (ignoruj�c spacje)
					size_t	uiFirstAfterSpace = szBracketExp.find_first_not_of(WHITE_CHARS);
					size_t	uiLastBeforeSpace = szBracketExp.find_last_not_of(WHITE_CHARS);
					if (uiFirstAfterSpace == std::string::npos || szBracketExp.substr(uiFirstAfterSpace, uiLastBeforeSpace - uiFirstAfterSpace + 1).empty())
						throw CalcError(CalcError::EMPTY_BRACKETS, "puste nawiasy", uiIdxStart + uiOffset, uiCurrIdx - 1 + uiOffset);

					 // Pomi� spacje po nawiasie
					while (isspace(szExp[uiCurrIdx]))
						uiCurrIdx++;

					 // Oblicz wyra�enie w nawiasie, utw�rz drzewo i zwr��
					CalcParser	oBracketExpParser;
					CalcTree *	opBracketExp;
					oBracketExpParser.insertExp(szBracketExp, uiIdxStart + 1 + uiOffset);
								
					opBracketExp = oBracketExpParser.parse();

					// Zapobiegnim niepotrzebnemu zagnie�d�eniu nawias�w
					if (opBracketExp->getType() == NODE_BRACKETS)
						opInnerPointer = opBracketExp;
					else
						opInnerPointer = new CalcTreeBrackets(opBracketExp);

					return bUseAddInv ? (CalcTree *)new CalcTreeAddInv(opInnerPointer) : opInnerPointer;
				}
				 // Napotkano liter� - zmienna
				else if (isalpha(cChar))
				{
					opInnerPointer = getVarFunct();
					return bUseAddInv ? (CalcTree *)new CalcTreeAddInv(opInnerPointer) : opInnerPointer;
				}
				 // Napotkano na niedozwolony znak na pocz�tku liczby
				else
				{
					throw CalcError(CalcError::UNEXPECTED_SIGN, "niespodziewany znak \'" + szExp.substr(uiCurrIdx, 1) + "\'", uiCurrIdx + uiOffset, uiCurrIdx + uiOffset);
				}
			}
			 // Wpisanie np 5.6e lub 5.6e- lub 5.6e+ i pojawienie si� innego znaku - interpretacja jako 5.6 * sta�a Eulera
			else if (eStat == GV_STAT_E_DELIM || eStat == GV_STAT_EXP_SIGN)
			{
				uiCurrIdx = uiExpSave;  // przywr�� wska�nik na pozycj� 'e'
				opInnerPointer = new CalcTreeValue(countVal(dVal, dExpSign * dExp));
				return bUseAddInv ? (CalcTree *)new CalcTreeAddInv(opInnerPointer) : opInnerPointer;
			}
			 // Wpisanie samej kropki
			else if (!bWasDigit)
			{
				throw CalcError(CalcError::UNEXPECTED_SIGN, "niespodziewany znak \'" + szExp.substr(uiCurrIdx - 1, 1) + "\'", uiCurrIdx - 1 + uiOffset, uiCurrIdx - 1 + uiOffset);
			}
			else
			{
				opInnerPointer = new CalcTreeValue(countVal(dVal, dExpSign * dExp));
				return bUseAddInv ? (CalcTree *)new CalcTreeAddInv(opInnerPointer) : opInnerPointer;
			}
		}
	}
}

// Metoda wylicza liczb� z podanej mantysty i wyk�adnika
//------------------------------------------------------------------------
double CalcParser::countVal(double dMantissa, double dExp)
{
	double	dVal;
	size_t	uiEndIdx = uiCurrIdx;

	// Sprawd�, czy mantysta i wyk�adnik s� sko�czone
	if (!std::isfinite(dMantissa) || !std::isfinite(dExp))
		throw CalcError(CalcError::PASRE_VAL_OUT_OF_RANGE, "przekroczono maksimum zakresu", uiIdxSave + uiOffset, uiEndIdx + uiOffset);

	// Policz liczb� i sprawd�, czy nie przekroczono zakresu
	feclearexcept(FE_ALL_EXCEPT);
	dVal = dMantissa * pow(10.0, dExp);
	if (fetestexcept(FE_OVERFLOW | FE_UNDERFLOW))
	{
		while (isspace(szExp[--uiEndIdx])) {}

		if (fetestexcept(FE_OVERFLOW))
			throw CalcError(CalcError::PASRE_VAL_OUT_OF_RANGE, "przekroczono maksimum zakresu", uiIdxSave + uiOffset, uiEndIdx + uiOffset);
		else if (fetestexcept(FE_UNDERFLOW))
			throw CalcError(CalcError::PASRE_VAL_OUT_OF_RANGE, "przekroczono minimum zakresu", uiIdxSave + uiOffset, uiEndIdx + uiOffset);
	}
	return dVal;
}

// Metoda przeskakuje przez wyra�enie w nawiasie, uwzgl�dniaj�c
// zagnie�d�enie. Je�li napotka po drodze koniec, wyrzuca wyj�tek
// o niespodziewanym ko�cu. Przesuwa indeks na pierwsz� pozycj� po nawiasie
// zamykaj�cym
//-------------------------------------------------------------------------
void CalcParser::skipBracketExp()
{
	if (szExp[uiCurrIdx] != '(')
		throw std::runtime_error("fetchBracketExp(): niepoprawny pierwszy znak");

	uiCurrIdx++;
	size_t			uiIdxStart = uiCurrIdx;
	unsigned short	usLvl = 1;

	 // Szukaj ko�ca wyra�enia w nawiasie
	while (true)
	{
		switch (szExp[uiCurrIdx])
		{
		case '(':
			usLvl++;
			break;

		case ')':
			usLvl--;

			 // Domkni�to wyra�enie
			if (!usLvl)
			{
				uiCurrIdx++;
				return;
			}

			break;

		 // Napotkano na koniec przed domkni�ciem nawiasu
		case 0:
			throw CalcError(CalcError::NOT_CLOSED_BRACKET, "niedomkni�te nawiasy", uiIdxStart - 1 + uiOffset, uiCurrIdx - 1 + uiOffset);
			break;
		}
		uiCurrIdx++;
	}
}

// Metoda pobiera wyra�enie w nawiasie, uwzgl�dniaj�c zagnie�d�enie.
// Je�li napotka po drodze koniec, wyrzuca wyj�tek o niespodziewanym ko�cu
// Przesuwa indeks na pierwsz� pozycj� po nawiasie zamykaj�cym
//------------------------------------------------------------------------
std::string CalcParser::fetchBracketExp()
{
	size_t uiIdxStart = uiCurrIdx + 1;
	skipBracketExp();
	return szExp.substr(uiIdxStart, uiCurrIdx - 1 - uiIdxStart);
}

// Metoda pobiera z wyra�enia zmienn�/funkcj� i przesuwa indeks.
// Zwraca odpowieni utworzony obiekt
//------------------------------------------------------------------------
CalcTree * CalcParser::getVarFunct()
{
	if (!isalpha(szExp[uiCurrIdx]))
		throw std::runtime_error("getVarFunct: pierwszy znak nie jest liter�");

	std::string		szVarName{""};
	size_t			uiEndIdx;		// Indeks znaku po ko�cu zmiennej

	 // Pobierz nazw� zmiennej
	uiEndIdx = szExp.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_", uiCurrIdx);
	if (uiEndIdx == std::string::npos)
		uiEndIdx = szExp.length();

	szVarName = szExp.substr(uiCurrIdx, uiEndIdx - uiCurrIdx);

	 // Pomi� spacje
	uiCurrIdx = (uiEndIdx != szExp.length()) ? szExp.find_first_not_of(WHITE_CHARS, uiEndIdx) : uiEndIdx;
	if (uiCurrIdx == std::string::npos)
		uiCurrIdx = szExp.length();

	 // Podano funkcj�
	if (szExp[uiCurrIdx] == '(')
	{
		// Sprawd�, czy taka funkcja istnieje. Je�li nie, ale istnieje zmienna, potraktuj jako zmienna * (co�)
		if (!gFunctEngine.exists(szVarName) && gVarEngine.exists(szVarName))
			return new CalcTreeVar(szVarName);

		uiCurrIdx++;

		size_t					uiParamBegIdx;									// Indeks pocz�tku parametru
		size_t					uiAllParamsBegIdx = uiParamBegIdx = uiCurrIdx;	// Indeks pocz�tku wszystkich parametr�w
		std::vector<CalcTree*>	opParamsVector;
		std::string				szParam;
		bool					bEndReached = false;		 // Napotkano na nawias zamykaj�cy

		 // Jed��e przez znaki i pobieraj parametry
		while (true)
		{
			 // Je�li napotkano na koniec, poinformuj w nast�pnej instrukcji
			if (szExp[uiCurrIdx] == ')')
				bEndReached = true;

			 // Napotkano przecinek lub �rednik - parsuj parametr i jed��e dalej; a je�li koniec - zako�cz pobieranie, i je�li podano, parsuj ostatni (jedyny) parametr
			if (bEndReached || szExp[uiCurrIdx] == ',' || szExp[uiCurrIdx] == ';')
			{
				 // Wydziel go
				szParam = szExp.substr(uiParamBegIdx, uiCurrIdx - uiParamBegIdx);

				 // Sprawd�, czy parametr nie jest pusty (ignoruj�c spacje)
				size_t	uiFirstAfterSpace = szParam.find_first_not_of(WHITE_CHARS);
				size_t	uiLastBeforeSpace = szParam.find_last_not_of(WHITE_CHARS);

				if (uiFirstAfterSpace == std::string::npos || szParam.substr(uiFirstAfterSpace, uiLastBeforeSpace - uiFirstAfterSpace + 1).empty())
				{
					 // Je�li pusty jest parametr przed przecinkiem, np. root(, 12), lub ostatni przed nawiasem, np. root (pi,), zwr�� b��d
					if (!bEndReached || !opParamsVector.empty())
						throw CalcError(CalcError::EMPTY_PARAM, "pusty parametr funkcji", uiParamBegIdx + uiOffset, uiCurrIdx + uiOffset);
					 // Zezw�l na funkcj� bez parametr�w
					else
					{
						uiCurrIdx++;
						break;
					}
				}

				 // Sparsuj parametr i dodaj do tablicy
				CalcParser oParser;
				oParser.insertExp(szParam, uiOffset + uiParamBegIdx);
				opParamsVector.push_back(oParser.parse());

				uiCurrIdx++;

				 // Je�li ju� koniec, przerwij pobieranie parametr�w
				if (bEndReached)
					break;
				 // W przeciwnym wypadku rozpocznij pobieranie nowego
				else
					uiParamBegIdx = uiCurrIdx;		// zmie� pocz�tek parametru na nowy
			}
			 // Wyra�enie w nawiasie w funkcji - przeskocz
			else if (szExp[uiCurrIdx] == '(')
				skipBracketExp();
			 // Nie domkni�to nawias�w funkcji przed ko�cem wyra�enia
			else if (szExp[uiCurrIdx] == 0)
				throw CalcError(CalcError::NOT_CLOSED_BRACKET, "niedomkni�te nawiasy", uiAllParamsBegIdx - 1 + uiOffset, uiCurrIdx - 1 + uiOffset);
			 // Inny znak - pomi�
			else
				uiCurrIdx++;
		}

		 // Pomi� spacje
		if (uiCurrIdx != szExp.length())
			uiCurrIdx = szExp.find_first_not_of(WHITE_CHARS, uiCurrIdx);
		if (uiCurrIdx == std::string::npos)
			uiCurrIdx = szExp.length();

		return new CalcTreeFunct(szVarName, opParamsVector);
	}
	 // Podano zmienn�
	else
	{
		return new CalcTreeVar(szVarName);
	}
}

// Metoda sprawdza, czy przed wyra�eniem nie podano znaku i przesuwa
// indeks. Zwraca true, je�li nale�y utworzy� minus unarny
//------------------------------------------------------------------------
bool CalcParser::checkSign()
{
	 // Zabezpiecz przed pobraniem pozazakrazowego znaku
	if (uiCurrIdx >= szExp.length())
		return false;

	bool bReturn = false;
	bool bRun = true;

	while (bRun)
	{
		switch (szExp[uiCurrIdx])
		{
		case '-':
			bReturn = !bReturn;
		CASE_WHITE_C:
		case '+':
			uiCurrIdx++;
			break;
		default:
			bRun = false;
			break;
		}
	}

	return bReturn;
}

// Metoda parsuj� nast�pny operator i przesuwa wska�nik. Zwraca jego
// rodzaj
//------------------------------------------------------------------------
CALC_NODE_TYPE CalcParser::getOp()
{
	 // Je�li napotkano koniec, zg�o�� niespodziewany koniec wyra�enia
	if (uiCurrIdx >= szExp.length())
		throw CalcError(CalcError::UNEXPECTED_END, "niespodziewany koniec wyra�enia", uiCurrIdx + uiOffset, uiCurrIdx + uiOffset);

	CALC_NODE_TYPE eNode = NODE_EMPTY;

	while (true)
	{
		switch (szExp[uiCurrIdx])
		{
		 // Opuszczaj spacje
		CASE_WHITE_C:
			uiCurrIdx++;
			break;

		 // Dodawanie
		case '+':
			if (eNode == NODE_EMPTY)
				eNode = NODE_SUM;
			else
				return eNode;

			uiCurrIdx++;
			break;

		 // Odejmowanie
		case '-':
			if (eNode == NODE_EMPTY)
				eNode = NODE_DIFF;
			else
				return eNode;

			uiCurrIdx++;
			break;

		 // Mno�enie
		case '*':
			 // Mno�enie *
			if (eNode == NODE_EMPTY)
				eNode = NODE_PRODUCT;
			 // Pot�gowanie - wersja **
			else if (eNode == NODE_PRODUCT)
				eNode = NODE_POW;
			else
				return eNode;

			uiCurrIdx++;
			break;

		 // Dzielenie
		case '/':
			if (eNode == NODE_EMPTY)
				eNode = NODE_QUOT;
			else
				return eNode;

			uiCurrIdx++;
			break;

		 // Silnia
		case '!':
			if (eNode == NODE_EMPTY)
				eNode = NODE_FACTOR;
			else
				return eNode;

			uiCurrIdx++;
			break;

		 // Pot�gowanie
		case '^':
			if (eNode == NODE_EMPTY)
				eNode = NODE_POW;
			else
				return eNode;

			uiCurrIdx++;
			break;

		 // Modulo
		case '%':
			if (eNode == NODE_EMPTY)
				eNode = NODE_MOD;
			else
				return eNode;

			uiCurrIdx++;
			break;

		 // Napotkanie potencjalnego pocz�tku poprawnego wyra�enia
		case '(':
		CASE_ALPHA_C:
		CASE_NUM_C:
		case '.':
		case ',':
			 // Je�li napotkano ju� na znak, zwr�� go. Je�li nie, zwr�� domy�lny iloczyn (5pi <=> 5*pi, 6(...) <=> 6*(...))
			if (eNode != NODE_EMPTY)
				return eNode;
			else
				return NODE_PRODUCT;
			break;

		 // Inny znak
		default:
			if (eNode == NODE_EMPTY)
			{
				throw CalcError(CalcError::UNEXPECTED_SIGN, "niespodziewany znak \'" + szExp.substr(uiCurrIdx, 1) + "\'", uiCurrIdx + uiOffset, uiCurrIdx + uiOffset);
				/*bool bWasError = false;
				size_t uiIdxMem = uiCurrIdx;
				CalcTree * opDelete = nullptr;
				CalcTree * opSave;
				
				// Zobacz, czy nast�pne nie jest poprawne wyra�enie - liczba, wyra�enie w nawiasie, funkcja, zmienna
				try {
					opDelete = getVal(opSave);
				} catch (CalcError & error) {
					(void)error;
					bWasError = true;
				}

				uiCurrIdx = uiIdxMem;

				 // Napotkano niepoprawny znak
				if (bWasError)
					throw CalcError(CalcError::UNEXPECTED_SIGN, "niespodziewany znak \'" + szExp.substr(uiCurrIdx, 1) + "\'", uiCurrIdx + uiOffset, uiCurrIdx + uiOffset);
				 // Zabrak�o znaku mi�dzy liczbami
				else
				{
					delete opDelete;
					throw CalcError(CalcError::EXPECTED_SIGN, "spodziewany znak dzia�ania", uiCurrIdx + uiOffset, uiCurrIdx + uiOffset);
				}*/
			}
			 // Napotkano na inny znak po sczytaniu dzia�ania - przerwij
			else
			{
				return eNode;
			}
			break;
		}
	}
}

// Metoda wstawia do parsera nowe wyra�enie
//------------------------------------------------------------------------
void CalcParser::insertExp(const std::string & szNewExp)
{
	szExp = szNewExp;
	uiCurrIdx = 0;
	uiOffset = 0;
}

// Metoda zwraca sta�� referencj� do obecnego wyra�enia
//-----------------------------------------------------------------------
const std::string & CalcParser::getExp()
{
	return szExp;
}

// Metoda parsuje wyra�enie wstawione za pomoc� insertExp, tworzy drzewo
// CalcTree i zwraca wska�nik do niego
//-----------------------------------------------------------------------
CalcTree * CalcParser::parse()
{
	size_t			uiLength = szExp.length();
	CALC_NODE_TYPE	eOpType;

	CalcTree *		opTree = nullptr;
	CalcTree *		opCurrentNode = nullptr;
	CalcTree *		opCurrentPointer = nullptr;
	CalcTree *		opNewNode = nullptr;
	CalcTree *		opParentNode = nullptr;

	 // Sprawd�, czy wyra�enie nie jest za d�ugie (zbyt d�ugie powoduj� prze�adownie stosu)
	if (uiLength > MAX_EXP_LENGTH)
		throw CalcError(CalcError::TOO_LONG, std::string("za d�ugie wyra�enie (") + std::to_string(uiLength) + std::string(" znak�w; max ") + std::to_string(MAX_EXP_LENGTH) + std::string(")"), 0, uiLength - 1);

	 // Przechwytuj b��dy - w razie czego usu� zaj�t� pami�� i wyrzu� dalej
	try
	{
		 // Pobierz pierwsz� warto��
		opTree = getVal(opCurrentPointer);
		opCurrentNode = opCurrentPointer;

		 // Powtarzaj a� do ko�ca wyra�enia
		while (uiCurrIdx != uiLength)
		{
			 // Pobierz operator i sprawd� rodzaj
			eOpType = getOp();

			switch (CALC_NODE_DATA[eOpType].eOpNaryType)
			{
			 // Operator unarny
			case OP_UNARY:

				 // ��czny lewostronnie - przesuwaj w g�r� drzewa opCurrentNode, a� w�ze�-rodzic jest o NI�SZYM priorytecie lub jest pocz�tkiem
				if (CALC_NODE_DATA[eOpType].eOpAssoc == OP_ASSOC_LEFT)
					while (opCurrentNode->getParent() != nullptr && opCurrentNode->getParent()->getOpPrecedence() > CALC_NODE_DATA[eOpType].usOpPrec)
						opCurrentNode = opCurrentNode->getParent();
				 // ��czny prawostronnie - przesuwaj w g�r� drzewa opCurrentNode, a� w�ze�-rodzic jest o NI�SZYM LUB R�WNYM priorytecie lub jest pocz�tkiem
				else
					while (opCurrentNode->getParent() != nullptr && opCurrentNode->getParent()->getOpPrecedence() >= CALC_NODE_DATA[eOpType].usOpPrec)
						opCurrentNode = opCurrentNode->getParent();
				 
				 // Od��cz w�ze� od rodzica
				opParentNode = opCurrentNode->getParent();
				if (opParentNode != nullptr)
					opCurrentNode = opParentNode->detachNode();
				else
					opCurrentNode = opTree;
				
				 // Przy��cz do rodzica w�ze� zagnie�d�ony w nowym, zale�nym od operatora
				switch (eOpType)
				{
				case NODE_FACTOR:
					opNewNode = new CalcTreeFactor(opCurrentNode);
					break;
				}

				if (opParentNode != nullptr)
					opParentNode->appendNode(opNewNode);
				else
					opTree = opNewNode;

				opNewNode = nullptr;
				break;

			 // Operator binarny
			case OP_BINARY:

				 // Pobierz nast�pn� warto��
				opNewNode = getVal(opCurrentPointer);

				 // ��czny lewostronnie - przesuwaj w g�r� drzewa opCurrentNode, a� w�ze�-rodzic jest o NI�SZYM priorytecie lub jest pocz�tkiem
				if (CALC_NODE_DATA[eOpType].eOpAssoc == OP_ASSOC_LEFT)
					while (opCurrentNode->getParent() != nullptr && opCurrentNode->getParent()->getOpPrecedence() > CALC_NODE_DATA[eOpType].usOpPrec)
						opCurrentNode = opCurrentNode->getParent();
				 // ��czny prawostronnie - przesuwaj w g�r� drzewa opCurrentNode, a� w�ze�-rodzic jest o NI�SZYM LUB R�WNYM priorytecie lub jest pocz�tkiem
				else
					while (opCurrentNode->getParent() != nullptr && opCurrentNode->getParent()->getOpPrecedence() >= CALC_NODE_DATA[eOpType].usOpPrec)
						opCurrentNode = opCurrentNode->getParent();

				 // Od��cz w�ze� od rodzica
				opParentNode = opCurrentNode->getParent();
				if (opParentNode != nullptr)
					opCurrentNode = opParentNode->detachNode();
				else
					opCurrentNode = opTree;

				 // Przy��cz do rodzica w�z�y zagnie�d�one w nowym, zela�nym od operatora
				switch (eOpType)
				{
				 // Suma
				case NODE_SUM:
					opNewNode = (new CalcTreeSum(opCurrentNode, opNewNode));
					break;
				
				 // R�nica
				case NODE_DIFF:
					opNewNode = (new CalcTreeDiff(opCurrentNode, opNewNode));
					break;
				
				 // Iloczyn
				case NODE_PRODUCT:
					opNewNode = (new CalcTreeProduct(opCurrentNode, opNewNode));
					break;

				 // Iloraz
				case NODE_QUOT:
					opNewNode = (new CalcTreeQuot(opCurrentNode, opNewNode));
					break;

				 // Pot�ga
				case NODE_POW:
					opNewNode = (new CalcTreePow(opCurrentNode, opNewNode));
					break;
				
				 // Modulo
				case NODE_MOD:
					opNewNode = (new CalcTreeMod(opCurrentNode, opNewNode));
					break;
				}

				if (opParentNode != nullptr)
					opParentNode->appendNode(opNewNode);
				else
					opTree = opNewNode;

				 // Przesu� wska�nik do ostatniego w�z�a na obiekt pod nowym w�z�em
				opCurrentNode = opCurrentPointer;
				opNewNode = nullptr;

				break;
			}
		}
		return opTree;
	}
	catch (CalcError & error)
	{
		(void)error;
		delete opNewNode;
		delete opTree;
		throw;
	}
	return nullptr;
}