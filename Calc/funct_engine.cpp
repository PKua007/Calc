// funct_engine.cpp
//---------------------------------------------------------------------
// Plik Ÿród³owy z sinlikiem funkcji
//---------------------------------------------------------------------
// Silnik zmiennych oferuje obs³ugê bazy funkcji - mo¿na do niej
// dodawaæ pozycje, przegl¹daæ, wykonywaæ funkcje i pobieraæ opisy.
// Obs³uguje funkcje z flagami (np. sin(pi, deg)) i zmienn¹ liczb¹
// parametrów (np. max(5, 6, 7, 24, pi))
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

// Funkcje dodaj¹ parametr funkcji o okreœlonym typie
//---------------------------------------------------------------------
// value, flag, va_list - poszczególne typy parametrów
// szName - nazwa parametru
// szDesc - opis parametru
//---------------------------------------------------------------------
size_t FunctEngine::Struct::addParam(Param::Value value, const std::string szParamName, const std::string szDesc) const
{
	// SprawdŸ, czy parametr o podanej nazwie ju¿ nie istnieje
	if (paramExists(szParamName))
		throw std::runtime_error("Funkcja \"" + szName + "\": ju¿ istnieje parametr o nazwie \"" + szParamName + "\"");

	// SprawdŸ, czy ostatnim parametrem nie jest zmienna lista argumentów
	if (!oParamsVector.empty() && oParamsVector.back().eType == Param::VA_LIST)
		throw std::runtime_error("Funkcja \"" + szName + "\": nie mo¿na dodaæ parametru po zmiennej liœcie parametrów");

	// SprawdŸ, czy obecny parametr ma wartoœæ domyœln¹, jeœli ostatni ma
	if (!value.bHasDefault && !oParamsVector.empty())
		if ( (oParamsVector.back().eType == Param::VALUE && oParamsVector.back().uParam.value.bHasDefault) ||
			(oParamsVector.back().eType == Param::FLAG && oParamsVector.back().uParam.flag.bHasDefault) )
			throw std::runtime_error("Funkcja \"" + szName + "\": parametry domyœlne musz¹ byæ na koñcu listy");

	oParamsVector.push_back(Param(value, szParamName, szDesc));
	return oParamsVector.size() - 1;
}

size_t FunctEngine::Struct::addParam(Param::Flag flag, const std::string szParamName, const std::string szDesc) const
{
	// SprawdŸ, czy parametr o podanej nazwie ju¿ nie istnieje
	if (paramExists(szParamName))
		throw std::runtime_error("Funkcja \"" + szName + "\": ju¿ istnieje parametr o nazwie \"" + szParamName + "\"");

	// SprawdŸ, czy ostatnim parametrem nie jest zmienna lista argumentów
	if (!oParamsVector.empty() && oParamsVector.back().eType == Param::VA_LIST)
		throw std::runtime_error("Funkcja \"" + szName + "\": nie mo¿na dodaæ parametru po zmiennej liœcie parametrów");

	// SprawdŸ, czy obecny parametr ma wartoœæ domyœln¹, jeœli ostatni ma
	if (!flag.bHasDefault && !oParamsVector.empty())
		if ((oParamsVector.back().eType == Param::VALUE && oParamsVector.back().uParam.value.bHasDefault) ||
			(oParamsVector.back().eType == Param::FLAG && oParamsVector.back().uParam.flag.bHasDefault))
			throw std::runtime_error("Funkcja \"" + szName + "\": parametry domyœlne musz¹ byæ na koñcu listy");

	oParamsVector.push_back(Param(flag, szParamName, szDesc));
	return oParamsVector.size() - 1;
}

