#include "stdafx.h"
#include "class_calcparser.h"
#include "newconio.h"

static int num = 0;

// Metoda wstawia do parsera nowe wyra¿enie z przesuniêciem
//------------------------------------------------------------------------
void CalcParser::insertExp(const std::string & szNewExp, size_t uiNewOffset)
{
	szExp = szNewExp;
	uiCurrIdx = 0;
	uiOffset = uiNewOffset;
}

// Metoda parsuje nastêpn¹ liczbê/wyra¿enie w nawiasie/zmienn¹/funkcjê
// z wyra¿enia i przesuwa wskaŸnik. Uwzglêdnia znak przed liczb¹.
// Tworzy odpowiedni obiekt CalcTree i zwraca wskaŸnik. Ustawia wartoœæ
// opInnerPointer na wskaŸnik do obiektu z pominiêciem CalcTreeAddInv
//------------------------------------------------------------------------
CalcTree * CalcParser::getVal(CalcTree *& opInnerPointer)
{
	double				dVal = 0.0;
	double				dExp = 0.0;
	double				dExpSign = 1.0;
	unsigned char		cChar;
	size_t				uiExpSave;					// zapamiêtana pozycja pocz¹tku wyk³adnika
	bool				bWasDigit = false;			// czy pojawi³a siê jakaœ cyfra?
	double				dFracFactor = 1.0;			// czynnik czêœci dziesiêtnej
	CALC_GETVAL_STATUS	eStat = GV_STAT_INIT;
	bool				bUseAddInv = checkSign();	// sprawdŸ, czy wystêpuj¹ znaki unarne (typu -5, +5, --+-5)
	
	uiIdxSave = uiCurrIdx;	// U¿ywaj pola "zapisany indeks" jako pocz¹tku liczby

	 // Jeœli napotkano koniec, zg³oœæ niespodziewany koniec wyra¿enia - musi byæ po checkSign()
	if (uiCurrIdx >= szExp.length())
		throw CalcError(CalcError::UNEXPECTED_END, "niespodziewany koniec wyra¿enia", uiCurrIdx + uiOffset, uiCurrIdx + uiOffset);

	 // Wczytaj kolejne znaki
	while (true)
	{
		cChar = szExp[uiCurrIdx];

		 // Jeœli spacje przed liczb¹, pomiñ; jeœli po, przeskocz i zakoñcz pobieranie
		if (isspace(cChar))
		{
			if (eStat != GV_STAT_INIT && eStat != GV_STAT_EXIT_SPACE)
			{
				 // Wpisanie samej kropki
				if (!bWasDigit)
					throw CalcError(CalcError::UNEXPECTED_SIGN, "niespodziewany znak \'" + szExp.substr(uiCurrIdx - 1, 1) + "\'", uiCurrIdx - 1 + uiOffset, uiCurrIdx - 1 + uiOffset);
				 // Napisanie np 5.6e, 5.6e- lub 5.6e+ - interpretacja jako 5.6 * sta³a Eulera i ewentualnei znak dzia³ania - przerwij pobieranie
				else if (eStat == GV_STAT_E_DELIM || eStat == GV_STAT_EXP_SIGN)
				{
					uiCurrIdx = uiExpSave;  // przywróæ wskaŸnik na pozycjê 'e'
					opInnerPointer = new CalcTreeValue(countVal(dVal, dExpSign * dExp));
					return bUseAddInv ? (CalcTree *)new CalcTreeAddInv(opInnerPointer) : opInnerPointer;
				}

				eStat = GV_STAT_EXIT_SPACE;
			}

			uiCurrIdx++;
			if (eStat == GV_STAT_INIT)
				uiIdxSave++;
		}
		 // Pierwszy niebia³y znak po przeskakiwaniu spacji po liczbie - przerwij
		else if (eStat == GV_STAT_EXIT_SPACE)
		{
			opInnerPointer = new CalcTreeValue(countVal(dVal, dExpSign * dExp));
			return bUseAddInv ? (CalcTree *)new CalcTreeAddInv(opInnerPointer) : opInnerPointer;
		}
		 // Pojawi³a siê cyfra
		else if (cChar >= 48 && cChar <= 57)
		{
			bWasDigit = true;

			 // Jeœli napotkano cyfrê na pocz¹tku (lub po spacjach) zmieñ status na pobór czêœci dziesiêtnej i zaznacz, ¿e podano czêœæ dziesiêtn¹
			if (eStat == GV_STAT_INIT)
				eStat = GV_STAT_DEC;
			 // Jeœli poprzednio by³ separator, zmieñ tryb na pobór czêœci u³amkowej
			else if (eStat == GV_STAT_DELIM)
				eStat = GV_STAT_FRAC;
			 // Jeœli poprzednio by³ znak 'e', albo '-' lub '+' po 'e', zmieñ tryb na pobór wyk³adnika
			else if (eStat == GV_STAT_E_DELIM || eStat == GV_STAT_EXP_SIGN)
				eStat = GV_STAT_EXP;

			 // Pobieranie dziesietnej
			if (eStat == GV_STAT_DEC)
				dVal = dVal * 10.0 + (double)(cChar - 48);
			 // Pobieranie ulamkowej
			else if (eStat == GV_STAT_FRAC)
				dVal += (double)(cChar - 48) * (dFracFactor /= 10.0);
			 // Pobieranie wyk³adnika
			else if (eStat == GV_STAT_EXP)
				dExp = dExp * 10.0 + (double)(cChar - 48);
			else
				throw CalcError(CalcError::UNKNOWN, "wystapi³ nieznany b³¹d; zg³oœ programiœcie i poka¿ screenshot", uiCurrIdx + uiOffset, uiCurrIdx + uiOffset);

			uiCurrIdx++;
		}
		 // Pojawi³ siê separator dziesiêtny - zmieñ status na napotkanie separatora
		else if (cChar == '.' || cChar == ',')
		{
			// Mo¿liwe tylko na pocz¹tku (.56) jub po czêœci dziesiêtnej (7.1, 56.)
			if (eStat != GV_STAT_INIT && eStat != GV_STAT_DEC)
				throw CalcError(CalcError::UNEXPECTED_SIGN, "niespodziewany znak \'" + szExp.substr(uiCurrIdx, 1) + "\'", uiCurrIdx + uiOffset, uiCurrIdx + uiOffset);

			eStat = GV_STAT_DELIM;
			uiCurrIdx++;
		}
		 // Pojawi³ siê znak 'e'
		else if (cChar == 'e' || cChar == 'E')
		{
			 // Je¿eli postawiono znaku 'e' po wpisaniu liczby
			if (eStat == GV_STAT_DEC || eStat == GV_STAT_DELIM || eStat == GV_STAT_FRAC)
			{
				 // Wpisanie samej kropki w czêœci mantysty
				if (!bWasDigit)
					throw CalcError(CalcError::UNEXPECTED_SIGN, "niespodziewany znak \'" + szExp.substr(uiCurrIdx - 1, 1) + "\'", uiCurrIdx - 1 + uiOffset, uiCurrIdx - 1 + uiOffset);

				uiExpSave = uiCurrIdx;  // Zapisz pozycjê znaku 'e' - gdyby dalsza czêœæ siê nie zgadza³a, e zostanie zinterpretowane jako sta³a Eulera i pozycja zostanie przywrócona
				eStat = GV_STAT_E_DELIM;
			}
			 // Ju¿ podano znak "e" lub "e-" lub "e+" - zinterpretuj jako pocz¹tek zmiennej "ee..." lub sta³¹ Eulera i znak dzia³ania
			else if (eStat == GV_STAT_E_DELIM || eStat == GV_STAT_EXP_SIGN)
			{
				uiCurrIdx = uiExpSave;  // przywróæ wskaŸnik na pozycjê pierwszego 'e'
				opInnerPointer = new CalcTreeValue(countVal(dVal, dExpSign * dExp));
				return bUseAddInv ? (CalcTree *)new CalcTreeAddInv(opInnerPointer) : opInnerPointer;
			}
			 // Podano znak 'e' ju¿ po wpisaniu wyk³adnika (5e5e) - zinterpretuj jako 5e5 * e - zwróæ liczbê
			else if (eStat == GV_STAT_EXP)
			{
				opInnerPointer = new CalcTreeValue(countVal(dVal, dExpSign * dExp));
				return bUseAddInv ? (CalcTree *)new CalcTreeAddInv(opInnerPointer) : opInnerPointer;
			}
			 // Podano znak 'e' na pocz¹tku liczby - interpretacja jako pocz¹tek zmiennej/funkcji - pobierz j¹
			else
			{
				opInnerPointer = getVarFunct();
				return bUseAddInv ? (CalcTree *)new CalcTreeAddInv(opInnerPointer) : opInnerPointer;
			}

			uiCurrIdx++;
		}
		 // Pojawi³ siê znak '+' lub '-' - sprawdŸ, czy to po znaku 'e'
		else if (cChar == '+' || cChar == '-')
		{
			if (eStat == GV_STAT_E_DELIM)
			{
				eStat = GV_STAT_EXP_SIGN;
				if (cChar == '-')
					dExpSign = -1.0;

				uiCurrIdx++;
			}
			 // Ju¿ podano znak wyk³adnika np. 5e-- - zinterpretuj jako 5*e +
			else if (eStat == GV_STAT_EXP_SIGN)
			{
				uiCurrIdx = uiExpSave;  // Przywróæ indeks 'e'
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
				 // Napotkano na wyra¿enie w nawiasie zamiast liczby
				if (cChar == '(')
				{
					size_t uiIdxStart = uiCurrIdx;
					
					 // Wydziel wyra¿enie w nawiasie
					std::string	szBracketExp = fetchBracketExp();

					 // SprawdŸ, czy wyra¿enie nie jest puste (ignoruj¹c spacje)
					size_t	uiFirstAfterSpace = szBracketExp.find_first_not_of(WHITE_CHARS);
					size_t	uiLastBeforeSpace = szBracketExp.find_last_not_of(WHITE_CHARS);
					if (uiFirstAfterSpace == std::string::npos || szBracketExp.substr(uiFirstAfterSpace, uiLastBeforeSpace - uiFirstAfterSpace + 1).empty())
						throw CalcError(CalcError::EMPTY_BRACKETS, "puste nawiasy", uiIdxStart + uiOffset, uiCurrIdx - 1 + uiOffset);

					 // Pomiñ spacje po nawiasie
					while (isspace(szExp[uiCurrIdx]))
						uiCurrIdx++;

					 // Oblicz wyra¿enie w nawiasie, utwórz drzewo i zwróæ
					CalcParser	oBracketExpParser;
					CalcTree *	opBracketExp;
					oBracketExpParser.insertExp(szBracketExp, uiIdxStart + 1 + uiOffset);
								
					opBracketExp = oBracketExpParser.parse();

					// Zapobiegnim niepotrzebnemu zagnie¿d¿eniu nawiasów
					if (opBracketExp->getType() == NODE_BRACKETS)
						opInnerPointer = opBracketExp;
					else
						opInnerPointer = new CalcTreeBrackets(opBracketExp);

					return bUseAddInv ? (CalcTree *)new CalcTreeAddInv(opInnerPointer) : opInnerPointer;
				}
				 // Napotkano literê - zmienna
				else if (isalpha(cChar))
				{
					opInnerPointer = getVarFunct();
					return bUseAddInv ? (CalcTree *)new CalcTreeAddInv(opInnerPointer) : opInnerPointer;
				}
				 // Napotkano na niedozwolony znak na pocz¹tku liczby
				else
				{
					throw CalcError(CalcError::UNEXPECTED_SIGN, "niespodziewany znak \'" + szExp.substr(uiCurrIdx, 1) + "\'", uiCurrIdx + uiOffset, uiCurrIdx + uiOffset);
				}
			}
			 // Wpisanie np 5.6e lub 5.6e- lub 5.6e+ i pojawienie siê innego znaku - interpretacja jako 5.6 * sta³a Eulera
			else if (eStat == GV_STAT_E_DELIM || eStat == GV_STAT_EXP_SIGN)
			{
				uiCurrIdx = uiExpSave;  // przywróæ wskaŸnik na pozycjê 'e'
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

// Metoda wylicza liczbê z podanej mantysty i wyk³adnika
//------------------------------------------------------------------------
double CalcParser::countVal(double dMantissa, double dExp)
{
	double	dVal;
	size_t	uiEndIdx = uiCurrIdx;

	// SprawdŸ, czy mantysta i wyk³adnik s¹ skoñczone
	if (!std::isfinite(dMantissa) || !std::isfinite(dExp))
		throw CalcError(CalcError::PASRE_VAL_OUT_OF_RANGE, "przekroczono maksimum zakresu", uiIdxSave + uiOffset, uiEndIdx + uiOffset);

	// Policz liczbê i sprawdŸ, czy nie przekroczono zakresu
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

// Metoda przeskakuje przez wyra¿enie w nawiasie, uwzglêdniaj¹c
// zagnie¿d¿enie. Jeœli napotka po drodze koniec, wyrzuca wyj¹tek
// o niespodziewanym koñcu. Przesuwa indeks na pierwsz¹ pozycjê po nawiasie
// zamykaj¹cym
//-------------------------------------------------------------------------
void CalcParser::skipBracketExp()
{
	if (szExp[uiCurrIdx] != '(')
		throw std::runtime_error("fetchBracketExp(): niepoprawny pierwszy znak");

	uiCurrIdx++;
	size_t			uiIdxStart = uiCurrIdx;
	unsigned short	usLvl = 1;

	 // Szukaj koñca wyra¿enia w nawiasie
	while (true)
	{
		switch (szExp[uiCurrIdx])
		{
		case '(':
			usLvl++;
			break;

		case ')':
			usLvl--;

			 // Domkniêto wyra¿enie
			if (!usLvl)
			{
				uiCurrIdx++;
				return;
			}

			break;

		 // Napotkano na koniec przed domkniêciem nawiasu
		case 0:
			throw CalcError(CalcError::NOT_CLOSED_BRACKET, "niedomkniête nawiasy", uiIdxStart - 1 + uiOffset, uiCurrIdx - 1 + uiOffset);
			break;
		}
		uiCurrIdx++;
	}
}

// Metoda pobiera wyra¿enie w nawiasie, uwzglêdniaj¹c zagnie¿d¿enie.
// Jeœli napotka po drodze koniec, wyrzuca wyj¹tek o niespodziewanym koñcu
// Przesuwa indeks na pierwsz¹ pozycjê po nawiasie zamykaj¹cym
//------------------------------------------------------------------------
std::string CalcParser::fetchBracketExp()
{
	size_t uiIdxStart = uiCurrIdx + 1;
	skipBracketExp();
	return szExp.substr(uiIdxStart, uiCurrIdx - 1 - uiIdxStart);
}

// Metoda pobiera z wyra¿enia zmienn¹/funkcjê i przesuwa indeks.
// Zwraca odpowieni utworzony obiekt
//------------------------------------------------------------------------
CalcTree * CalcParser::getVarFunct()
{
	if (!isalpha(szExp[uiCurrIdx]))
		throw std::runtime_error("getVarFunct: pierwszy znak nie jest liter¹");

	std::string		szVarName{""};
	size_t			uiEndIdx;		// Indeks znaku po koñcu zmiennej

	 // Pobierz nazwê zmiennej
	uiEndIdx = szExp.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_", uiCurrIdx);
	if (uiEndIdx == std::string::npos)
		uiEndIdx = szExp.length();

	szVarName = szExp.substr(uiCurrIdx, uiEndIdx - uiCurrIdx);

	 // Pomiñ spacje
	uiCurrIdx = (uiEndIdx != szExp.length()) ? szExp.find_first_not_of(WHITE_CHARS, uiEndIdx) : uiEndIdx;
	if (uiCurrIdx == std::string::npos)
		uiCurrIdx = szExp.length();

	 // Podano funkcjê
	if (szExp[uiCurrIdx] == '(')
	{
		// SprawdŸ, czy taka funkcja istnieje. Jeœli nie, ale istnieje zmienna, potraktuj jako zmienna * (coœ)
		if (!gFunctEngine.exists(szVarName) && gVarEngine.exists(szVarName))
			return new CalcTreeVar(szVarName);

		uiCurrIdx++;

		size_t					uiParamBegIdx;									// Indeks pocz¹tku parametru
		size_t					uiAllParamsBegIdx = uiParamBegIdx = uiCurrIdx;	// Indeks pocz¹tku wszystkich parametrów
		std::vector<CalcTree*>	opParamsVector;
		std::string				szParam;
		bool					bEndReached = false;		 // Napotkano na nawias zamykaj¹cy

		 // JedŸ¿e przez znaki i pobieraj parametry
		while (true)
		{
			 // Jeœli napotkano na koniec, poinformuj w nastêpnej instrukcji
			if (szExp[uiCurrIdx] == ')')
				bEndReached = true;

			 // Napotkano przecinek lub œrednik - parsuj parametr i jedŸ¿e dalej; a jeœli koniec - zakoñcz pobieranie, i jeœli podano, parsuj ostatni (jedyny) parametr
			if (bEndReached || szExp[uiCurrIdx] == ',' || szExp[uiCurrIdx] == ';')
			{
				 // Wydziel go
				szParam = szExp.substr(uiParamBegIdx, uiCurrIdx - uiParamBegIdx);

				 // SprawdŸ, czy parametr nie jest pusty (ignoruj¹c spacje)
				size_t	uiFirstAfterSpace = szParam.find_first_not_of(WHITE_CHARS);
				size_t	uiLastBeforeSpace = szParam.find_last_not_of(WHITE_CHARS);

				if (uiFirstAfterSpace == std::string::npos || szParam.substr(uiFirstAfterSpace, uiLastBeforeSpace - uiFirstAfterSpace + 1).empty())
				{
					 // Jeœli pusty jest parametr przed przecinkiem, np. root(, 12), lub ostatni przed nawiasem, np. root (pi,), zwróæ b³¹d
					if (!bEndReached || !opParamsVector.empty())
						throw CalcError(CalcError::EMPTY_PARAM, "pusty parametr funkcji", uiParamBegIdx + uiOffset, uiCurrIdx + uiOffset);
					 // Zezwól na funkcjê bez parametrów
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

				 // Jeœli ju¿ koniec, przerwij pobieranie parametrów
				if (bEndReached)
					break;
				 // W przeciwnym wypadku rozpocznij pobieranie nowego
				else
					uiParamBegIdx = uiCurrIdx;		// zmieñ pocz¹tek parametru na nowy
			}
			 // Wyra¿enie w nawiasie w funkcji - przeskocz
			else if (szExp[uiCurrIdx] == '(')
				skipBracketExp();
			 // Nie domkniêto nawiasów funkcji przed koñcem wyra¿enia
			else if (szExp[uiCurrIdx] == 0)
				throw CalcError(CalcError::NOT_CLOSED_BRACKET, "niedomkniête nawiasy", uiAllParamsBegIdx - 1 + uiOffset, uiCurrIdx - 1 + uiOffset);
			 // Inny znak - pomiñ
			else
				uiCurrIdx++;
		}

		 // Pomiñ spacje
		if (uiCurrIdx != szExp.length())
			uiCurrIdx = szExp.find_first_not_of(WHITE_CHARS, uiCurrIdx);
		if (uiCurrIdx == std::string::npos)
			uiCurrIdx = szExp.length();

		return new CalcTreeFunct(szVarName, opParamsVector);
	}
	 // Podano zmienn¹
	else
	{
		return new CalcTreeVar(szVarName);
	}
}

// Metoda sprawdza, czy przed wyra¿eniem nie podano znaku i przesuwa
// indeks. Zwraca true, jeœli nale¿y utworzyæ minus unarny
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

// Metoda parsujê nastêpny operator i przesuwa wskaŸnik. Zwraca jego
// rodzaj
//------------------------------------------------------------------------
CALC_NODE_TYPE CalcParser::getOp()
{
	 // Jeœli napotkano koniec, zg³oœæ niespodziewany koniec wyra¿enia
	if (uiCurrIdx >= szExp.length())
		throw CalcError(CalcError::UNEXPECTED_END, "niespodziewany koniec wyra¿enia", uiCurrIdx + uiOffset, uiCurrIdx + uiOffset);

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

		 // Mno¿enie
		case '*':
			 // Mno¿enie *
			if (eNode == NODE_EMPTY)
				eNode = NODE_PRODUCT;
			 // Potêgowanie - wersja **
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

		 // Potêgowanie
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

		 // Napotkanie potencjalnego pocz¹tku poprawnego wyra¿enia
		case '(':
		CASE_ALPHA_C:
		CASE_NUM_C:
		case '.':
		case ',':
			 // Jeœli napotkano ju¿ na znak, zwróæ go. Jeœli nie, zwróæ domyœlny iloczyn (5pi <=> 5*pi, 6(...) <=> 6*(...))
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
				
				// Zobacz, czy nastêpne nie jest poprawne wyra¿enie - liczba, wyra¿enie w nawiasie, funkcja, zmienna
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
				 // Zabrak³o znaku miêdzy liczbami
				else
				{
					delete opDelete;
					throw CalcError(CalcError::EXPECTED_SIGN, "spodziewany znak dzia³ania", uiCurrIdx + uiOffset, uiCurrIdx + uiOffset);
				}*/
			}
			 // Napotkano na inny znak po sczytaniu dzia³ania - przerwij
			else
			{
				return eNode;
			}
			break;
		}
	}
}

// Metoda wstawia do parsera nowe wyra¿enie
//------------------------------------------------------------------------
void CalcParser::insertExp(const std::string & szNewExp)
{
	szExp = szNewExp;
	uiCurrIdx = 0;
	uiOffset = 0;
}

// Metoda zwraca sta³¹ referencjê do obecnego wyra¿enia
//-----------------------------------------------------------------------
const std::string & CalcParser::getExp()
{
	return szExp;
}

// Metoda parsuje wyra¿enie wstawione za pomoc¹ insertExp, tworzy drzewo
// CalcTree i zwraca wskaŸnik do niego
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

	 // SprawdŸ, czy wyra¿enie nie jest za d³ugie (zbyt d³ugie powoduj¹ prze³adownie stosu)
	if (uiLength > MAX_EXP_LENGTH)
		throw CalcError(CalcError::TOO_LONG, std::string("za d³ugie wyra¿enie (") + std::to_string(uiLength) + std::string(" znaków; max ") + std::to_string(MAX_EXP_LENGTH) + std::string(")"), 0, uiLength - 1);

	 // Przechwytuj b³êdy - w razie czego usuñ zajêt¹ pamiêæ i wyrzuæ dalej
	try
	{
		 // Pobierz pierwsz¹ wartoœæ
		opTree = getVal(opCurrentPointer);
		opCurrentNode = opCurrentPointer;

		 // Powtarzaj a¿ do koñca wyra¿enia
		while (uiCurrIdx != uiLength)
		{
			 // Pobierz operator i sprawdŸ rodzaj
			eOpType = getOp();

			switch (CALC_NODE_DATA[eOpType].eOpNaryType)
			{
			 // Operator unarny
			case OP_UNARY:

				 // £¹czny lewostronnie - przesuwaj w górê drzewa opCurrentNode, a¿ wêze³-rodzic jest o NI¯SZYM priorytecie lub jest pocz¹tkiem
				if (CALC_NODE_DATA[eOpType].eOpAssoc == OP_ASSOC_LEFT)
					while (opCurrentNode->getParent() != nullptr && opCurrentNode->getParent()->getOpPrecedence() > CALC_NODE_DATA[eOpType].usOpPrec)
						opCurrentNode = opCurrentNode->getParent();
				 // £¹czny prawostronnie - przesuwaj w górê drzewa opCurrentNode, a¿ wêze³-rodzic jest o NI¯SZYM LUB RÓWNYM priorytecie lub jest pocz¹tkiem
				else
					while (opCurrentNode->getParent() != nullptr && opCurrentNode->getParent()->getOpPrecedence() >= CALC_NODE_DATA[eOpType].usOpPrec)
						opCurrentNode = opCurrentNode->getParent();
				 
				 // Od³¹cz wêze³ od rodzica
				opParentNode = opCurrentNode->getParent();
				if (opParentNode != nullptr)
					opCurrentNode = opParentNode->detachNode();
				else
					opCurrentNode = opTree;
				
				 // Przy³¹cz do rodzica wêze³ zagnie¿d¿ony w nowym, zale¿nym od operatora
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

				 // Pobierz nastêpn¹ wartoœæ
				opNewNode = getVal(opCurrentPointer);

				 // £¹czny lewostronnie - przesuwaj w górê drzewa opCurrentNode, a¿ wêze³-rodzic jest o NI¯SZYM priorytecie lub jest pocz¹tkiem
				if (CALC_NODE_DATA[eOpType].eOpAssoc == OP_ASSOC_LEFT)
					while (opCurrentNode->getParent() != nullptr && opCurrentNode->getParent()->getOpPrecedence() > CALC_NODE_DATA[eOpType].usOpPrec)
						opCurrentNode = opCurrentNode->getParent();
				 // £¹czny prawostronnie - przesuwaj w górê drzewa opCurrentNode, a¿ wêze³-rodzic jest o NI¯SZYM LUB RÓWNYM priorytecie lub jest pocz¹tkiem
				else
					while (opCurrentNode->getParent() != nullptr && opCurrentNode->getParent()->getOpPrecedence() >= CALC_NODE_DATA[eOpType].usOpPrec)
						opCurrentNode = opCurrentNode->getParent();

				 // Od³¹cz wêze³ od rodzica
				opParentNode = opCurrentNode->getParent();
				if (opParentNode != nullptr)
					opCurrentNode = opParentNode->detachNode();
				else
					opCurrentNode = opTree;

				 // Przy³¹cz do rodzica wêz³y zagnie¿d¿one w nowym, zela¿nym od operatora
				switch (eOpType)
				{
				 // Suma
				case NODE_SUM:
					opNewNode = (new CalcTreeSum(opCurrentNode, opNewNode));
					break;
				
				 // Ró¿nica
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

				 // Potêga
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

				 // Przesuñ wskaŸnik do ostatniego wêz³a na obiekt pod nowym wêz³em
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