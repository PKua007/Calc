// class_calctree.cpp
//---------------------------------------------------------------------
// Plik Ÿrod³owy z klas¹ reprezentuj¹c¹ drzewo wyra¿enia
//---------------------------------------------------------------------
// Obiekty przechowuj¹ wyra¿enie w postaci drzewo. Metody pozwalaj¹
// na obliczanie wyra¿enia, wydrukowanie go w postaci ci¹gu znaków,
// zapisywane s¹ tak¿e informacje o b³êdach logicznych
// (nieznane funkcje, zmienne, wartoœci spoza dziedziny, itd.)
//---------------------------------------------------------------------
// (C) 2016 PKua
//---------------------------------------------------------------------

#include "stdafx.h"
#include "class_calctree.h"
#include <assert.h>
#include "main_func.h"


//**********************************************************************
// Metody CalcTree
//**********************************************************************


// Konstruktor klasy bazowej okreœlaj¹cy parametry wêz³a
//----------------------------------------------------------------------
CalcTree::CalcTree(CALC_NODE_TYPE eNodeType) : eNodeType(eNodeType), usOpPrecedence(CALC_NODE_DATA[eNodeType].usOpPrec), eOpAssoc(CALC_NODE_DATA[eNodeType].eOpAssoc) {}

// Metoda zwraca sta³¹ referencjê do typu wêz³a
//----------------------------------------------------------------------
const CALC_NODE_TYPE & CalcTree::getType() const
{
	return eNodeType;
}

// Metoda zwraca sta³¹ referencjê do rzêdu operatora
//----------------------------------------------------------------------
const unsigned short & CalcTree::getOpPrecedence() const
{
	return usOpPrecedence;
}

// Metoda zwraca sta³¹ referencjê do typu operatora
//-----------------------------------------------------------------------
const CALC_OP_ASSOC & CalcTree::getOpAssocType() const
{
	return eOpAssoc;
}

// Metoda zwraca sta³y wskaŸnik do rodzica
//----------------------------------------------------------------------- 
CalcTree * CalcTree::getParent() const
{
	return opParent;
}

// Metoda wyœwietla reprezentacjê wyra¿enia w postaci ci¹gu znaków
// i ustala w niej indeksy koñców i pocz¹tków wêz³ów (do b³edów)
//-----------------------------------------------------------------------
std::string CalcTree::print(CALC_PRINT_MODE eMode)
{
	if (opParent != nullptr)
		throw CalcDevError(CalcDevError::PRINT_NOT_ROOT, "metodê print() mo¿na wywo³aæ tylko na korzeniu");

	size_t idx = 0U;
	return printIntern(eMode, idx);
}


//**********************************************************************************************************************
// IMPLEMENTACJE METOD DLA WSYSTKICH WÊZ£ÓW
//
//    checkSymbol ()
// Dla wêz³a typu NODE_VAR lub NODE_FUNCT sprawdza, czy zmienna/funkcja o podanej nazwie istnieje i jak nie, dodaje
// flagê b³êdu. Wywo³ujê metodê checkSymbols na wszystkich dzieciach
//
//    count ()
// Oblicza wartoœæ wêz³a - wartoœci dzieci oblicza poprzez wywo³anie count () na nich
//
//    printIntern(CALC_PRINT_MODE eMode, size_t & idx)
// Generuje reprezentacjê wêz³a w postaci ci¹gu znaków - reprezentacjê dzieci pobiera poprzez wywo³anie printIntern na nich.
// Dostêpne tryby - liniowy (PRINT_INLINE), blokowy (PRINT_BLOCK), blokowy z klamrami (PRINT_BLOCK_BRACKETS)
//
//    appendNode(CalcTree * opNode)
// Jeœli wêze³ ma jedno dziecko, podmienia je danym (i w razie potrzeby usuwa stare), a jeœli dwa, to podmienia drugie.
// Wêz³y NODE_VALUE, NODE_VAR, NODE_FUNCT i NODE_BRACKETS nie obs³uguj¹ metody i nie powinna byæ wywo³ana.
//
//    detachNode()
// Jeœli wêze³ ma jedno dziecko, odpina je, a jeœli dwa, to odpina drugie. Zwraca wskaŸnik odpiêtego.
// Wêz³y NODE_VALUE, NODE_VAR, NODE_FUNCT i NODE_BRACKETS nie obs³uguj¹ metody i nie powinna byæ wywo³ana.
//*********************************************************************************************************************


// Metody CalcTreeEmpty
//---------------------------------------------------------------------

CalcTreeEmpty::CalcTreeEmpty() : CalcTree(NODE_EMPTY) {}

void CalcTreeEmpty::checkSymbols()
{
	throw CalcDevError(CalcDevError::EMPTY_CHILD_CALL, "próba dostêpu do pustego wêz³a (metoda checkSymbols)");
}

double CalcTreeEmpty::count()
{
	throw CalcDevError(CalcDevError::EMPTY_CHILD_CALL, "próba dostêpu do pustego wêz³a (metoda count)");
	return 0.0;
}

std::string CalcTreeEmpty::printIntern(CALC_PRINT_MODE eMode, size_t & idx)
{
	return std::string(eMode == PRINT_INLINE ? "{brak}" : "PUSTY WÊZE£");
}

void CalcTreeEmpty::appendNode(CalcTree * opNode)
{
	throw CalcDevError(CalcDevError::EMPTY_CHILD_CALL, "próba dostêpu do pustego wêz³a (metoda appendNode)");
}

CalcTree * CalcTreeEmpty::detachNode()
{
	throw CalcDevError(CalcDevError::EMPTY_CHILD_CALL, "próba dostêpu do pustego wêz³a (metoda detachNode)");
	return nullptr;
}


// Metody CalcTreeValue
//---------------------------------------------------------------------


// Konstruktor przyjmuj¹cy wartoœæ
//---------------------------------------------------------------------
CalcTreeValue::CalcTreeValue(double dValue) : CalcTree(NODE_VALUE), dValue(dValue) {}

void CalcTreeValue::checkSymbols()
{
	return;
}

double CalcTreeValue::count()
{ 
	//throw CalcError(CalcError::UNKNOWN, "wartoœæ", uiNodeBegIdx, uiNodeEndIdx);
	return dValue;
}

std::string CalcTreeValue::printIntern(CALC_PRINT_MODE eMode, size_t & idx)
{
	std::stringstream szValueStream;

	switch (eMode)
	{
	case PRINT_INLINE:
		uiNodeBegIdx = idx;
		szValueStream << FormatedNumber(dValue, ucPrecision, eNotation);		idx += szValueStream.str().length();
		uiNodeEndIdx = idx - 1;
		return szValueStream.str();

	case PRINT_BLOCK:
	case PRINT_BLOCK_BRACKETS:
		szValueStream << "Wartoœæ(" << dValue << ")";
		return szValueStream.str();
	}
	return std::string();
}

