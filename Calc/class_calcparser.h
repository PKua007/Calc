// class_calcparser.h
//---------------------------------------------------------------------
// Plik nag��wkowy z klas� parsera
//---------------------------------------------------------------------
// Obiekty przyjmuj� ci�g znak�w do sparsowania i tworz� gotowe drzewo
// wyra�enia, kt�re mo�e zosta� pobrane. Je�li wyst�pj� b��dy podczas
// parsingu, wyrzucane s� wyj�tki.
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

#define MAX_EXP_LENGTH	1000		// Maksymalna d�ugo�� wyra�enia


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
	std::string			szExp{ "" };		// Wyra�enie do zinterpretowania

	size_t				uiCurrIdx = std::string::npos;		// Obecny indeks w ci�gu znak�w
	size_t				uiOffset = std::string::npos;		// Przesuni�cie indeksu
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