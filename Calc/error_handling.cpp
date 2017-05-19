// error_handling.cpp
//---------------------------------------------------------------------
// Plik �r�d�owy z klas� b��d�w interpretacji wyra�e�
//---------------------------------------------------------------------
// (C) 2016 PKua
//---------------------------------------------------------------------

#include "stdafx.h"
#include "error_handling.h"


//**************************************************************************************************************************************************
// Metody CalcError
//**************************************************************************************************************************************************


// Metoda s�u�y do por�wnania indeks�w pocz�tku b��du przy sortowaniu rosn�co
//--------------------------------------------------------------------------------------------------------------------------------------------------
bool CalcError::compareBegIdx(Struct sctErr1, Struct sctErr2)
{ 
	return sctErr1.uiBegIdx < sctErr2.uiBegIdx;
}

// Konstruktor klasy z podanym pierwszym b��dem do zapisania
//--------------------------------------------------------------------------------------------------------------------------------------------------
CalcError::CalcError(CALC_ERROR_TYPE eNewType, std::string szNewMsg, size_t uiNewBegIdx, size_t uiNewEndIdx)
{
	appendError(eNewType, szNewMsg, uiNewBegIdx, uiNewEndIdx);
}

// Metoda dodaje nowy b��d do listy o podanych parametrach
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

// Metoda do��cza do listy b��d�w inny obiekt listy
//-------------------------------------------------------------------------------
void CalcError::appendError(CalcError oError)
{
	oErrVector.insert(oErrVector.begin(), oError.oErrVector.begin(), oError.oErrVector.end());
}

// Metoda zwraca liczb� b��d�w na li�cie
//-------------------------------------------------------------------------------
size_t CalcError::getNum() const
{
	return oErrVector.size();
}

// Metoda zwraca ci�g znak�w z reprezentacj� listy b��d�w
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
	szPrint.pop_back();		// Usu� niepotrzebny enter

	return szPrint;
}

// Metoda sortuje b��dy wg. pocz�tk�w rosn�co
//--------------------------------------------------------
void CalcError::sortBegIdx()
{
	std::sort(oErrVector.begin(), oErrVector.end(), compareBegIdx);
}

// Operator [] pobrania b��du z danego indeksu
//--------------------------------------------------------
const CalcError::Struct & CalcError::operator[](size_t idx) const
{
	return oErrVector.at(idx);
}

// Funkcja czy�ci ostatnie b��dy �rodowiska zmiennoprzecinkowego
//--------------------------------------------------------
void CalcError::clearFenv()
{
	std::feclearexcept(FE_ALL_EXCEPT);
}

// Funkcja sprawdza b��dy �rodowiska zmiennoprzecinkowego
// i w razie czego wyrzuca odpowiednie wyj�tki CalcError
//--------------------------------------------------------
void CalcError::checkFenv(unsigned char err_types, std::string custom_message, size_t err_beg_idx, size_t err_end_idx)
{
	// Sprawd�, czy wyst�pi� jakikolwiek b��d fenv z wymienionych
	if (!std::fetestexcept(err_types))
		return;

	// B��dy zakresu
	if (err_types & FENV_RANGE)
	{
		if (fetestexcept(FE_OVERFLOW))
			throw CalcError(CalcError::RANGE_OVERFLOW, custom_message.empty() ? "przekroczono maksimum zakresu" : custom_message, err_beg_idx, err_end_idx);
		else if (fetestexcept(FE_UNDERFLOW))
			throw CalcError(CalcError::RANGE_UNDERFLOW, custom_message.empty() ? "przekroczono minimum zakresu" : custom_message, err_beg_idx, err_end_idx);
	}

	if (err_types & FENV_DOMAIN)
	{
		// B��dy dziedziny (razem z divbyzero)
		if (err_types & FENV_DIVBYZERO)
		{
			if (fetestexcept(FE_DIVBYZERO | FE_INVALID))
				throw CalcError(CalcError::OUT_OF_DOMAIN, custom_message.empty() ? "parametr poza dziedzin� funkcji" : custom_message, err_beg_idx, err_end_idx);
		}
		// B��dy dziedziny (bez divbyzero)
		else if (!(err_types & FENV_DIVBYZERO))
		{
			if (fetestexcept(FE_INVALID))
				throw CalcError(CalcError::OUT_OF_DOMAIN, custom_message.empty() ? "parametr poza dziedzin� funkcji" : custom_message, err_beg_idx, err_end_idx);
		}
	}

	// B��dy dzielenia przez zero
	if (err_types & FENV_DIVBYZERO)
		if (fetestexcept(FE_DIVBYZERO))
			throw CalcError(CalcError::DIV_ZERO, custom_message.empty() ? "dzielenie przez 0" : custom_message, err_beg_idx, err_end_idx);
}


//**************************************************************************************************************************************************
// Metody CalcDevError
//**************************************************************************************************************************************************


// Konstruktor przyjmuj�cy rodzaj b��du i wiadomo��
//--------------------------------------------------------
CalcDevError::CalcDevError(CALC_DEV_ERROR_TYPE eType, std::string szMsg) : eType(eType), szMsg(szMsg) {}

// Metoda generuje stringa reprezentuj�cego zawarto�� b��du
//--------------------------------------------------------
std::string CalcDevError::print()
{
	return std::string("CalcDevError {eType: ") + std::string(szName[eType]) + std::string(", szMsg: \"") + szMsg + std::string("\"}");
}
