// error_handling.h
//---------------------------------------------------------------------
// Plik nag��wkowy z klas� b��d�w interpretacji wyra�e�
//---------------------------------------------------------------------
// (C) 2016 PKua
//---------------------------------------------------------------------

#pragma once

#include <sstream>
#include <vector>
#include <algorithm>
#include <cfenv>

#include "pkua_utils.h"


// Definicje rodzaj�w kontrolowanych b��d�w fenv
//-----------------------------------------------------------------------------------------

#define FENV_RANGE			FE_OVERFLOW | FE_UNDERFLOW		// B��d wyj�cia poza zakres double
#define	FENV_DOMAIN			FE_INVALID						// Poza dziedzin�
#define	FENV_DIVBYZERO		FE_DIVBYZERO					// Dzielenie przez zero
#define FENV_DOMANDZERO		FENV_DOMAIN | FENV_DIVBYZERO	// Poza dziedzin� dla funkcji, kt�re mog� wyrzuca� FE_DIVBYZERO dla niekt�rych warto�ci spoza dziedziny (np std::tgamma)
#define FENV_ALL			FENV_RANGE | FENV_DOMAIN | FENV_DIVBYZERO


// Klasa CalcError
//---------------------------------------------------------------------------------
// Obiekty tej klasy s� wyrzucane jako wyj�tki w klasach interpretujacych wyra�enia
// Klasa zapewnia tak�e integraln� obs�ug� b��d�w fenv
//---------------------------------------------------------------------------------

class CalcError
{
public:
	 // Wyliczenie rodzaj�w b��d�w
	enum CALC_ERROR_TYPE
	{
		 // Podstawowe
		NO,						// Brak b��du
		UNKNOWN,				// Nieznany b��d

		 // Parser
		UNEXPECTED_END,			// Niespodziewany koniec wyra�enia
		UNEXPECTED_SIGN,		// Niespodziewany znak w wyra�eniu
		EXPECTED_SIGN,			// Brak spodziewanego znaku w wyra�eniu
		EMPTY_BRACKETS,			// Puste nawiasy
		NOT_CLOSED_BRACKET,		// Niedomkni�te nawiasy
		TOO_LONG,				// Za d�ugie wyra�enie
		EMPTY_PARAM,			// Pusty parametr funkcji
		PASRE_VAL_OUT_OF_RANGE,	// Za du�a lub za ma�a liczba w parserze

		 // Interpreter
		DIV_ZERO,				// Dzielenie przez 0
		OUT_OF_DOMAIN,			// Poza dziedzin�
		INVALID_POWER,			// Niew�a�ciwe pot�gowanie
		RANGE_OVERFLOW,			// Za du�a liczba
		RANGE_UNDERFLOW,		// Za ma�a liczba
		UNKNOWN_VAR,			// Nieznana zmienna
		UNKNOWN_FUNCT,			// Nieznana funkcja
		BAD_PARAM_NUM,			// Z�a liczba paramet�w
		EXPECTED_FLAG,			// Spodziewana flaga funkcji w parametrze
		BAD_FLAG,				// Niepoprawna flaga funkcji w parametrze
		INDETERM,				// Symbol nieoznaczony
		EXPECTED_INT,			// Podano liczb� nieca�kowit�
		NEGATIVE_ROOT,			// Pierwiastek parzystego stopnia liczby ujemnej
		BAD_LOG_BASE,			// Niepoprawna podstawa logarytmu
		EXPECTED_POSITIVE,		// Spodziewano si� liczby dodatniej
		EXPECTED_NEGATIVE,		// Spodziewano si� liczby ujemnej
		EXPECTED_NPOSITIVE,		// Spodziewano si� liczby niedodatniej
		EXPECTED_NNEGATIVE,		// Spodziewano si� liczby nieujemnej
		EXPECTED_POSITIVE_INT,	// Spodziewano si� liczby dodatniej
		EXPECTED_NEGATIVE_INT,	// Spodziewano si� liczby ujemnej
		EXPECTED_NPOSITIVE_INT,	// Spodziewano si� liczby niedodatniej
		EXPECTED_NNEGATIVE_INT	// Spodziewano si� liczby nieujemnej
	};
	