void CalcTreeValue::appendNode(CalcTree * opNode)
{
	return;
}

CalcTree * CalcTreeValue::detachNode()
{
	return nullptr;
}


// Metody CalcTreeVar
//---------------------------------------------------------------------


// Konstruktor przyjmuj¹cy nazwê zmiennej
//---------------------------------------------------------------------
CalcTreeVar::CalcTreeVar(std::string szName) : CalcTree(NODE_VAR), szName(szName) {}

void CalcTreeVar::checkSymbols()
{
	if (!gVarEngine.exists(szName))
		throw CalcError(CalcError::UNKNOWN_VAR, "nieznana zmienna " + szName);
}

double CalcTreeVar::count()
{
	size_t idx;

	 // SprawdŸ, czy zmienna istnieje i czy nie ma próby u¿ycia flagi
	if ((idx = gVarEngine.getIdx(szName)) == VarEngine::nidx || gVarEngine[idx].type() == VarEngine::FLAG_VAR)
		throw CalcError(CalcError::UNKNOWN_VAR, "nieznana zmienna \"" + szName + "\"", uiNodeBegIdx, uiNodeEndIdx);

	return gVarEngine[idx].value();
}

std::string CalcTreeVar::printIntern(CALC_PRINT_MODE eMode, size_t & idx)
{
	switch (eMode)
	{
	case PRINT_INLINE:
		uiNodeBegIdx = idx;
		idx += szName.length();
		uiNodeEndIdx = idx - 1;

		return szName;

	case PRINT_BLOCK:
	case PRINT_BLOCK_BRACKETS:
		return std::string("Zmienna(") + szName + (")");
	}
	return std::string();
}

void CalcTreeVar::appendNode(CalcTree * opNode)
{
	return;
}

CalcTree * CalcTreeVar::detachNode()
{
	return nullptr;
}


// Metody CalcTreeFunct
//---------------------------------------------------------------------


// Konstruktor przyjmuj¹cy nazwê funkcji, tablicê wskaŸników do parametrów i liczbê parametrów
//--------------------------------------------------------------------------------------------
CalcTreeFunct::CalcTreeFunct(std::string _szName, std::vector<CalcTree*> _opParamsVector) : CalcTree(NODE_FUNCT), szName(_szName), opParamsVector(_opParamsVector)
{
	// Zamieñ nazwê funkcji na g³ówn¹, jeœli podano jedn¹ z alternatywnych
	size_t idx = gFunctEngine.getIdx(szName);
	if (idx != FunctEngine::nidx)
		szName = gFunctEngine[idx].szName;
}

void CalcTreeFunct::checkSymbols()
{
	return;
}

double CalcTreeFunct::count()
{
	size_t idx;

	// SprawdŸ, czy funkcja istnieje
	if ((idx = gFunctEngine.getIdx(szName)) == FunctEngine::nidx)
		throw CalcError(CalcError::UNKNOWN_FUNCT, "nieznana funkcja \"" + szName + "\"", uiNodeBegIdx, uiNodeBegIdx + szName.length() - 1);

	// SprawdŸ, czy zgadza siê liczba parametrów (z uwzglêdnieniem domyœlnych)
	if (!gFunctEngine[idx].isNumParamsValid(opParamsVector.size()))
		throw CalcError(CalcError::BAD_PARAM_NUM, "z³a liczba parametrów funkcji; zobacz @flist " + szName, uiNodeBegIdx + szName.length(), uiNodeEndIdx);

	std::vector<double>	dParamVector(opParamsVector.size());
	CalcError			oErrors;
	size_t				uiValueIdx = 0U;
	size_t				uiParamIdx = 0U;
	size_t				uiFlagIdx;
	double				dResult;

	// Próbuj pobraæ wszystkie parametry - w razie czego dodawaj b³êdy
	for (auto const & i : opParamsVector)
	{
		switch (gFunctEngine[idx][uiParamIdx].eType)
		{
		case FunctEngine::Param::VALUE:
			try
			{
				dParamVector[uiValueIdx] = i->count();
			}
			catch (CalcError & error)
			{
				oErrors.appendError(error);
			}
			break;

		case FunctEngine::Param::FLAG:
			// Parametr nie jest zmienn¹
			if (i->eNodeType != NODE_VAR)
				oErrors.appendError(CalcError::EXPECTED_FLAG, "oczekiwana flaga; zobacz @flist " + szName, i->uiNodeBegIdx, i->uiNodeEndIdx);

			// Zmienna nie istnieje
			else if ((uiFlagIdx = gVarEngine.getIdx(dynamic_cast<CalcTreeVar*>(i)->getName())) == VarEngine::nidx)
				oErrors.appendError(CalcError::BAD_FLAG, "niepoprawna flaga; zobacz @flist " + szName, i->uiNodeBegIdx, i->uiNodeEndIdx);

			// Zmienna nie jest na liœcie flag
			else if (gFunctEngine[idx][uiParamIdx].uParam.flag.uiFlagIdxSet.find(uiFlagIdx) == gFunctEngine[idx][uiParamIdx].uParam.flag.uiFlagIdxSet.end())
				oErrors.appendError(CalcError::BAD_FLAG, "niepoprawna flaga; zobacz @flist " + szName, i->uiNodeBegIdx, i->uiNodeEndIdx);

			// Wszystko ok - dodaj wartoœæ flagi do wektora
			else
				dParamVector[uiValueIdx] = gVarEngine[uiFlagIdx].value();

			break;

		case FunctEngine::Param::VA_LIST:
			try
			{
				dParamVector[uiValueIdx] = i->count();
			}
			catch (CalcError & error)
			{
				oErrors.appendError(error);
			}

			// Zmniejsz indeks w wektorze parametrów, aby przy nastêpnym obiegu wróciæ do zmiennej listy parametrów
			uiParamIdx--;
			break;
		}

		// Zwiêksz indeks w wektorze obliczonych wartoœci i w wektorze parametrów
		uiValueIdx++;
		uiParamIdx++;
	}

	// Jeœli wyst¹pi³y b³êdy, wyrzuæ
	if (oErrors.getNum())
		throw oErrors;

	// Uzupe³nij funkcjê o niewprowadzone parametry z wartoœciami 
	gFunctEngine[idx].autoCompParams(dParamVector);
	
	try
	{
		dResult = gFunctEngine[idx].pFunctPointer(dParamVector);
	}
	catch (CalcError & error)
	{
		throw CalcError(error[0].eType, error[0].szMsg, uiNodeBegIdx, uiNodeBegIdx + szName.length() - 1);
	}

	return pkutl::intel_round_if_int(dResult);
}

