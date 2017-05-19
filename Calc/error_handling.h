// error_handling.h
//---------------------------------------------------------------------
// Plik nag³ówkowy z klas¹ b³êdów interpretacji wyra¿eñ
//---------------------------------------------------------------------
// (C) 2016 PKua
//---------------------------------------------------------------------

#pragma once

#include <sstream>
#include <vector>
#include <algorithm>
#include <cfenv>

#include "pkua_utils.h"


// Definicje rodzajów kontrolowanych b³êdów fenv
//-----------------------------------------------------------------------------------------

#define FENV_RANGE			FE_OVERFLOW | FE_UNDERFLOW		// B³¹d wyjœcia poza zakres double
#define	FENV_DOMAIN			FE_INVALID						// Poza dziedzin¹
#define	FENV_DIVBYZERO		FE_DIVBYZERO					// Dzielenie przez zero
#define FENV_DOMANDZERO		FENV_DOMAIN | FENV_DIVBYZERO	// Poza dziedzin¹ dla funkcji, które mog¹ wyrzucaæ FE_DIVBYZERO dla niektórych wartoœci spoza dziedziny (np std::tgamma)
#define FENV_ALL			FENV_RANGE | FENV_DOMAIN | FENV_DIVBYZERO


// Klasa CalcError
//---------------------------------------------------------------------------------
// Obiekty tej klasy s¹ wyrzucane jako wyj¹tki w klasach interpretujacych wyra¿enia
// Klasa zapewnia tak¿e integraln¹ obs³ugê b³êdów fenv
//---------------------------------------------------------------------------------

class CalcError
{
public:
	 // Wyliczenie rodzajów b³êdów
	enum CALC_ERROR_TYPE
	{
		 // Podstawowe
		NO,						// Brak b³êdu
		UNKNOWN,				// Nieznany b³¹d

		 // Parser
		UNEXPECTED_END,			// Niespodziewany koniec wyra¿enia
		UNEXPECTED_SIGN,		// Niespodziewany znak w wyra¿eniu
		EXPECTED_SIGN,			// Brak spodziewanego znaku w wyra¿eniu
		EMPTY_BRACKETS,			// Puste nawiasy
		NOT_CLOSED_BRACKET,		// Niedomkniête nawiasy
		TOO_LONG,				// Za d³ugie wyra¿enie
		EMPTY_PARAM,			// Pusty parametr funkcji
		PASRE_VAL_OUT_OF_RANGE,	// Za du¿a lub za ma³a liczba w parserze

		 // Interpreter
		DIV_ZERO,				// Dzielenie przez 0
		OUT_OF_DOMAIN,			// Poza dziedzin¹
		INVALID_POWER,			// Niew³aœciwe potêgowanie
		RANGE_OVERFLOW,			// Za du¿a liczba
		RANGE_UNDERFLOW,		// Za ma³a liczba
		UNKNOWN_VAR,			// Nieznana zmienna
		UNKNOWN_FUNCT,			// Nieznana funkcja
		BAD_PARAM_NUM,			// Z³a liczba parametów
		EXPECTED_FLAG,			// Spodziewana flaga funkcji w parametrze
		BAD_FLAG,				// Niepoprawna flaga funkcji w parametrze
		INDETERM,				// Symbol nieoznaczony
		EXPECTED_INT,			// Podano liczbê nieca³kowit¹
		NEGATIVE_ROOT,			// Pierwiastek parzystego stopnia liczby ujemnej
		BAD_LOG_BASE,			// Niepoprawna podstawa logarytmu
		EXPECTED_POSITIVE,		// Spodziewano siê liczby dodatniej
		EXPECTED_NEGATIVE,		// Spodziewano siê liczby ujemnej
		EXPECTED_NPOSITIVE,		// Spodziewano siê liczby niedodatniej
		EXPECTED_NNEGATIVE,		// Spodziewano siê liczby nieujemnej
		EXPECTED_POSITIVE_INT,	// Spodziewano siê liczby dodatniej
		EXPECTED_NEGATIVE_INT,	// Spodziewano siê liczby ujemnej
		EXPECTED_NPOSITIVE_INT,	// Spodziewano siê liczby niedodatniej
		EXPECTED_NNEGATIVE_INT	// Spodziewano siê liczby nieujemnej
	};
	
	 // Tablica nazw b³êdów
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

	 // Struktura b³êdu
	struct Struct
	{
		CALC_ERROR_TYPE		eType = NO;						// Typ b³êdu
		std::string			szMsg = "";						// Treœæ b³êdu
		size_t				uiBegIdx = std::string::npos;	// Pozycja pocz¹tku b³êdu
		size_t				uiEndIdx = std::string::npos;	// Pozycja koñca b³êdu
	};

	std::vector<Struct>	oErrVector;			// Lista b³êdów
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
// Obiekty tej klasy s¹ wyrzucane jako wyj¹tki przy niedozwolonym u¿ywaniu klas kalkualtora
//-----------------------------------------------------------------------------------------

class CalcDevError
{
public:
	 // Wyliczenie rodzajów b³êdów
	enum CALC_DEV_ERROR_TYPE
	{
		NO,							// Brak b³êdu
		UNKNOWN,					// Nieznany b³¹d

		 // B³êdy CalcTree
		EMPTY_CHILD_CALL,			// Próba dostêpu do pustego dziecka
		DOUBLE_PARENT_ASSOC,		// Próba podwójnego przypisania rodzica
		PRINT_NOT_ROOT				// Wywo³anie metody przeliczenia indeksów b³êdów nie na korzeniu 
	};

	 // Tablica nazw b³êdów
	static constexpr char * szName[] =
	{
		"NO",
		"UNKNOWN",
		"EMPTY_CHILD_ASSOC",
		"DOUBLE_PARENT_ASSOC",
		"PRINT_NOT_ROOT"
	};

	const CALC_DEV_ERROR_TYPE	eType;		// Typ b³êdu
	const std::string			szMsg;		// Wiadomoœæ b³êdu

	CalcDevError() = delete;
	CalcDevError(CALC_DEV_ERROR_TYPE eType, std::string szMsg);

	std::string print();
};

typedef CalcDevError::CALC_DEV_ERROR_TYPE CalcDevErrType;