size_t FunctEngine::Struct::addParam(Param::VaList va_list, const std::string szParamName, const std::string szDesc) const
{
	// SprawdŸ, czy parametr o podanej nazwie ju¿ nie istnieje
	if (paramExists(szParamName))
		throw std::runtime_error("Funkcja \"" + szName + "\": ju¿ istnieje parametr o nazwie \"" + szParamName + "\"");

	// SprawdŸ, czy maksymalna liczba parametrów nie jest mniejsza od minimalnej
	if (va_list.usMaxParams <= va_list.usMinParams)
		throw std::runtime_error("Funkcja \"" + szName + "\": maksymalna liczba parametrów zmiennej listy musi byæ wiêksza od minimalnej");
		
	if (!oParamsVector.empty())
	{
		switch (oParamsVector.back().eType)
		{
		// SprawdŸ, czy nie dodano zmiennej listy argumentów po innej zmiennej liœcie argumentów
		case Param::VA_LIST:
			throw std::runtime_error("Funkcja \"" + szName + "\": ju¿ zdefiniowano zmienn¹ listê parametrów");
			break;

		// SprawdŸ, czy nie dodano zmiennej listy argumentów po parametrze z wartoœci¹ domyœln¹
		case Param::VALUE:
			if (oParamsVector.back().uParam.value.bHasDefault)
				throw std::runtime_error("Funkcja \"" + szName + "\": nie mo¿na dodaæ zmiennej listy parametrów po parametrze z wartoœci¹ domyœln¹");
			break;

		case Param::FLAG:
			if (oParamsVector.back().uParam.flag.bHasDefault)
				throw std::runtime_error("Funkcja \"" + szName + "\": nie mo¿na dodaæ zmiennej listy parametrów po parametrze z wartoœci¹ domyœln¹");
			break;
		}
	}

	oParamsVector.push_back(Param(va_list, szParamName, szDesc));
	return oParamsVector.size() - 1;
}


// Funkcja pobiera indeks parametru po jego nazwie; jeœli nie istnieje,
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