std::string CalcTreeFunct::printIntern(CALC_PRINT_MODE eMode, size_t & idx)
{
	switch (eMode)
	{
	case PRINT_INLINE:
		uiNodeBegIdx = idx;
		if (!opParamsVector.empty())
		{
			std::string szReturn;

			szReturn += szName + "(";			idx += szReturn.length();

			 // Wypisz parametry
			for (auto const & i : opParamsVector)
			{
				szReturn += i->printIntern(eMode, idx);
				szReturn += "; ";		idx += 2;
			}
			
			 // Usuñ niepotrzebne ", " na koñcu
			szReturn.pop_back();
			szReturn.pop_back();
			idx -= 1;		// Przesuwamy indeks o ")" i zmniejszamy o usuniête ", " 
			uiNodeEndIdx = idx - 1;
			return szReturn + ")";
		}
		else
		{
			idx += szName.length() + 2;
			uiNodeEndIdx = idx - 1;
			return szName + "()";
		}

	case PRINT_BLOCK:
	case PRINT_BLOCK_BRACKETS:
		std::string szReturn{ "Funkcja(" + szName + ")" };

		if (eMode == PRINT_BLOCK_BRACKETS)
			szReturn += "\n{";

		 // Jeœli brak parametrów - wypisz BRAK PARAMETRÓW
		if (opParamsVector.empty())
			szReturn += "\n\tBRAK PARAMETRÓW";
		 // Jeœli s¹ jakieœ, wypisz w postaci PARAMETR: cuœ
		else
		{
			std::string szLine;
			unsigned short usParamCount = 1;
			for (auto const & i : opParamsVector)
			{
				 // Pobierz wydruk dziecka i wypisz
				std::stringstream szExpStream("PARAMETR #" + std::to_string(usParamCount++) + ": " + i->printIntern(eMode, idx));
				while (std::getline(szExpStream, szLine))
					szReturn += "\n\t" + szLine;
				szReturn += ",";
			}

			 // Usuñ niepotrzebny przecinek na koñcu
			szReturn.pop_back();
		}

		if (eMode == PRINT_BLOCK_BRACKETS)
			szReturn += "\n}";

		return szReturn;
	}
	return std::string();
}

void CalcTreeFunct::appendNode(CalcTree * opNode)
{
	return;
}

CalcTree * CalcTreeFunct::detachNode()
{
	return nullptr;
}


// Metody CalcTreeBrackets
//---------------------------------------------------------------------


// Konstruktor z samym wyra¿eniem w nawiasie - domyœlny typ nawiasów
//---------------------------------------------------------------------
CalcTreeBrackets::CalcTreeBrackets(CalcTree * opExp) : CalcTree(NODE_BRACKETS), opExp(opExp), cSigns(BRACKETS_ARRAY[PARENTHESES])
{
	 // Zapobiegnij podwójnemu przypisaniu rodzica
	if (opExp->opParent != nullptr)
		throw CalcDevError(CalcDevError::DOUBLE_PARENT_ASSOC, std::string("próba podwójnego przypisania rodzica"));

	opExp->opParent = this;
}

// Konstruktor z wyra¿eniem w nawiasie i podanym typem nawiasów
//-------------------------------------------------------------------
CalcTreeBrackets::CalcTreeBrackets(CALC_BRACKET_TYPE eType, CalcTree * opExp) : CalcTree(NODE_BRACKETS), opExp(opExp), cSigns(BRACKETS_ARRAY[eType]) 
{
	 // Zapobiegnij podwójnemu przypisaniu rodzica
	if (opExp->opParent != nullptr)
		throw CalcDevError(CalcDevError::DOUBLE_PARENT_ASSOC, std::string("próba podwójnego przypisania rodzica"));

	opExp->opParent = this;
}

CalcTreeBrackets::~CalcTreeBrackets()
{
	delete opExp;
}

void CalcTreeBrackets::checkSymbols()
{
	 // Wywo³aj metodê na dziecku
	opExp->checkSymbols();
}

double CalcTreeBrackets::count()
{	
	return opExp->count();
}

std::string CalcTreeBrackets::printIntern(CALC_PRINT_MODE eMode, size_t & idx)
{
	std::string szReturn = "";
	switch (eMode)
	{
	case PRINT_INLINE:
		uiNodeBegIdx = idx;
		szReturn += std::string(1, cSigns[0]);		idx += 1;
		szReturn += opExp->printIntern(eMode, idx);
		szReturn += std::string(1, cSigns[1]);		idx += 1;

		uiNodeEndIdx = idx - 1;
		return szReturn;

	case PRINT_BLOCK:
	case PRINT_BLOCK_BRACKETS:
		 // Przygotuj pocz¹tek bloku
		std::stringstream szResultStream;
		std::stringstream szExpStream(opExp->printIntern(eMode, idx));

		szResultStream << "Nawiasy " << cSigns[0] << " " << cSigns[1];
		if (eMode == PRINT_BLOCK_BRACKETS) 
			szResultStream << "\n{";

		 // Pobierz wydruk dziecka i zwiêksz zag³êbienie tabulacji
		std::string szLine;
		while (std::getline(szExpStream, szLine))
			szResultStream << "\n\t" << szLine;

		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n}";
		return szResultStream.str();
	}
	return std::string();
}

void CalcTreeBrackets::appendNode(CalcTree * opNode)
{
	delete opExp;
	opExp = opNode;
	opExp->opParent = this;
}

CalcTree * CalcTreeBrackets::detachNode()
{
	if (opExp->eNodeType == NODE_EMPTY)
		return nullptr;

	opExp->opParent = nullptr;
	CalcTree * opSave = opExp;
	opExp = new CalcTreeEmpty();
	return opSave;
}


// Metody CalcTreeAddInv
//---------------------------------------------------------------------


// Konstruktor z wyra¿eniem "pod minusem"
//---------------------------------------------------------------------
CalcTreeAddInv::CalcTreeAddInv(CalcTree * opExp) : CalcTree(NODE_ADD_INV), opExp(opExp)
{
	 // Zapobiegnij podwójnemu przypisaniu rodzica
	if (opExp->opParent != nullptr)
		throw CalcDevError(CalcDevError::DOUBLE_PARENT_ASSOC, std::string("próba podwójnego przypisania rodzica"));

	opExp->opParent = this;
}

CalcTreeAddInv::~CalcTreeAddInv()
{
	delete opExp;
}

void CalcTreeAddInv::checkSymbols()
{
	 // Wywo³aj metodê na dziecku
	opExp->checkSymbols();
}

double CalcTreeAddInv::count()
{
	return -opExp->count();
}

