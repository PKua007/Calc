// var_engine.cpp
//---------------------------------------------------------------------
// Plik Ÿród³owy z sinlikiem zmiennych
//---------------------------------------------------------------------
// Silnik zmiennych oferuje obs³ugê bazy zmiennych - mo¿na do niej
// dodawaæ pozycje, przegl¹daæ, pobieraæ wartoœci. Obs³uguje specjalne
// zmienne steruj¹ce do funkcji, które mog¹ byæ u¿yte tylko
// w konkretnych funkcjach.
//---------------------------------------------------------------------
// (C) 2016 PKua
//---------------------------------------------------------------------

#include "stdafx.h"
#include "var_engine.h"


// Metody klasy VarEngine::Struct
//************************************************************************************************************************


// Konstruktor struktury zmiennej - przyjmuje nazwê, wartoœæ, tablicê powi¹zanych funkcji i iloœæ elementów w tablicy
//------------------------------------------------------------------------------------------------------------------------
VarEngine::Struct::Struct(const std::string & szName, double dValue, const std::string & szDesc, VAR_TYPE eType) :
	szName(szName),
	dValue(dValue),
	szDesc(szDesc),
	eType(eType) {
	//std::cout << "konstruktor" << std::endl;
}

/*// Konstruktor kopiuj¹cy struktury zmiennej
//------------------------------------------------------------------------------------------------------------------------
VarEngine::Struct::Struct(const Struct & o) : szName(o.szName), dValue(o.dValue), szDesc(o.szDesc), bIsConst(o.bIsConst), uiLinkedNumber(o.uiLinkedNumber), szLinkedArray(o.szLinkedArray)
{
	//std::cout << "konstruktor kopiujacy " << o.uiLinkedNumber << " " << uiLinkedNumber << std::endl;

	 // Skopiuj tablicê funkcji
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

// Metoda zmienia wartoœæ zmiennej, jeœli mo¿na. Jeœli nie, wyrzuca wyj¹tek
//------------------------------------------------------------------------------------------------------------------------
void VarEngine::Struct::changeValue(double dNewValue)
{
	if (eType != NONCONST_VAR)
		throw std::runtime_error("Próba zmiany wartoœci sta³ej wyra¿enia");
	dValue = dNewValue;
}


// Metody klasy VarEngine
//************************************************************************************************************************


// Funkcja dodaje now¹ zmienn¹ do bazy - jeœli siê uda, zwraca indeks dodanej; jeœli nie, to -1
//------------------------------------------------------------------------------------------------------------------------
// szName - nazwa nowej zmiennej
// dValue - wartoœæ nowej zmiennej
// szDesc - opis zmiennej
// eType - typ zmiennej
//     VarEngine::CONST_VAR - sta³a
//     VarEngine::NONCONST_VAR - modyfikowalna zmienna
//     VarEngine::FLAG_VAR - flaga funkcji
//------------------------------------------------------------------------------------------------------------------------
size_t VarEngine::add(const std::string & szName, double dValue, const std::string & szDesc, VAR_TYPE eType)
{
	if (exists(szName))
		throw std::invalid_argument("Redefinicja zmiennej " + szName);

	if (szName.length() > _MAX_VAR_LENGTH)
		throw std::invalid_argument("Próba utworzenie zmiennej o zbyt d³ugiej nazwie " + szName);

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

	 // Dodaj zmienn¹ do tablicy
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

// Funkcja zwraca strukturê zmiennej o podanym indeksie
//------------------------------------------------------------------------------------------------------------------------
VarEngine::Struct & VarEngine::operator[](size_t idx)
{
	if (idx >= oVarVector.size())
		throw std::runtime_error("Próba dostêpu do nieistniej¹cego indeksu zmiennej");

	return oVarVector.at(idx);
}

// Funkcja zwraca strukturê zmiennej o podanej nazwie
//------------------------------------------------------------------------------------------------------------------------
VarEngine::Struct & VarEngine::operator[](const std::string & szName)
{
	size_t idx = getIdx(szName);
	if (idx == nidx)
		throw std::runtime_error("Próba dostêpu do nieistniej¹cej zmiennej \"" + szName + "\"");

	return oVarVector.at(idx);
}