// Funkcja sprawdza, czy funkcja przyjmuje podan¹ iloœæ argumentów
//---------------------------------------------------------------------
bool FunctEngine::Struct::isNumParamsValid(size_t uiNum) const
{
	// Pusta lista parametrów - nie powinien byæ podany ¿aden
	if (oParamsVector.empty())
		return !uiNum;

	// Ostatni parmetr jest zmienn¹ list¹ argumentów - sprawdŸ dostêpny zakres paramterów
	if (oParamsVector.back().eType == Param::VA_LIST)
	{
		if (uiNum < oParamsVector.size() - 1 + oParamsVector.back().uParam.va_list.usMinParams)
			return false;
		else if (oParamsVector.back().uParam.va_list.usMaxParams != (unsigned short)(-1) && uiNum > oParamsVector.size() - 1 + oParamsVector.back().uParam.va_list.usMaxParams)
			return false;
		else
			return true;
	}

	// Czy uiNum nie przekracza liczby parametrów?
	if (uiNum > oParamsVector.size())
		return false;

	// Czy uiNum nie jest za ma³e?
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

// Funkcja automatycznie uzupe³nia wektor podany poprzez referencjê
// z parametrami o niepodane wartoœci domyœlne 
//---------------------------------------------------------------------
void FunctEngine::Struct::autoCompParams(std::vector<double> &dParamsVector) const
{
	// SprawdŸ, czy w ogóle trzeba uzupe³niaæ
	if (oParamsVector.empty() || oParamsVector.back().eType == Param::VA_LIST || dParamsVector.size() >= oParamsVector.size())
		return;

	auto iterLast = oParamsVector.begin() + dParamsVector.size();

	// SprawdŸ na wszelki wypadek, czy nie podano za ma³o argumentów
	switch ((*iterLast).eType)
	{
	case Param::VALUE:
		if (!(*iterLast).uParam.value.bHasDefault)
			throw std::runtime_error("Funkcja \"" + szName + "\": próba autouzupe³nienia parametru bez domyœlnej wartoœci");
		break;

	case Param::FLAG:
		if (!(*iterLast).uParam.flag.bHasDefault)
			throw std::runtime_error("Funkcja \"" + szName + "\": próba autouzupe³nienia parametru bez domyœlnej wartoœci");
		break;
	}

	// Iteruj po reszcie niepodanych parametrów i uzupe³nij
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

// Operator dostêpu do parametru po jego indeksie
//---------------------------------------------------------------------
const FunctEngine::Param & FunctEngine::Struct::operator[](size_t idx) const
{
	if (idx >= oParamsVector.size())
		throw std::runtime_error("Funkcja \"" + szName + "\": próba dostêpu do parametru o nieistniej¹cym indeksie");

	return oParamsVector.at(idx);
}

// Operator dostêpu do parametru po jego nazwie
//---------------------------------------------------------------------
const FunctEngine::Param & FunctEngine::Struct::operator[](const std::string & szName) const
{
	size_t idx = getParamIdx(szName);
	if (idx == nidx)
		throw std::runtime_error("Funkcja \"" + szName + "\": próba dostêpu do nieistniej¹cego parametru \"" + szName + "\"");

	return oParamsVector.at(idx);
}


// Funkcja dodaje funkcjê do bazy funkcji
//---------------------------------------------------------------------
// szName - nazwa funkcji
// pFunctPointer - wskaŸnik do funkcji przyjmuj¹cej wektor liczb
//   zmiennoprzecinkowych i zwracaj¹cej liczbê zmiennoprzecinkow¹
// szDesc - opis funkcji
// szAltNames - alternatywne nazwy funkcji w formacie "nazwa1;nazwa2"
//---------------------------------------------------------------------
size_t FunctEngine::add(const std::string & szName, FunctPtr pFunctPointer, const std::string & szDesc, const std::string & szAltNames)
{
	if (exists(szName))
		throw std::runtime_error("Funkcja \"" + szName + "\" ju¿ istnieje");

	if (pFunctPointer == nullptr)
		throw std::runtime_error("Funkcja \"" + szName + "\": próba przydzielenia pustego wskaŸnika funkcji");

	// Rozbij alternatywne nazwy
	std::vector<std::string> szAltNamesVector = pkutl::explode(szAltNames, ';');

	// Usuñ niepotrzebne bia³e znaki
	std::transform(szAltNamesVector.begin(), szAltNamesVector.end(), szAltNamesVector.begin(), pkutl::trim_ws);

	// Usuñ puste wpisy
	szAltNamesVector.erase(
		std::remove_if(szAltNamesVector.begin(), szAltNamesVector.end(), [](const std::string & alt_name) { return alt_name.empty(); }),
		szAltNamesVector.end() );

	// Usuñ duplikaty i sprawdŸ, czy któraœ z alternatycnych nazw nie jest zajêta
	for (auto i = szAltNamesVector.begin(); i != szAltNamesVector.end(); i++)
	{
		if (exists(*i))
			throw std::invalid_argument("Funkcja \"" + szName + "\": alternatywna nazwa \"" + *i + "\" ju¿ jest zajêta");

		for (auto j = i + 1; j != szAltNamesVector.end(); j++)
		{
			if (*i == *j)
			{
				szAltNamesVector.erase(j, j + 1);
				break;
			}
		}
	}

	// Dodaj funkcjê do bazy i zwróæ indeks
	oFunctVector.push_back(Struct(szName, pFunctPointer, szDesc, szAltNamesVector));
	return oFunctVector.size() - 1;
}

// Funkcja sprawdza, czy funkcja o podenej nazwie istnieje
//---------------------------------------------------------------------
bool FunctEngine::exists(const std::string & szName) const
{
	return getIdx(szName) != nidx;
}

// Funkcja pobiera indeks funkcji o podanej nazwie; jeœli nie istnieje,
// zwraca FunctEngine::nidx
//---------------------------------------------------------------------
size_t FunctEngine::getIdx(const std::string & szName) const
{
	for (auto i = oFunctVector.begin(); i != oFunctVector.end(); i++)
		if ((*i).szName == szName || std::find((*i).szAltNamesVector.begin(), (*i).szAltNamesVector.end(), szName) != (*i).szAltNamesVector.end())
			return i - oFunctVector.begin();

	return nidx;
}

// Operator dostêpu do funkcji po indeksie
//---------------------------------------------------------------------
const FunctEngine::Struct & FunctEngine::operator[](size_t idx)
{
	if (idx >= oFunctVector.size())
		throw std::runtime_error("Próba dostêpu do nieistniej¹cego indeksu funkcji");

	return oFunctVector.at(idx);
}

// Operator dostêpu do funkcji po nazwia
//---------------------------------------------------------------------
const FunctEngine::Struct & FunctEngine::operator[](const std::string & szName)
{
	size_t idx = getIdx(szName);
	if (idx == nidx)
		throw std::runtime_error("Próba dostêpu do nieistniej¹cej funkcji \"" + szName + "\"");

	return oFunctVector.at(idx);
}

// Konstuktor parametru "wartoœæ" bez wartoœci domyœlnej
//---------------------------------------------------------------------
FunctEngine::Param::Value::Value() :
	bHasDefault(false),
	dDefaultVal(0.0) 
{}


// Konstruktor parametru "wartoœæ" z wartoœci¹ domyœln¹
//---------------------------------------------------------------------
FunctEngine::Param::Value::Value(double dDefaultVal) :
	bHasDefault(true),
	dDefaultVal(dDefaultVal)
{}

// Konstruktor parametru "flaga" bez wartoœci domyœlnej
//---------------------------------------------------------------------
FunctEngine::Param::Flag::Flag(const std::string & szFlags, const std::string & szDefaultFlag) :
	bHasDefault(false),
	uiDefaultIdx(VarEngine::nidx)
{
	// Rozbij listê flag
	std::vector<std::string> szFlagsVector = pkutl::explode(szFlags, ';');

	// Usuñ bia³e znaki z koñców flag
	std::transform(szFlagsVector.begin(), szFlagsVector.end(), szFlagsVector.begin(), pkutl::trim_ws);

	// Usuñ puste wpisy
	szFlagsVector.erase(
		std::remove_if(szFlagsVector.begin(), szFlagsVector.end(), [](const std::string & flag) {return flag.empty(); }),
		szFlagsVector.end()
	);

	// SprawdŸ, czy wszystkie flagi istniej¹ i utwórz set z indeksami
	std::transform(
		szFlagsVector.begin(),
		szFlagsVector.end(),
		std::inserter(uiFlagIdxSet, uiFlagIdxSet.begin()),
		[](const std::string & flag) {
			size_t idx = gVarEngine.getIdx(flag);
			if (idx == nidx)
				throw std::invalid_argument("Próba utworzenia parametru z nieistniej¹c¹ flagê \"" + flag + "\"");
			else if (gVarEngine[idx].type() != VarEngine::FLAG_VAR)
				throw std::invalid_argument("Próba utworzenia parametru ze zmienn¹ \"" + flag + "\", która nie jest flag¹");
			return idx;
	});
	
	// SprawdŸ, czy domyœlna flaga istnieje i jest na liœcie flag
	if (!szDefaultFlag.empty())
	{
		size_t idx = gVarEngine.getIdx(szDefaultFlag);

		// SprawdŸ, czy flaga istnieje i czy jest na liœcie dostêpnych
		if (idx == VarEngine::nidx || std::find(uiFlagIdxSet.begin(), uiFlagIdxSet.end(), idx) == uiFlagIdxSet.end())
			throw std::invalid_argument("Domyœlnej flagi \"" + szDefaultFlag + "\" nie ma na liœcie wszystkich flag");

		uiDefaultIdx = idx;
		bHasDefault = true;
	}
}

// Konstruktor parametru "zmienna lista argumentów"
//---------------------------------------------------------------------
FunctEngine::Param::VaList::VaList(unsigned short usMinParams, unsigned short usMaxParams) :
	usMinParams(usMinParams),
	usMaxParams(usMaxParams) {}

// Destruktor klasy Param - usuwa odpowiedni obiekt w unii
//---------------------------------------------------------------------
FunctEngine::Param::~Param()
{
	// Usuñ adekwatny obiekt parametru
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

// Konstruktor kopiuj¹cy parametru - kopiuje odpowiedni obiekt unii
//---------------------------------------------------------------------
FunctEngine::Param::Param(const Param & other) :
	szName(other.szName),
	szDesc(other.szDesc),
	eType(other.eType)
{
	// Skopiuj adekwatny obiekt parametru uprzednio wywo³uj¹c konstruktor na odpowiednim obiekcie unii
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

	// Przenieœ adekwatny obiekt parametru uprzednio wywo³uj¹c konstruktor na odpowiednim obiekcie unii
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
	// Wywo³aj konstruktor na obiekcie value unii i przypisz przekazany parametr
	new(&uParam.value) Value;
	uParam.value = value;
}

FunctEngine::Param::Param(const Flag & flag, const std::string szName, const std::string szDesc) :
	szName(szName),
	szDesc(szDesc),
	eType(FLAG)
{
	// Wywo³aj konstruktor na obiekcie flag unii i przypisz przekazany parametr
	new(&uParam.flag) Flag;
	uParam.flag = flag;
}

FunctEngine::Param::Param(const VaList & va_list, const std::string szName, const std::string szDesc) :
	szName(szName),
	szDesc(szDesc),
	eType(VA_LIST)
{
	// Wywo³aj konstruktor na obiekcie va_list unii i przypisz przekazany parametr
	new(&uParam.va_list) VaList;
	uParam.va_list = va_list;
}