std::string CalcTreeAddInv::printIntern(CALC_PRINT_MODE eMode, size_t & idx)
{
	std::string szReturn = "";
	switch (eMode)
	{
	case PRINT_INLINE:
		 // Jeœli rodzic to operator dwuargumentowy, silnia, lub minus unarny, dodaj nawiasy
		CALC_NODE_TYPE eParentNodeType;
		if (opParent != nullptr)
			eParentNodeType = opParent->getType();
		else
			eParentNodeType = NODE_EMPTY;

		uiNodeBegIdx = idx;
		switch (eParentNodeType)
		{
			case NODE_ADD_INV:
			case NODE_SUM:
			case NODE_DIFF:
			case NODE_PRODUCT:
			case NODE_QUOT:
			case NODE_POW:
			case NODE_FACTOR:
			case NODE_MOD:
				szReturn += std::string("(-");		idx += 2;
				szReturn += opExp->printIntern(eMode, idx);
				szReturn += std::string(")");		idx += 1;
				break;

			default:
				szReturn += std::string("-");		idx += 1;
				szReturn += opExp->printIntern(eMode, idx);
		}

		uiNodeEndIdx = idx - 1;
		return szReturn;

	case PRINT_BLOCK:
	case PRINT_BLOCK_BRACKETS:
		 // Przygotuj pocz¹tek bloku
		std::stringstream szResultStream;
		std::stringstream szExpStream(opExp->printIntern(eMode, idx));

		szResultStream << "Liczba przeciwna";
		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n{";

		 // Pobierz wydruk dziecka i zwiêksz zag³êbienie tabulacji
		std::string szLine;
		while (std::getline(szExpStream, szLine))
			szResultStream << "\n\t" << szLine;

		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n}";
		return szResultStream.str();
	}
	return std::string();
}

void CalcTreeAddInv::appendNode(CalcTree * opNode)
{
	delete opExp;
	opExp = opNode;
	opExp->opParent = this;
}

CalcTree * CalcTreeAddInv::detachNode()
{
	if (opExp->eNodeType == NODE_EMPTY)
		return nullptr;

	opExp->opParent = nullptr;
	CalcTree * opSave = opExp;
	opExp = new CalcTreeEmpty();
	return opSave;
}


// Metody CalcTreeSum
//---------------------------------------------------------------------


// Konstruktor ze wskaŸnikami do czynników
//---------------------------------------------------------------------
CalcTreeSum::CalcTreeSum(CalcTree * opSummand1, CalcTree * opSummand2) : CalcTree(NODE_SUM), opSummand1(opSummand1), opSummand2(opSummand2)
{
	 // Zapobiegnij podwójnemu przypisaniu rodzica
	if (opSummand1->opParent != nullptr || opSummand2->opParent != nullptr)
		throw CalcDevError(CalcDevError::DOUBLE_PARENT_ASSOC, std::string("próba podwójnego przypisania rodzica"));

	opSummand1->opParent = this;
	opSummand2->opParent = this;
}

CalcTreeSum::~CalcTreeSum()
{
	delete opSummand1;
	delete opSummand2;
	//std::cout << "usuwam" << std::endl;
}

void CalcTreeSum::checkSymbols()
{
	 // Próbuj wywo³aæ na dzieciach i przeka¿ b³êdy
	CalcError oErrors;

	try {
		opSummand1->checkSymbols();
	} catch (CalcError & error){
		oErrors.appendError(error);
	} 
	
	try {
		opSummand2->checkSymbols();
	} catch (CalcError & error) {
		oErrors.appendError(error);
	}

	if (oErrors.getNum())
		throw oErrors;
}

double CalcTreeSum::count()
{
	 // Próbuj obliczyæ sk³adniki i w razie czego wychwyæ b³êdy i przeka¿ dalej
	CalcError oErrors;
	double dSummand1, dSummand2;
	double dResult;

	try{
		dSummand1 = opSummand1->count();
	} catch (CalcError & error) {
		oErrors.appendError(error);
	}

	try {
		dSummand2 = opSummand2->count();
	} catch (CalcError & error) {
		oErrors.appendError(error);
	}

	if (oErrors.getNum())
		throw oErrors;

	CalcError::clearFenv();
	dResult = pkutl::intel_sum(dSummand1, dSummand2);
	CalcError::checkFenv(FENV_RANGE, "", uiNodeBegIdx, uiNodeEndIdx);
	return dResult;
}

std::string CalcTreeSum::printIntern(CALC_PRINT_MODE eMode, size_t & idx)
{
	std::string szReturn = "";
	switch (eMode)
	{
	case PRINT_INLINE:
		 // Jeœli rodzic ma wiêkszy priorytet, dodaj nawiasy
		unsigned short usParentOpPrecedence;
		if (opParent != nullptr)
			usParentOpPrecedence = opParent->getOpPrecedence();
		else
			usParentOpPrecedence = 0;

		uiNodeBegIdx = idx;
		if (usParentOpPrecedence > usOpPrecedence)
		{
			szReturn += std::string("(");		idx += 1;
			szReturn += opSummand1->printIntern(eMode, idx);
			szReturn += std::string(" + ");		idx += 3;
			szReturn += opSummand2->printIntern(eMode, idx);
			szReturn += std::string(")");		idx += 1;
		}
		else
		{
			szReturn += opSummand1->printIntern(eMode, idx);
			szReturn += std::string(" + ");		idx += 3;
			szReturn += opSummand2->printIntern(eMode, idx);
		}

		uiNodeEndIdx = idx - 1;
		return szReturn;

	case PRINT_BLOCK:
	case PRINT_BLOCK_BRACKETS:
		 // Przygotuj pocz¹tek bloku
		std::stringstream szResultStream;
		std::stringstream szSummand1Stream(opSummand1->printIntern(eMode, idx));
		std::stringstream szSummand2Stream(opSummand2->printIntern(eMode, idx));

		szResultStream << "Suma";
		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n{";

		 // Pobierz wydruk sk³adników i zwiêksz zag³êbienie tabulacji
		std::string szLine;
		while (std::getline(szSummand1Stream, szLine))
			szResultStream << "\n\t" << szLine;

		szResultStream << ",";
		while (std::getline(szSummand2Stream, szLine))
			szResultStream << "\n\t" << szLine;

		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n}";
		return szResultStream.str();
	}
	return std::string();
}

void CalcTreeSum::appendNode(CalcTree * opNode)
{
	delete opSummand2;
	opSummand2 = opNode;
	opSummand2->opParent = this;
}

CalcTree * CalcTreeSum::detachNode()
{
	if (opSummand2->eNodeType == NODE_EMPTY)
		return nullptr;

	opSummand2->opParent = nullptr;
	CalcTree * opSave = opSummand2;
	opSummand2 = new CalcTreeEmpty();
	return opSave;
}


// Metody CalcTreeDiff
//---------------------------------------------------------------------


// Konstruktor pobieraj¹cy odjemn¹ i odjemnik
//---------------------------------------------------------------------
CalcTreeDiff::CalcTreeDiff(CalcTree * opMinuend, CalcTree * opSubtrahend) : CalcTree(NODE_DIFF), opMinuend(opMinuend), opSubtrahend(opSubtrahend)
{
	 // Zapobiegnij podwójnemu przypisaniu rodzica
	if (opMinuend->opParent != nullptr || opSubtrahend->opParent != nullptr)
		throw CalcDevError(CalcDevError::DOUBLE_PARENT_ASSOC, std::string("próba podwójnego przypisania rodzica"));

	opMinuend->opParent = this;
	opSubtrahend->opParent = this;
}

