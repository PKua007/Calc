// funct_engine.cpp
//---------------------------------------------------------------------
// Plik �r�d�owy z sinlikiem funkcji
//---------------------------------------------------------------------
// Silnik zmiennych oferuje obs�ug� bazy funkcji - mo�na do niej
// dodawa� pozycje, przegl�da�, wykonywa� funkcje i pobiera� opisy.
// Obs�uguje funkcje z flagami (np. sin(pi, deg)) i zmienn� liczb�
// parametr�w (np. max(5, 6, 7, 24, pi))
//---------------------------------------------------------------------
// (C) 2016 PKua
//---------------------------------------------------------------------


#include "stdafx.h"
#include "funct_engine.h"


// Konstruktor struktury funkcji
//---------------------------------------------------------------------
FunctEngine::Struct::Struct(const std::string & szName, FunctPtr pFunctPointer, const std::string & szDesc, const std::vector<std::string> & szAltNamesVector) :
	szName(szName),
	pFunctPointer(pFunctPointer),
	szDesc(szDesc),
	szAltNamesVector(szAltNamesVector) { }

// Funkcje dodaj� parametr funkcji o okre�lonym typie
//---------------------------------------------------------------------
// value, flag, va_list - poszczeg�lne typy parametr�w
// szName - nazwa parametru
// szDesc - opis parametru
//---------------------------------------------------------------------
size_t FunctEngine::Struct::addParam(Param::Value value, const std::string szParamName, const std::string szDesc) const
{
	// Sprawd�, czy parametr o podanej nazwie ju� nie istnieje
	if (paramExists(szParamName))
		throw std::runtime_error("Funkcja \"" + szName + "\": ju� istnieje parametr o nazwie \"" + szParamName + "\"");

	// Sprawd�, czy ostatnim parametrem nie jest zmienna lista argument�w
	if (!oParamsVector.empty() && oParamsVector.back().eType == Param::VA_LIST)
		throw std::runtime_error("Funkcja \"" + szName + "\": nie mo�na doda� parametru po zmiennej li�cie parametr�w");

	// Sprawd�, czy obecny parametr ma warto�� domy�ln�, je�li ostatni ma
	if (!value.bHasDefault && !oParamsVector.empty())
		if ( (oParamsVector.back().eType == Param::VALUE && oParamsVector.back().uParam.value.bHasDefault) ||
			(oParamsVector.back().eType == Param::FLAG && oParamsVector.back().uParam.flag.bHasDefault) )
			throw std::runtime_error("Funkcja \"" + szName + "\": parametry domy�lne musz� by� na ko�cu listy");

	oParamsVector.push_back(Param(value, szParamName, szDesc));
	return oParamsVector.size() - 1;
}

size_t FunctEngine::Struct::addParam(Param::Flag flag, const std::string szParamName, const std::string szDesc) const
{
	// Sprawd�, czy parametr o podanej nazwie ju� nie istnieje
	if (paramExists(szParamName))
		throw std::runtime_error("Funkcja \"" + szName + "\": ju� istnieje parametr o nazwie \"" + szParamName + "\"");

	// Sprawd�, czy ostatnim parametrem nie jest zmienna lista argument�w
	if (!oParamsVector.empty() && oParamsVector.back().eType == Param::VA_LIST)
		throw std::runtime_error("Funkcja \"" + szName + "\": nie mo�na doda� parametru po zmiennej li�cie parametr�w");

	// Sprawd�, czy obecny parametr ma warto�� domy�ln�, je�li ostatni ma
	if (!flag.bHasDefault && !oParamsVector.empty())
		if ((oParamsVector.back().eType == Param::VALUE && oParamsVector.back().uParam.value.bHasDefault) ||
			(oParamsVector.back().eType == Param::FLAG && oParamsVector.back().uParam.flag.bHasDefault))
			throw std::runtime_error("Funkcja \"" + szName + "\": parametry domy�lne musz� by� na ko�cu listy");

	oParamsVector.push_back(Param(flag, szParamName, szDesc));
	return oParamsVector.size() - 1;
}

