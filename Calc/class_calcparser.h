// class_calcparser.h
//---------------------------------------------------------------------
// Plik nag³ówkowy z klas¹ parsera
//---------------------------------------------------------------------
// Obiekty przyjmuj¹ ci¹g znaków do sparsowania i tworz¹ gotowe drzewo
// wyra¿enia, które mo¿e zostaæ pobrane. Jeœli wyst¹pj¹ b³êdy podczas
// parsingu, wyrzucane s¹ wyj¹tki.
//---------------------------------------------------------------------
// (C) 2016 PKua
//---------------------------------------------------------------------

#pragma once

#include <string>
#include <sstream>
#include <ctype.h>

#include "error_handling.h"
#include "class_calctree.h"
#include "var_engine.h"
#include "funct_engine.h"
#include "pkua_utils.h"

#define MAX_EXP_LENGTH	1000		// Maksymalna d³ugoœæ wyra¿enia


 // Wyliczenie statusu poboru liczby
enum CALC_GETVAL_STATUS
{
	GV_STAT_INIT,
	GV_STAT_DEC,
	GV_STAT_DELIM,
	GV_STAT_FRAC,
	GV_STAT_E_DELIM,
	GV_STAT_EXP_SIGN,
	GV_STAT_EXP,
	GV_STAT_EXIT_SPACE
};

// Klasa CalcParser
//---------------------------------------------------------------------
class CalcParser
{
private:
	std::string			szExp{ "" };		// Wyra¿enie do zinterpretowania

	size_t				uiCurrIdx = std::string::npos;		// Obecny indeks w ci¹gu znaków
	size_t				uiOffset = std::string::npos;		// Przesuniêcie indeksu
	size_t				uiIdxSave = std::string::npos;		// Zapisany indeks
	
	void				insertExp(const std::string & szNewExp, size_t uiNewOffset);
	CalcTree *			getVal(CalcTree * &opInnerPointer);
	double				countVal(double dMantissa, double dExp);
	void				skipBracketExp();
	inline std::string	fetchBracketExp();
	CalcTree *			getVarFunct();
	bool				checkSign();
	CALC_NODE_TYPE		getOp();

public:
	CalcParser() = default;
	~CalcParser() = default;

	void				insertExp(const std::string & szNewExp);
	const std::string	&getExp();
	CalcTree *			parse();
};