CalcTreeDiff::~CalcTreeDiff()
{
	delete opMinuend;
	delete opSubtrahend;
}

void CalcTreeDiff::checkSymbols()
{
	 // Próbuj wywo³aæ na dzieciach i przeka¿ b³êdy
	CalcError oErrors;

	try {
		opMinuend->checkSymbols();
	} catch (CalcError & error) {
		oErrors.appendError(error);
	}

	try {
		opSubtrahend->checkSymbols();
	} catch (CalcError & error) {
		oErrors.appendError(error);
	}

	if (oErrors.getNum())
		throw oErrors;
}

double CalcTreeDiff::count()
{
	 // Próbuj obliczyæ odjemn¹ i odjemnik i w razie czego wychwyæ b³¹d i przeka¿ dalej
	CalcError oErrors;
	double dMinuend, dSubtrahend;
	double dResult;

	try {
		dMinuend = opMinuend->count();
	} catch (CalcError & error) {
		oErrors.appendError(error);
	}

	try {
		dSubtrahend = opSubtrahend->count();
	} catch (CalcError & error) {
		oErrors.appendError(error);
	}

	if (oErrors.getNum())
		throw oErrors;

	CalcError::clearFenv();
	dResult = pkutl::intel_diff(dMinuend, dSubtrahend);
	CalcError::checkFenv(FENV_RANGE, "", uiNodeBegIdx, uiNodeEndIdx);
	return dResult;
}

std::string CalcTreeDiff::printIntern(CALC_PRINT_MODE eMode, size_t & idx)
{
	std::string szReturn = "";
	switch (eMode)
	{
	case PRINT_INLINE:
		// Jeœli rodzic ma wiêkszy priorytet, dodaj nawiasy
		unsigned short usParentOpPrecedence;
		if (opParent != nullptr)
			usParentOpPrecedence = opParent->getOpPrecedence();
		else
			usParentOpPrecedence = 0;

		uiNodeBegIdx = idx;
		if (usParentOpPrecedence > usOpPrecedence)
		{
			szReturn += std::string("(");		idx += 1;
			szReturn += opMinuend->printIntern(eMode, idx);
			szReturn += std::string(" - ");		idx += 3;
			szReturn += opSubtrahend->printIntern(eMode, idx);
			szReturn += std::string(")");		idx += 1;
		}
		else
		{
			szReturn += opMinuend->printIntern(eMode, idx);
			szReturn += std::string(" - ");		idx += 3;
			szReturn += opSubtrahend->printIntern(eMode, idx);
		}

		uiNodeEndIdx = idx - 1;
		return szReturn;

	case PRINT_BLOCK:
	case PRINT_BLOCK_BRACKETS:
		// Przygotuj pocz¹tek bloku
		std::stringstream szResultStream;
		std::stringstream szMinuendStream(opMinuend->printIntern(eMode, idx));
		std::stringstream szSubtrahendStream(opSubtrahend->printIntern(eMode, idx));

		szResultStream << "Ró¿nica";
		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n{";

		// Pobierz wydruk odjemnej i odjemnika i zwiêksz zag³êbienie tabulacji
		std::string szLine;
		while (std::getline(szMinuendStream, szLine))
			szResultStream << "\n\t" << szLine;

		szResultStream << ",";
		while (std::getline(szSubtrahendStream, szLine))
			szResultStream << "\n\t" << szLine;

		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n}";
		return szResultStream.str();
	}
	return std::string();
}

void CalcTreeDiff::appendNode(CalcTree * opNode)
{
	delete opSubtrahend;
	opSubtrahend = opNode;
	opSubtrahend->opParent = this;
}

CalcTree * CalcTreeDiff::detachNode()
{
	if (opSubtrahend->eNodeType == NODE_EMPTY)
		return nullptr;

	opSubtrahend->opParent = nullptr;
	CalcTree * opSave = opSubtrahend;
	opSubtrahend = new CalcTreeEmpty();
	return opSave;
}


// Metody CalcTreeProduct
//---------------------------------------------------------------------


// Konstruktor pobierajacy czynniki
//---------------------------------------------------------------------
CalcTreeProduct::CalcTreeProduct(CalcTree * opFactor1, CalcTree * opFactor2) : CalcTree(NODE_PRODUCT), opFactor1(opFactor1), opFactor2(opFactor2)
{
	 // Zapobiegnij podwójnemu przypisaniu rodzica
	if (opFactor1->opParent != nullptr || opFactor2->opParent != nullptr)
		throw CalcDevError(CalcDevError::DOUBLE_PARENT_ASSOC, std::string("próba podwójnego przypisania rodzica"));

	opFactor1->opParent = this;
	opFactor2->opParent = this;
}

CalcTreeProduct::~CalcTreeProduct()
{
	delete opFactor1;
	delete opFactor2;
	//std::cout << "usuwam" << std::endl;
}

void CalcTreeProduct::checkSymbols()
{
	 // Próbuj wywo³aæ na dzieciach i przeka¿ b³êdy
	CalcError oErrors;

	try {
		opFactor1->checkSymbols();
	} catch (CalcError & error) {
		oErrors.appendError(error);
	}

	try {
		opFactor2->checkSymbols();
	} catch (CalcError & error) {
		oErrors.appendError(error);
	}

	if (oErrors.getNum())
		throw oErrors;
}

double CalcTreeProduct::count()
{
	 // Próbuj obliczyæ czynniki i w razie czego wychwyæ b³¹d i przeka¿ dalej
	CalcError oErrors;
	double dFactor1, dFactor2;
	double dResult;

	try {
		dFactor1 = opFactor1->count();
	} catch (CalcError & error) {
		oErrors.appendError(error);
	}

	try {
		dFactor2 = opFactor2->count();
	} catch (CalcError & error) {
		oErrors.appendError(error);
	}

	if (oErrors.getNum())
		throw oErrors;

	CalcError::clearFenv();
	dResult = dFactor1 * dFactor2;
	CalcError::checkFenv(FENV_RANGE, "", uiNodeBegIdx, uiNodeEndIdx);
	return pkutl::intel_round_if_int(dResult);
}