size_t FunctEngine::Struct::addParam(Param::VaList va_list, const std::string szParamName, const std::string szDesc) const
{
	// Sprawd�, czy parametr o podanej nazwie ju� nie istnieje
	if (paramExists(szParamName))
		throw std::runtime_error("Funkcja \"" + szName + "\": ju� istnieje parametr o nazwie \"" + szParamName + "\"");

	// Sprawd�, czy maksymalna liczba parametr�w nie jest mniejsza od minimalnej
	if (va_list.usMaxParams <= va_list.usMinParams)
		throw std::runtime_error("Funkcja \"" + szName + "\": maksymalna liczba parametr�w zmiennej listy musi by� wi�ksza od minimalnej");
		
	if (!oParamsVector.empty())
	{
		switch (oParamsVector.back().eType)
		{
		// Sprawd�, czy nie dodano zmiennej listy argument�w po innej zmiennej li�cie argument�w
		case Param::VA_LIST:
			throw std::runtime_error("Funkcja \"" + szName + "\": ju� zdefiniowano zmienn� list� parametr�w");
			break;

		// Sprawd�, czy nie dodano zmiennej listy argument�w po parametrze z warto�ci� domy�ln�
		case Param::VALUE:
			if (oParamsVector.back().uParam.value.bHasDefault)
				throw std::runtime_error("Funkcja \"" + szName + "\": nie mo�na doda� zmiennej listy parametr�w po parametrze z warto�ci� domy�ln�");
			break;

		case Param::FLAG:
			if (oParamsVector.back().uParam.flag.bHasDefault)
				throw std::runtime_error("Funkcja \"" + szName + "\": nie mo�na doda� zmiennej listy parametr�w po parametrze z warto�ci� domy�ln�");
			break;
		}
	}

	oParamsVector.push_back(Param(va_list, szParamName, szDesc));
	return oParamsVector.size() - 1;
}


// Funkcja pobiera indeks parametru po jego nazwie; je�li nie istnieje,
// zwraca FunctEngine::nidx
//---------------------------------------------------------------------
size_t FunctEngine::Struct::getParamIdx(const std::string & szName) const
{
	for (auto i = oParamsVector.begin(); i != oParamsVector.end(); i++)
		if ((*i).szName == szName)
			return i - oParamsVector.begin();

	return nidx;
}

// Funkcja sprawdza, czy parametr o podanej nazwie istnieje
//---------------------------------------------------------------------
bool FunctEngine::Struct::paramExists(const std::string & szName) const
{
	return getParamIdx(szName) != nidx;
}

// Funkcja sprawdza, czy funkcja przyjmuje podan� ilo�� argument�w
//---------------------------------------------------------------------
bool FunctEngine::Struct::isNumParamsValid(size_t uiNum) const
{
	// Pusta lista parametr�w - nie powinien by� podany �aden
	if (oParamsVector.empty())
		return !uiNum;

	// Ostatni parmetr jest zmienn� list� argument�w - sprawd� dost�pny zakres paramter�w
	if (oParamsVector.back().eType == Param::VA_LIST)
	{
		if (uiNum < oParamsVector.size() - 1 + oParamsVector.back().uParam.va_list.usMinParams)
			return false;
		else if (oParamsVector.back().uParam.va_list.usMaxParams != (unsigned short)(-1) && uiNum > oParamsVector.size() - 1 + oParamsVector.back().uParam.va_list.usMaxParams)
			return false;
		else
			return true;
	}

	// Czy uiNum nie przekracza liczby parametr�w?
	if (uiNum > oParamsVector.size())
		return false;

	// Czy uiNum nie jest za ma�e?
	if (uiNum < oParamsVector.size())
	{
		switch (oParamsVector[uiNum].eType)
		{
		case Param::VALUE:
			if (!oParamsVector[uiNum].uParam.value.bHasDefault)
				return false;
			break;

		case Param::FLAG:
			if (!oParamsVector[uiNum].uParam.flag.bHasDefault)
				return false;
			break;
		}
		return true;
	}

	return true;
}

