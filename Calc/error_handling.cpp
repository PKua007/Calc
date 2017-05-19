// error_handling.cpp
//---------------------------------------------------------------------
// Plik Ÿród³owy z klas¹ b³êdów interpretacji wyra¿eñ
//---------------------------------------------------------------------
// (C) 2016 PKua
//---------------------------------------------------------------------

#include "stdafx.h"
#include "error_handling.h"


//**************************************************************************************************************************************************
// Metody CalcError
//**************************************************************************************************************************************************


// Metoda s³u¿y do porównania indeksów pocz¹tku b³êdu przy sortowaniu rosn¹co
//--------------------------------------------------------------------------------------------------------------------------------------------------
bool CalcError::compareBegIdx(Struct sctErr1, Struct sctErr2)
{ 
	return sctErr1.uiBegIdx < sctErr2.uiBegIdx;
}

// Konstruktor klasy z podanym pierwszym b³êdem do zapisania
//--------------------------------------------------------------------------------------------------------------------------------------------------
CalcError::CalcError(CALC_ERROR_TYPE eNewType, std::string szNewMsg, size_t uiNewBegIdx, size_t uiNewEndIdx)
{
	appendError(eNewType, szNewMsg, uiNewBegIdx, uiNewEndIdx);
}

// Metoda dodaje nowy b³¹d do listy o podanych parametrach
//--------------------------------------------------------------------------------------------------------------------------------------------------
void CalcError::appendError(CALC_ERROR_TYPE eNewType, std::string szNewMsg, size_t uiNewBegIdx, size_t uiNewEndIdx)
{
	Struct sctNewError;
	sctNewError.eType = eNewType;
	sctNewError.szMsg = szNewMsg;
	sctNewError.uiBegIdx = uiNewBegIdx;
	sctNewError.uiEndIdx = uiNewEndIdx;

	oErrVector.push_back(sctNewError);
}

// Metoda do³¹cza do listy b³êdów inny obiekt listy
//-------------------------------------------------------------------------------
void CalcError::appendError(CalcError oError)
{
	oErrVector.insert(oErrVector.begin(), oError.oErrVector.begin(), oError.oErrVector.end());
}

// Metoda zwraca liczbê b³êdów na liœcie
//-------------------------------------------------------------------------------
size_t CalcError::getNum() const
{
	return oErrVector.size();
}

// Metoda zwraca ci¹g znaków z reprezentacj¹ listy b³êdów
//-------------------------------------------------------
std::string CalcError::print() const
{
	std::stringstream szPrintStream;
	std::string szPrint;
	
	if (oErrVector.empty())
		return std::string("{puste}");

	for (auto const & i : oErrVector)
		szPrintStream << "CalcError {eType: " << szName[i.eType] 
			<< ", szMsg: \"" << i.szMsg
			<< "\", uiBegIdx: " << ((i.uiBegIdx == std::string::npos) ? "[brak]" : std::to_string(i.uiBegIdx))
			<< ", uiEndIdx: " << ((i.uiEndIdx == std::string::npos) ? "[brak]" : std::to_string(i.uiEndIdx))
			<< "}\n";

	szPrint = szPrintStream.str();
	szPrint.pop_back();		// Usuñ niepotrzebny enter

	return szPrint;
}

// Metoda sortuje b³êdy wg. pocz¹tków rosn¹co
//--------------------------------------------------------
void CalcError::sortBegIdx()
{
	std::sort(oErrVector.begin(), oErrVector.end(), compareBegIdx);
}

// Operator [] pobrania b³êdu z danego indeksu
//--------------------------------------------------------
const CalcError::Struct & CalcError::operator[](size_t idx) const
{
	return oErrVector.at(idx);
}

// Funkcja czyœci ostatnie b³êdy œrodowiska zmiennoprzecinkowego
//--------------------------------------------------------
void CalcError::clearFenv()
{
	std::feclearexcept(FE_ALL_EXCEPT);
}

// Funkcja sprawdza b³êdy œrodowiska zmiennoprzecinkowego
// i w razie czego wyrzuca odpowiednie wyj¹tki CalcError
//--------------------------------------------------------
void CalcError::checkFenv(unsigned char err_types, std::string custom_message, size_t err_beg_idx, size_t err_end_idx)
{
	// SprawdŸ, czy wyst¹pi³ jakikolwiek b³¹d fenv z wymienionych
	if (!std::fetestexcept(err_types))
		return;

	// B³êdy zakresu
	if (err_types & FENV_RANGE)
	{
		if (fetestexcept(FE_OVERFLOW))
			throw CalcError(CalcError::RANGE_OVERFLOW, custom_message.empty() ? "przekroczono maksimum zakresu" : custom_message, err_beg_idx, err_end_idx);
		else if (fetestexcept(FE_UNDERFLOW))
			throw CalcError(CalcError::RANGE_UNDERFLOW, custom_message.empty() ? "przekroczono minimum zakresu" : custom_message, err_beg_idx, err_end_idx);
	}

	if (err_types & FENV_DOMAIN)
	{
		// B³êdy dziedziny (razem z divbyzero)
		if (err_types & FENV_DIVBYZERO)
		{
			if (fetestexcept(FE_DIVBYZERO | FE_INVALID))
				throw CalcError(CalcError::OUT_OF_DOMAIN, custom_message.empty() ? "parametr poza dziedzin¹ funkcji" : custom_message, err_beg_idx, err_end_idx);
		}
		// B³êdy dziedziny (bez divbyzero)
		else if (!(err_types & FENV_DIVBYZERO))
		{
			if (fetestexcept(FE_INVALID))
				throw CalcError(CalcError::OUT_OF_DOMAIN, custom_message.empty() ? "parametr poza dziedzin¹ funkcji" : custom_message, err_beg_idx, err_end_idx);
		}
	}

	// B³êdy dzielenia przez zero
	if (err_types & FENV_DIVBYZERO)
		if (fetestexcept(FE_DIVBYZERO))
			throw CalcError(CalcError::DIV_ZERO, custom_message.empty() ? "dzielenie przez 0" : custom_message, err_beg_idx, err_end_idx);
}


//**************************************************************************************************************************************************
// Metody CalcDevError
//**************************************************************************************************************************************************


// Konstruktor przyjmuj¹cy rodzaj b³êdu i wiadomoœæ
//--------------------------------------------------------
CalcDevError::CalcDevError(CALC_DEV_ERROR_TYPE eType, std::string szMsg) : eType(eType), szMsg(szMsg) {}

// Metoda generuje stringa reprezentuj¹cego zawartoœæ b³êdu
//--------------------------------------------------------
std::string CalcDevError::print()
{
	return std::string("CalcDevError {eType: ") + std::string(szName[eType]) + std::string(", szMsg: \"") + szMsg + std::string("\"}");
}