std::string CalcTreeProduct::printIntern(CALC_PRINT_MODE eMode, size_t & idx)
{
	std::string szReturn = "";
	switch (eMode)
	{
	case PRINT_INLINE:
		 // Jeœli rodzic ma wiêkszy priorytet, dodaj nawiasy
		unsigned short usParentOpPrecedence;
		if (opParent != nullptr)
			usParentOpPrecedence = opParent->getOpPrecedence();
		else
			usParentOpPrecedence = 0;

		uiNodeBegIdx = idx;
		if (usParentOpPrecedence > usOpPrecedence)
		{
			szReturn += std::string("(");		idx += 1;
			szReturn += opFactor1->printIntern(eMode, idx);
			szReturn += std::string(" * ");		idx += 3;
			szReturn += opFactor2->printIntern(eMode, idx);
			szReturn += std::string(")");		idx += 1;
		}
		else
		{
			szReturn += opFactor1->printIntern(eMode, idx);
			szReturn += std::string(" * ");		idx += 3;
			szReturn += opFactor2->printIntern(eMode, idx);
		}

		uiNodeEndIdx = idx - 1;
		return szReturn;

	case PRINT_BLOCK:
	case PRINT_BLOCK_BRACKETS:
		 // Przygotuj pocz¹tek bloku
		std::stringstream szResultStream;
		std::stringstream szFactor1Stream(opFactor1->printIntern(eMode, idx));
		std::stringstream szFactor2Stream(opFactor2->printIntern(eMode, idx));

		szResultStream << "Iloczyn";
		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n{";

		 // Pobierz wydruk czynników i zwiêksz zag³êbienie tabulacji
		std::string szLine;
		while (std::getline(szFactor1Stream, szLine))
			szResultStream << "\n\t" << szLine;

		szResultStream << ",";
		while (std::getline(szFactor2Stream, szLine))
			szResultStream << "\n\t" << szLine;

		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n}";
		return szResultStream.str();
	}
	return std::string();
}

void CalcTreeProduct::appendNode(CalcTree * opNode)
{
	delete opFactor2;
	opFactor2 = opNode;
	opFactor2->opParent = this;
}

CalcTree * CalcTreeProduct::detachNode()
{
	if (opFactor2->eNodeType == NODE_EMPTY)
		return nullptr;

	opFactor2->opParent = nullptr;
	CalcTree * opSave = opFactor2;
	opFactor2 = new CalcTreeEmpty();
	return opSave;
}


// Metody CalcTreeQuot
//---------------------------------------------------------------------


// Konstruktor pobieraj¹cy dzieln¹ i dzielnik
//---------------------------------------------------------------------
CalcTreeQuot::CalcTreeQuot(CalcTree * opDividend, CalcTree * opDivisor) : CalcTree(NODE_QUOT), opDividend(opDividend), opDivisor(opDivisor)
{
	 // Zapobiegnij podwójnemu przypisaniu rodzica
	if (opDividend->opParent != nullptr || opDivisor->opParent != nullptr)
		throw CalcDevError(CalcDevError::DOUBLE_PARENT_ASSOC, std::string("próba podwójnego przypisania rodzica"));

	opDividend->opParent = this;
	opDivisor->opParent = this;
}

CalcTreeQuot::~CalcTreeQuot()
{
	delete opDividend;
	delete opDivisor;
}

void CalcTreeQuot::checkSymbols()
{
	 // Próbuj wywo³aæ na dzieciach i przeka¿ b³êdy
	CalcError oErrors;

	try {
		opDividend->checkSymbols();
	} catch (CalcError & error) {
		oErrors.appendError(error);
	}

	try {
		opDivisor->checkSymbols();
	} catch (CalcError & error) {
		oErrors.appendError(error);
	}

	if (oErrors.getNum())
		throw oErrors;
}

double CalcTreeQuot::count()
{
	 // Próbuj obliczyæ dzieln¹ i dzielnik i w razie czego wychwyæ b³¹d i przeka¿ dalej
	CalcError oErrors;
	double dDividend, dDivisor;
	double dResult;

	try {
		dDividend = opDividend->count();
	} catch (CalcError & error) {
		oErrors.appendError(error);
	}

	try {
		dDivisor = opDivisor->count();
	} catch (CalcError & error) {
		oErrors.appendError(error);
	}

	if (oErrors.getNum())
		throw oErrors;

	if (dDivisor == 0.0 && dDividend == 0.0)
		throw CalcError(CalcError::INDETERM, "symbol nieoznaczony 0/0", uiNodeBegIdx, uiNodeEndIdx);

	 // Zabezpiecz przed dzieleniem przez 0
	if (dDivisor == 0.0)
	{
		opDivisor->eErrorFlag = CalcError::DIV_ZERO;
		throw CalcError(CalcError::DIV_ZERO, "dzielenie przez 0... jesteœ debilem xD", opDivisor->uiNodeBegIdx, opDivisor->uiNodeEndIdx);
	}

	CalcError::clearFenv();
	dResult = dDividend / dDivisor;
	CalcError::checkFenv(FENV_RANGE, "", uiNodeBegIdx, uiNodeEndIdx);
	return pkutl::intel_round_if_int(dResult);
}

std::string CalcTreeQuot::printIntern(CALC_PRINT_MODE eMode, size_t & idx)
{
	std::string szReturn = "";
	switch (eMode)
	{
	case PRINT_INLINE:
		// Jeœli rodzic ma wiêkszy priorytet, dodaj nawiasy
		unsigned short usParentOpPrecedence;
		if (opParent != nullptr)
			usParentOpPrecedence = opParent->getOpPrecedence();
		else
			usParentOpPrecedence = 0;

		uiNodeBegIdx = idx;
		if (usParentOpPrecedence > usOpPrecedence)
		{
			szReturn += std::string("(");		idx += 1;
			szReturn += opDividend->printIntern(eMode, idx);
			szReturn += std::string(" / ");		idx += 3;
			szReturn += opDivisor->printIntern(eMode, idx);
			szReturn += std::string(")");		idx += 1;
		}
		else
		{
			szReturn += opDividend->printIntern(eMode, idx);
			szReturn += std::string(" / ");		idx += 3;
			szReturn += opDivisor->printIntern(eMode, idx);
		}

		uiNodeEndIdx = idx - 1;
		return szReturn;

	case PRINT_BLOCK:
	case PRINT_BLOCK_BRACKETS:
		// Przygotuj pocz¹tek bloku
		std::stringstream szResultStream;
		std::stringstream szDividendStream(opDividend->printIntern(eMode, idx));
		std::stringstream szDivisorStream(opDivisor->printIntern(eMode, idx));

		szResultStream << "Iloraz";
		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n{";

		// Pobierz wydruk dzielnej i dzielnika i zwiêksz zag³êbienie tabulacji
		std::string szLine;
		while (std::getline(szDividendStream, szLine))
			szResultStream << "\n\t" << szLine;

		szResultStream << ",";
		while (std::getline(szDivisorStream, szLine))
			szResultStream << "\n\t" << szLine;

		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n}";
		return szResultStream.str();
	}
	return std::string();
}

void CalcTreeQuot::appendNode(CalcTree * opNode)
{
	delete opDivisor;
	opDivisor = opNode;
	opDivisor->opParent = this;
}

CalcTree * CalcTreeQuot::detachNode()
{
	if (opDivisor->eNodeType == NODE_EMPTY)
		return nullptr;

	opDivisor->opParent = nullptr;
	CalcTree * opSave = opDivisor;
	opDivisor = new CalcTreeEmpty();
	return opSave;
}


