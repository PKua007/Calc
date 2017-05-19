// var_engine.cpp
//---------------------------------------------------------------------
// Plik �r�d�owy z sinlikiem zmiennych
//---------------------------------------------------------------------
// Silnik zmiennych oferuje obs�ug� bazy zmiennych - mo�na do niej
// dodawa� pozycje, przegl�da�, pobiera� warto�ci. Obs�uguje specjalne
// zmienne steruj�ce do funkcji, kt�re mog� by� u�yte tylko
// w konkretnych funkcjach.
//---------------------------------------------------------------------
// (C) 2016 PKua
//---------------------------------------------------------------------

#include "stdafx.h"
#include "var_engine.h"


// Metody klasy VarEngine::Struct
//************************************************************************************************************************


// Konstruktor struktury zmiennej - przyjmuje nazw�, warto��, tablic� powi�zanych funkcji i ilo�� element�w w tablicy
//------------------------------------------------------------------------------------------------------------------------
VarEngine::Struct::Struct(const std::string & szName, double dValue, const std::string & szDesc, VAR_TYPE eType) :
	szName(szName),
	dValue(dValue),
	szDesc(szDesc),
	eType(eType) {
	//std::cout << "konstruktor" << std::endl;
}

/*// Konstruktor kopiuj�cy struktury zmiennej
//------------------------------------------------------------------------------------------------------------------------
VarEngine::Struct::Struct(const Struct & o) : szName(o.szName), dValue(o.dValue), szDesc(o.szDesc), bIsConst(o.bIsConst), uiLinkedNumber(o.uiLinkedNumber), szLinkedArray(o.szLinkedArray)
{
	//std::cout << "konstruktor kopiujacy " << o.uiLinkedNumber << " " << uiLinkedNumber << std::endl;

	 // Skopiuj tablic� funkcji
	if (o.szLinkedArray != nullptr)
	{
		//std::cout << "kopiuje tablice " << std::endl;
		szLinkedArray = new std::string[uiLinkedNumber];
		for (size_t i = 0U; i < uiLinkedNumber; i++)
			szLinkedArray[i] = o.szLinkedArray[i];
	}
}

// Destruktor struktury zmiennej
//------------------------------------------------------------------------------------------------------------------------
VarEngine::Struct::~Struct()
{
	if (szLinkedArray != nullptr)
		delete[] szLinkedArray;
	//std::cout << "destruktor" << std::endl;
}*/

// Metoda sprawdza, czy zmienna jest przypisana do podanej funkcji
//------------------------------------------------------------------------------------------------------------------------
bool VarEngine::Struct::isLinkedTo(const std::string & szFunctName) const
{
	return std::find(szLinkedVector.begin(), szLinkedVector.end(), szFunctName) != szLinkedVector.end();
}

// Metoda zmienia warto�� zmiennej, je�li mo�na. Je�li nie, wyrzuca wyj�tek
//------------------------------------------------------------------------------------------------------------------------
void VarEngine::Struct::changeValue(double dNewValue)
{
	if (eType != NONCONST_VAR)
		throw std::runtime_error("Pr�ba zmiany warto�ci sta�ej wyra�enia");
	dValue = dNewValue;
}


// Metody klasy VarEngine
//************************************************************************************************************************


// Funkcja dodaje now� zmienn� do bazy - je�li si� uda, zwraca indeks dodanej; je�li nie, to -1
//------------------------------------------------------------------------------------------------------------------------
// szName - nazwa nowej zmiennej
// dValue - warto�� nowej zmiennej
// szDesc - opis zmiennej
// eType - typ zmiennej
//     VarEngine::CONST_VAR - sta�a
//     VarEngine::NONCONST_VAR - modyfikowalna zmienna
//     VarEngine::FLAG_VAR - flaga funkcji
//------------------------------------------------------------------------------------------------------------------------
size_t VarEngine::add(const std::string & szName, double dValue, const std::string & szDesc, VAR_TYPE eType)
{
	if (exists(szName))
		throw std::invalid_argument("Redefinicja zmiennej " + szName);

	if (szName.length() > _MAX_VAR_LENGTH)
		throw std::invalid_argument("Pr�ba utworzenie zmiennej o zbyt d�ugiej nazwie " + szName);

	 // Zaktualizuj liczniki
	switch (eType)
	{
	case NONCONST_VAR:
		uiNumNonConsts++;
		uiNumNonFlags++;
		break;

	case CONST_VAR:
		uiNumConsts++;
		uiNumNonFlags++;
		break;

	case FLAG_VAR:
		uiNumConsts++;
		uiNumFlags++;
		break;

	 // Niepoprawny rodzaj zmiennej - nie dodawaj zmiennej
	default:
		return nidx;
	}

	 // Dodaj zmienn� do tablicy
	oVarVector.push_back(Struct{ szName, dValue, szDesc, eType });
	return oVarVector.size() - 1U;
}


// Funkcja sprawdza, czy zmienna szName istnieje
//------------------------------------------------------------------------------------------------------------------------
bool VarEngine::exists(const std::string & szName) const
{
	return getIdx(szName) != nidx;
}

// Funkcja pobiera indeks zmiennej o podanej nazwie
//------------------------------------------------------------------------------------------------------------------------
size_t VarEngine::getIdx(const std::string & szName) const
{
	for (auto i = oVarVector.begin(); i != oVarVector.end(); i++)
		if ((*i).szName == szName)
			return i - oVarVector.begin();

	return nidx;
}

// Funkcja zwraca struktur� zmiennej o podanym indeksie
//------------------------------------------------------------------------------------------------------------------------
VarEngine::Struct & VarEngine::operator[](size_t idx)
{
	if (idx >= oVarVector.size())
		throw std::runtime_error("Pr�ba dost�pu do nieistniej�cego indeksu zmiennej");

	return oVarVector.at(idx);
}

// Funkcja zwraca struktur� zmiennej o podanej nazwie
//------------------------------------------------------------------------------------------------------------------------
VarEngine::Struct & VarEngine::operator[](const std::string & szName)
{
	size_t idx = getIdx(szName);
	if (idx == nidx)
		throw std::runtime_error("Pr�ba dost�pu do nieistniej�cej zmiennej \"" + szName + "\"");

	return oVarVector.at(idx);
}