// Funkcja automatycznie uzupe�nia wektor podany poprzez referencj�
// z parametrami o niepodane warto�ci domy�lne 
//---------------------------------------------------------------------
void FunctEngine::Struct::autoCompParams(std::vector<double> &dParamsVector) const
{
	// Sprawd�, czy w og�le trzeba uzupe�nia�
	if (oParamsVector.empty() || oParamsVector.back().eType == Param::VA_LIST || dParamsVector.size() >= oParamsVector.size())
		return;

	auto iterLast = oParamsVector.begin() + dParamsVector.size();

	// Sprawd� na wszelki wypadek, czy nie podano za ma�o argument�w
	switch ((*iterLast).eType)
	{
	case Param::VALUE:
		if (!(*iterLast).uParam.value.bHasDefault)
			throw std::runtime_error("Funkcja \"" + szName + "\": pr�ba autouzupe�nienia parametru bez domy�lnej warto�ci");
		break;

	case Param::FLAG:
		if (!(*iterLast).uParam.flag.bHasDefault)
			throw std::runtime_error("Funkcja \"" + szName + "\": pr�ba autouzupe�nienia parametru bez domy�lnej warto�ci");
		break;
	}

	// Iteruj po reszcie niepodanych parametr�w i uzupe�nij
	std::transform(iterLast, oParamsVector.end(), 
		std::back_inserter(dParamsVector),
		[](const Param & param) {
		switch (param.eType)
		{
		case Param::VALUE:
			return param.uParam.value.dDefaultVal;
			break;

		case Param::FLAG:
			return gVarEngine[param.uParam.flag.uiDefaultIdx].value();
			break;

		default:
			return 0.0;
			break;
		}
	});
}

// Operator dost�pu do parametru po jego indeksie
//---------------------------------------------------------------------
const FunctEngine::Param & FunctEngine::Struct::operator[](size_t idx) const
{
	if (idx >= oParamsVector.size())
		throw std::runtime_error("Funkcja \"" + szName + "\": pr�ba dost�pu do parametru o nieistniej�cym indeksie");

	return oParamsVector.at(idx);
}

// Operator dost�pu do parametru po jego nazwie
//---------------------------------------------------------------------
const FunctEngine::Param & FunctEngine::Struct::operator[](const std::string & szName) const
{
	size_t idx = getParamIdx(szName);
	if (idx == nidx)
		throw std::runtime_error("Funkcja \"" + szName + "\": pr�ba dost�pu do nieistniej�cego parametru \"" + szName + "\"");

	return oParamsVector.at(idx);
}


// Funkcja dodaje funkcj� do bazy funkcji
//---------------------------------------------------------------------
// szName - nazwa funkcji
// pFunctPointer - wska�nik do funkcji przyjmuj�cej wektor liczb
//   zmiennoprzecinkowych i zwracaj�cej liczb� zmiennoprzecinkow�
// szDesc - opis funkcji
// szAltNames - alternatywne nazwy funkcji w formacie "nazwa1;nazwa2"
//---------------------------------------------------------------------
size_t FunctEngine::add(const std::string & szName, FunctPtr pFunctPointer, const std::string & szDesc, const std::string & szAltNames)
{
	if (exists(szName))
		throw std::runtime_error("Funkcja \"" + szName + "\" ju� istnieje");

	if (pFunctPointer == nullptr)
		throw std::runtime_error("Funkcja \"" + szName + "\": pr�ba przydzielenia pustego wska�nika funkcji");

	// Rozbij alternatywne nazwy
	std::vector<std::string> szAltNamesVector = pkutl::explode(szAltNames, ';');

	// Usu� niepotrzebne bia�e znaki
	std::transform(szAltNamesVector.begin(), szAltNamesVector.end(), szAltNamesVector.begin(), pkutl::trim_ws);

	// Usu� puste wpisy
	szAltNamesVector.erase(
		std::remove_if(szAltNamesVector.begin(), szAltNamesVector.end(), [](const std::string & alt_name) { return alt_name.empty(); }),
		szAltNamesVector.end() );

	// Usu� duplikaty i sprawd�, czy kt�ra� z alternatycnych nazw nie jest zaj�ta
	for (auto i = szAltNamesVector.begin(); i != szAltNamesVector.end(); i++)
	{
		if (exists(*i))
			throw std::invalid_argument("Funkcja \"" + szName + "\": alternatywna nazwa \"" + *i + "\" ju� jest zaj�ta");

		for (auto j = i + 1; j != szAltNamesVector.end(); j++)
		{
			if (*i == *j)
			{
				szAltNamesVector.erase(j, j + 1);
				break;
			}
		}
	}

	// Dodaj funkcj� do bazy i zwr�� indeks
	oFunctVector.push_back(Struct(szName, pFunctPointer, szDesc, szAltNamesVector));
	return oFunctVector.size() - 1;
}