// Metody CalcTreePow
//---------------------------------------------------------------------


// Konstruktor pobieraj¹cy podstawê i wyk³adnik
//---------------------------------------------------------------------
CalcTreePow::CalcTreePow(CalcTree * opBase, CalcTree * opExponent) : CalcTree(NODE_POW), opBase(opBase), opExponent(opExponent)
{
	 // Zapobiegnij podwójnego przypisania rodzica
	if (opBase->opParent != nullptr || opExponent->opParent != nullptr)
		throw CalcDevError(CalcDevError::DOUBLE_PARENT_ASSOC, std::string("próba podwójnego przypisania rodzica"));

	opBase->opParent = this;
	opExponent->opParent = this;
}

CalcTreePow::~CalcTreePow()
{
	delete opBase;
	delete opExponent;
}

void CalcTreePow::checkSymbols()
{
	 // Próbuj wywo³aæ na dzieciach i przeka¿ b³êdy
	CalcError oErrors;

	try {
		opBase->checkSymbols();
	} catch (CalcError & error) {
		oErrors.appendError(error);
	}

	try {
		opExponent->checkSymbols();
	} catch (CalcError & error) {
		oErrors.appendError(error);
	}

	if (oErrors.getNum())
		throw oErrors;
}

double CalcTreePow::count()
{
	 // Próbuj obliczyæ podstawê i wyk³adnik i w razie czego wychwyæ b³¹d i przeka¿ dalej
	CalcError oErrors;
	double dBase, dExponent;
	double dResult;

	try {
		dBase = opBase->count();
	} catch (CalcError & error) {
		oErrors.appendError(error);
	}

	try {
		dExponent = opExponent->count();
	} catch (CalcError & error) {
		oErrors.appendError(error);
	}

	if (oErrors.getNum())
		throw oErrors;

	if (dBase == 0.0 && dExponent == 0.0)
		throw CalcError(CalcError::INDETERM, "symbol nieoznaczony 0^0", uiNodeBegIdx, uiNodeEndIdx);

	CalcError::clearFenv();
	dResult = pow(dBase, dExponent);

	 // Wy³apano b³¹d
	CalcError::checkFenv(FENV_DOMAIN, "nieokreœlone potêgowanie", uiNodeBegIdx, uiNodeEndIdx);
	CalcError::checkFenv(FENV_DIVBYZERO, "dzielenie przez 0 - ujemny wyk³adnik", opExponent->uiNodeBegIdx, opExponent->uiNodeEndIdx);
	CalcError::checkFenv(FENV_RANGE, "", uiNodeBegIdx, uiNodeEndIdx);

	return dResult;
}

std::string CalcTreePow::printIntern(CALC_PRINT_MODE eMode, size_t & idx)
{
	std::string szReturn = "";
	switch (eMode)
	{
	case PRINT_INLINE:
		// Jeœli rodzic ma wiêkszy priorytet, dodaj nawiasy
		unsigned short usParentOpPrecedence;
		if (opParent != nullptr)
			usParentOpPrecedence = opParent->getOpPrecedence();
		else
			usParentOpPrecedence = 0;

		uiNodeBegIdx = idx;
		if (usParentOpPrecedence > usOpPrecedence)
		{
			szReturn += std::string("(");		idx += 1;
			szReturn += opBase->printIntern(eMode, idx);
			szReturn += std::string("^");		idx += 1;
			szReturn += opExponent->printIntern(eMode, idx);
			szReturn += std::string(")");		idx += 1;
		}
		else
		{
			szReturn += opBase->printIntern(eMode, idx);
			szReturn += std::string("^");		idx += 1;
			szReturn += opExponent->printIntern(eMode, idx);
		}

		uiNodeEndIdx = idx - 1;
		return szReturn;

	case PRINT_BLOCK:
	case PRINT_BLOCK_BRACKETS:
		// Przygotuj pocz¹tek bloku
		std::stringstream szResultStream;
		std::stringstream szBaseStream(opBase->printIntern(eMode, idx));
		std::stringstream szExponentStream(opExponent->printIntern(eMode, idx));

		szResultStream << "Potêga";
		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n{";

		// Pobierz wydruk podstawy i wyk³adnika i zwiêksz zag³êbienie tabulacji
		std::string szLine;
		while (std::getline(szBaseStream, szLine))
			szResultStream << "\n\t" << szLine;

		szResultStream << ",";
		while (std::getline(szExponentStream, szLine))
			szResultStream << "\n\t" << szLine;

		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n}";
		return szResultStream.str();
	}
	return std::string();
}

void CalcTreePow::appendNode(CalcTree * opNode)
{
	delete opExponent;
	opExponent = opNode;
	opExponent->opParent = this;
}

CalcTree * CalcTreePow::detachNode()
{
	if (opExponent->eNodeType == NODE_EMPTY)
		return nullptr;

	opExponent->opParent = nullptr;
	CalcTree * opSave = opExponent;
	opExponent = new CalcTreeEmpty();
	return opSave;
}


// Metody CalcTreeFactor
//---------------------------------------------------------------------


// Konstruktor pobieraj¹cy wyra¿enie pod silni¹
//---------------------------------------------------------------------
CalcTreeFactor::CalcTreeFactor(CalcTree * opExp) : CalcTree(NODE_FACTOR), opExp(opExp) 
{
	 // Zapobiegnij podwójnemu przypisaniu rodzica
	if (opExp->opParent != nullptr)
		throw CalcDevError(CalcDevError::DOUBLE_PARENT_ASSOC, std::string("próba podwójnego przypisania rodzica"));

	opExp->opParent = this;
}

CalcTreeFactor::~CalcTreeFactor()
{
	delete opExp;
}

void CalcTreeFactor::checkSymbols()
{
	 // Wywo³aj metodê na dziecku
	opExp->checkSymbols();
}

double CalcTreeFactor::count()
{
	double dResult;

	CalcError::clearFenv();
	dResult = tgamma(opExp->count() + 1.0);
	CalcError::checkFenv(FENV_RANGE, "", uiNodeBegIdx, uiNodeEndIdx);
	CalcError::checkFenv(FENV_DOMANDZERO, "funkcja gamma - parametr poza dziedzin¹", opExp->uiNodeBegIdx, opExp->uiNodeEndIdx);

	return dResult;
}