	 // Tablica nazw b��d�w
	static constexpr char * szName[] =
	{
		"NO",
		"UNKNOWN",
		"UNEXPECTED_END",
		"UNEXPECTED_SIGN",
		"EXPECTED_SIGN",
		"EMPTY_BRACKETS",
		"NOT_CLOSED_BRACKET",
		"TOO_LONG",
		"EMPTY_PARAM",
		"PASRE_VAL_OUT_OF_RANGE",

		"DIV_ZERO",
		"OUT_OF_DOMAIN",
		"INVALID_POWER",
		"RANGE_OVERFLOW",
		"RANGE_UNDERFLOW",
		"UNKNOWN_VAR",
		"UNKNOWN_FUNCT",
		"BAD_PARAM_NUM",
		"EXPECTED_FLAG",
		"BAD_FLAG",
		"INDETERM",
		"EXPECTED_INT",
		"NEGATIVE_ROOT",
		"BAD_LOG_BASE",
		"EXPECTED_POSITIVE",
		"EXPECTED_NEGATIVE",
		"EXPECTED_NPOSITIVE",
		"EXPECTED_NNEGATIVE",
		"EXPECTED_POSITIVE_INT",
		"EXPECTED_NEGATIVE_INT",
		"EXPECTED_NPOSITIVE_INT",
		"EXPECTED_NNEGATIVE_INT"
	};

private:

	 // Struktura b��du
	struct Struct
	{
		CALC_ERROR_TYPE		eType = NO;						// Typ b��du
		std::string			szMsg = "";						// Tre�� b��du
		size_t				uiBegIdx = std::string::npos;	// Pozycja pocz�tku b��du
		size_t				uiEndIdx = std::string::npos;	// Pozycja ko�ca b��du
	};

	std::vector<Struct>	oErrVector;			// Lista b��d�w
	static bool compareBegIdx(Struct sctErr1, Struct sctErr2);

public:
	CalcError(CALC_ERROR_TYPE eNewType, std::string szNewMsg, size_t uiNewBegIdx = std::string::npos, size_t uiNewEndIdx = std::string::npos);
	CalcError() {};
	void			appendError(CALC_ERROR_TYPE eNewType, std::string szNewMsg, size_t uiNewBegIdx = std::string::npos, size_t uiNewEndIdx = std::string::npos);
	void			appendError(CalcError oError);
	size_t			getNum(void) const;
	std::string		print() const;
	void			sortBegIdx();
	const Struct	&operator[](size_t idx) const;

	static void		clearFenv();
	static void		checkFenv(unsigned char err_types, std::string custom_message = "", size_t err_beg_idx = std::string::npos, size_t err_end_idx = std::string::npos);
}; 

typedef CalcError::CALC_ERROR_TYPE CalcErrType;


// Klasa CalcDevError
//-----------------------------------------------------------------------------------------
// Obiekty tej klasy s� wyrzucane jako wyj�tki przy niedozwolonym u�ywaniu klas kalkualtora
//-----------------------------------------------------------------------------------------

class CalcDevError
{
public:
	 // Wyliczenie rodzaj�w b��d�w
	enum CALC_DEV_ERROR_TYPE
	{
		NO,							// Brak b��du
		UNKNOWN,					// Nieznany b��d

		 // B��dy CalcTree
		EMPTY_CHILD_CALL,			// Pr�ba dost�pu do pustego dziecka
		DOUBLE_PARENT_ASSOC,		// Pr�ba podw�jnego przypisania rodzica
		PRINT_NOT_ROOT				// Wywo�anie metody przeliczenia indeks�w b��d�w nie na korzeniu 
	};

	 // Tablica nazw b��d�w
	static constexpr char * szName[] =
	{
		"NO",
		"UNKNOWN",
		"EMPTY_CHILD_ASSOC",
		"DOUBLE_PARENT_ASSOC",
		"PRINT_NOT_ROOT"
	};

	const CALC_DEV_ERROR_TYPE	eType;		// Typ b��du
	const std::string			szMsg;		// Wiadomo�� b��du

	CalcDevError() = delete;
	CalcDevError(CALC_DEV_ERROR_TYPE eType, std::string szMsg);

	std::string print();
};

typedef CalcDevError::CALC_DEV_ERROR_TYPE CalcDevErrType;