// Funkcja sprawdza, czy funkcja o podenej nazwie istnieje
//---------------------------------------------------------------------
bool FunctEngine::exists(const std::string & szName) const
{
	return getIdx(szName) != nidx;
}

// Funkcja pobiera indeks funkcji o podanej nazwie; je�li nie istnieje,
// zwraca FunctEngine::nidx
//---------------------------------------------------------------------
size_t FunctEngine::getIdx(const std::string & szName) const
{
	for (auto i = oFunctVector.begin(); i != oFunctVector.end(); i++)
		if ((*i).szName == szName || std::find((*i).szAltNamesVector.begin(), (*i).szAltNamesVector.end(), szName) != (*i).szAltNamesVector.end())
			return i - oFunctVector.begin();

	return nidx;
}

// Operator dost�pu do funkcji po indeksie
//---------------------------------------------------------------------
const FunctEngine::Struct & FunctEngine::operator[](size_t idx)
{
	if (idx >= oFunctVector.size())
		throw std::runtime_error("Pr�ba dost�pu do nieistniej�cego indeksu funkcji");

	return oFunctVector.at(idx);
}

// Operator dost�pu do funkcji po nazwia
//---------------------------------------------------------------------
const FunctEngine::Struct & FunctEngine::operator[](const std::string & szName)
{
	size_t idx = getIdx(szName);
	if (idx == nidx)
		throw std::runtime_error("Pr�ba dost�pu do nieistniej�cej funkcji \"" + szName + "\"");

	return oFunctVector.at(idx);
}

// Konstuktor parametru "warto��" bez warto�ci domy�lnej
//---------------------------------------------------------------------
FunctEngine::Param::Value::Value() :
	bHasDefault(false),
	dDefaultVal(0.0) 
{}


// Konstruktor parametru "warto��" z warto�ci� domy�ln�
//---------------------------------------------------------------------
FunctEngine::Param::Value::Value(double dDefaultVal) :
	bHasDefault(true),
	dDefaultVal(dDefaultVal)
{}

// Konstruktor parametru "flaga" bez warto�ci domy�lnej
//---------------------------------------------------------------------
FunctEngine::Param::Flag::Flag(const std::string & szFlags, const std::string & szDefaultFlag) :
	bHasDefault(false),
	uiDefaultIdx(VarEngine::nidx)
{
	// Rozbij list� flag
	std::vector<std::string> szFlagsVector = pkutl::explode(szFlags, ';');

	// Usu� bia�e znaki z ko�c�w flag
	std::transform(szFlagsVector.begin(), szFlagsVector.end(), szFlagsVector.begin(), pkutl::trim_ws);

	// Usu� puste wpisy
	szFlagsVector.erase(
		std::remove_if(szFlagsVector.begin(), szFlagsVector.end(), [](const std::string & flag) {return flag.empty(); }),
		szFlagsVector.end()
	);

	// Sprawd�, czy wszystkie flagi istniej� i utw�rz set z indeksami
	std::transform(
		szFlagsVector.begin(),
		szFlagsVector.end(),
		std::inserter(uiFlagIdxSet, uiFlagIdxSet.begin()),
		[](const std::string & flag) {
			size_t idx = gVarEngine.getIdx(flag);
			if (idx == nidx)
				throw std::invalid_argument("Pr�ba utworzenia parametru z nieistniej�c� flag� \"" + flag + "\"");
			else if (gVarEngine[idx].type() != VarEngine::FLAG_VAR)
				throw std::invalid_argument("Pr�ba utworzenia parametru ze zmienn� \"" + flag + "\", kt�ra nie jest flag�");
			return idx;
	});
	
	// Sprawd�, czy domy�lna flaga istnieje i jest na li�cie flag
	if (!szDefaultFlag.empty())
	{
		size_t idx = gVarEngine.getIdx(szDefaultFlag);

		// Sprawd�, czy flaga istnieje i czy jest na li�cie dost�pnych
		if (idx == VarEngine::nidx || std::find(uiFlagIdxSet.begin(), uiFlagIdxSet.end(), idx) == uiFlagIdxSet.end())
			throw std::invalid_argument("Domy�lnej flagi \"" + szDefaultFlag + "\" nie ma na li�cie wszystkich flag");

		uiDefaultIdx = idx;
		bHasDefault = true;
	}
}