std::string CalcTreeFactor::printIntern(CALC_PRINT_MODE eMode, size_t & idx)
{
	std::string szReturn = "";
	switch (eMode)
	{
	case PRINT_INLINE:
		 // Jeœli rodzic to potêga, dodaj nawiasy
		CALC_NODE_TYPE eParentNodeType;
		if (opParent != nullptr)
			eParentNodeType = opParent->getType();
		else
			eParentNodeType = NODE_EMPTY;

		uiNodeBegIdx = idx;
		switch (eParentNodeType)
		{
		case NODE_POW:
			szReturn += std::string("(");		idx += 1;
			szReturn += opExp->printIntern(eMode, idx);
			szReturn += std::string("!)");		idx += 2;
			break;

		default:
			szReturn += opExp->printIntern(eMode, idx);
			szReturn += std::string("!");		idx += 1;
		}

		uiNodeEndIdx = idx - 1;
		return szReturn;

	case PRINT_BLOCK:
	case PRINT_BLOCK_BRACKETS:
		 // Przygotuj pocz¹tek bloku
		std::stringstream szResultStream;
		std::stringstream szExpStream(opExp->printIntern(eMode, idx));

		szResultStream << "Silnia";
		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n{";

		 // Pobierz wydruk dziecka i zwiêksz zag³êbienie tabulacji
		std::string szLine;
		while (std::getline(szExpStream, szLine))
			szResultStream << "\n\t" << szLine;

		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n}";
		return szResultStream.str();
	}
	return std::string();
}

void CalcTreeFactor::appendNode(CalcTree * opNode)
{
	delete opExp;
	opExp = opNode;
	opExp->opParent = this;
}

CalcTree * CalcTreeFactor::detachNode()
{
	if (opExp->eNodeType == NODE_EMPTY)
		return nullptr;

	opExp->opParent = nullptr;
	CalcTree * opSave = opExp;
	opExp = new CalcTreeEmpty();
	return opSave;
}


// Metody CalcTreeMod
//---------------------------------------------------------------------


// Konstruktor przyjmuj¹cy dzieln¹ i dzielnik
//--------------------------------------------------------------------
CalcTreeMod::CalcTreeMod(CalcTree * opDividend, CalcTree * opDivisor) : CalcTree(NODE_MOD), opDividend(opDividend), opDivisor(opDivisor) 
{
	 // Zapobiegnij podwójnemu przypisaniu rodzica
	if (opDividend->opParent != nullptr || opDivisor->opParent != nullptr)
		throw CalcDevError(CalcDevError::DOUBLE_PARENT_ASSOC, std::string("próba podwójnego przypisania rodzica"));

	opDividend->opParent = this;
	opDivisor->opParent = this;
}

CalcTreeMod::~CalcTreeMod()
{
	delete opDividend;
	delete opDivisor;
}

void CalcTreeMod::checkSymbols()
{
	 // Próbuj wywo³aæ na dzieciach i przeka¿ b³êdy
	CalcError oErrors;

	try {
		opDividend->checkSymbols();
	}
	catch (CalcError & error) {
		oErrors.appendError(error);
	}

	try {
		opDivisor->checkSymbols();
	}
	catch (CalcError & error) {
		oErrors.appendError(error);
	}

	if (oErrors.getNum())
		throw oErrors;
}

double CalcTreeMod::count()
{
	 // Próbuj obliczyæ dzieln¹ i dzielnik i w razie czego wychwyæ b³¹d i przeka¿ dalej
	CalcError oErrors;
	double dDividend = 0.0, dDivisor = 0.0;

	try {
		dDividend = opDividend->count();
	}
	catch (CalcError & error) {
		oErrors.appendError(error);
	}

	try {
		dDivisor = opDivisor->count();
	}
	catch (CalcError & error) {
		oErrors.appendError(error);
	}

	// SprawdŸ, czy podano liczby ca³kowite
	if (!pkutl::intel_is_int(dDividend))
		oErrors.appendError(CalcError(CalcError::EXPECTED_INT, "modulo przyjmuje tylko ca³kowite wartoœci", opDividend->uiNodeBegIdx, opDividend->uiNodeEndIdx));
	if (!pkutl::intel_is_int(dDivisor))
		oErrors.appendError(CalcError(CalcError::EXPECTED_INT, "modulo przyjmuje tylko ca³kowite wartoœci", opDivisor->uiNodeBegIdx, opDivisor->uiNodeEndIdx));

	if (oErrors.getNum())
	{
		oErrors.sortBegIdx();
		throw oErrors;
	}

	 // Zabezpiecz przed dzieleniem przez 0
	if (abs(dDivisor) < std::numeric_limits<double>::epsilon())
	{
		opDivisor->eErrorFlag = CalcError::DIV_ZERO;
		CalcError error(CalcError::DIV_ZERO, "dzielenie przez 0... jesteœ debilem xD");
		throw error;
	}

	return int(dDividend) % int(dDivisor);
}

std::string CalcTreeMod::printIntern(CALC_PRINT_MODE eMode, size_t & idx)
{
	std::string szReturn = "";
	switch (eMode)
	{
	case PRINT_INLINE:
		 // Jeœli rodzic ma wiêkszy priorytet, dodaj nawiasy
		unsigned short usParentOpPrecedence;
		if (opParent != nullptr)
			usParentOpPrecedence = opParent->getOpPrecedence();
		else
			usParentOpPrecedence = 0;

		uiNodeBegIdx = idx;
		if (usParentOpPrecedence > usOpPrecedence)
		{
			szReturn += std::string("(");		idx += 1;
			szReturn += opDividend->printIntern(eMode, idx);
			szReturn += std::string(" % ");		idx += 3;
			szReturn += opDivisor->printIntern(eMode, idx);
			szReturn += std::string(")");		idx += 1;
		}
		else
		{
			szReturn += opDividend->printIntern(eMode, idx);
			szReturn += std::string(" % ");		idx += 3;
			szReturn += opDivisor->printIntern(eMode, idx);
		}

		uiNodeEndIdx = idx - 1;
		return szReturn;

	case PRINT_BLOCK:
	case PRINT_BLOCK_BRACKETS:
		 // Przygotuj pocz¹tek bloku
		std::stringstream szResultStream;
		std::stringstream szDividendStream(opDividend->printIntern(eMode, idx));
		std::stringstream szDivisorStream(opDivisor->printIntern(eMode, idx));

		szResultStream << "Modulo";
		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n{";

		 // Pobierz wydruk dzielnej i dzielnika i zwiêksz zag³êbienie tabulacji
		std::string szLine;
		while (std::getline(szDividendStream, szLine))
			szResultStream << "\n\t" << szLine;

		szResultStream << ",";
		while (std::getline(szDivisorStream, szLine))
			szResultStream << "\n\t" << szLine;

		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n}";
		return szResultStream.str();
	}
	return std::string();
}

void CalcTreeMod::appendNode(CalcTree * opNode)
{
	delete opDivisor;
	opDivisor = opNode;
	opDivisor->opParent = this;
}

CalcTree * CalcTreeMod::detachNode()
{
	if (opDivisor->eNodeType == NODE_EMPTY)
		return nullptr;

	opDivisor->opParent = nullptr;
	CalcTree * opSave = opDivisor;
	opDivisor = new CalcTreeEmpty();
	return opSave;
}