// Konstruktor parametru "zmienna lista argument�w"
//---------------------------------------------------------------------
FunctEngine::Param::VaList::VaList(unsigned short usMinParams, unsigned short usMaxParams) :
	usMinParams(usMinParams),
	usMaxParams(usMaxParams) {}

// Destruktor klasy Param - usuwa odpowiedni obiekt w unii
//---------------------------------------------------------------------
FunctEngine::Param::~Param()
{
	// Usu� adekwatny obiekt parametru
	switch (eType)
	{
	case VALUE:
		uParam.value.~Value();
		break;

	case FLAG:
		uParam.flag.~Flag();
		break;

	case VA_LIST:
		uParam.va_list.~VaList();
		break;
	};
}

// Konstruktor kopiuj�cy parametru - kopiuje odpowiedni obiekt unii
//---------------------------------------------------------------------
FunctEngine::Param::Param(const Param & other) :
	szName(other.szName),
	szDesc(other.szDesc),
	eType(other.eType)
{
	// Skopiuj adekwatny obiekt parametru uprzednio wywo�uj�c konstruktor na odpowiednim obiekcie unii
	switch (eType)
	{
	case VALUE:
		new(&uParam.value) Value;
		uParam.value = other.uParam.value;
		break;

	case FLAG:
		new(&uParam.flag) Flag;
		uParam.flag = other.uParam.flag;
		break;

	case VA_LIST:
		new(&uParam.va_list) VaList;
		uParam.va_list = other.uParam.va_list;
		break;
	};
}

// Konstruktor przenoszenia parametru - kopiuje odpowiedni obiekt unii
//---------------------------------------------------------------------
FunctEngine::Param::Param(Param && other) :
	eType(other.eType)
{
	szName = std::move(other.szName);
	szDesc = std::move(other.szDesc);

	// Przenie� adekwatny obiekt parametru uprzednio wywo�uj�c konstruktor na odpowiednim obiekcie unii
	switch (other.eType)
	{
	case VALUE:
		new(&uParam.value) Value;
		uParam.value = std::move(other.uParam.value);
		break;

	case FLAG:
		new(&uParam.flag) Flag;
		uParam.flag = std::move(other.uParam.flag);
		break;

	case VA_LIST:
		new(&uParam.va_list) VaList;
		uParam.va_list = std::move(other.uParam.va_list);
		break;
	};
}

// Konstruktory klasy parametru
//---------------------------------------------------------------------
// value, flag, va_list - odpowiedni rodzaj parametru z danymi
// szName - nazwa parametru (unikalna)
// szDesc - opis parametru
//---------------------------------------------------------------------
FunctEngine::Param::Param(const Value & value, const std::string szName, const std::string szDesc) :
	szName(szName),
	szDesc(szDesc),
	eType(VALUE)
{
	// Wywo�aj konstruktor na obiekcie value unii i przypisz przekazany parametr
	new(&uParam.value) Value;
	uParam.value = value;
}

FunctEngine::Param::Param(const Flag & flag, const std::string szName, const std::string szDesc) :
	szName(szName),
	szDesc(szDesc),
	eType(FLAG)
{
	// Wywo�aj konstruktor na obiekcie flag unii i przypisz przekazany parametr
	new(&uParam.flag) Flag;
	uParam.flag = flag;
}

FunctEngine::Param::Param(const VaList & va_list, const std::string szName, const std::string szDesc) :
	szName(szName),
	szDesc(szDesc),
	eType(VA_LIST)
{
	// Wywo�aj konstruktor na obiekcie va_list unii i przypisz przekazany parametr
	new(&uParam.va_list) VaList;
	uParam.va_list = va_list;
}