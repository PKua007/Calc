// class_calctree.cpp
//---------------------------------------------------------------------
// Plik �rod�owy z klas� reprezentuj�c� drzewo wyra�enia
//---------------------------------------------------------------------
// Obiekty przechowuj� wyra�enie w postaci drzewo. Metody pozwalaj�
// na obliczanie wyra�enia, wydrukowanie go w postaci ci�gu znak�w,
// zapisywane s� tak�e informacje o b��dach logicznych
// (nieznane funkcje, zmienne, warto�ci spoza dziedziny, itd.)
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


// Konstruktor klasy bazowej okre�laj�cy parametry w�z�a
//----------------------------------------------------------------------
CalcTree::CalcTree(CALC_NODE_TYPE eNodeType) : eNodeType(eNodeType), usOpPrecedence(CALC_NODE_DATA[eNodeType].usOpPrec), eOpAssoc(CALC_NODE_DATA[eNodeType].eOpAssoc) {}

// Metoda zwraca sta�� referencj� do typu w�z�a
//----------------------------------------------------------------------
const CALC_NODE_TYPE & CalcTree::getType() const
{
	return eNodeType;
}

// Metoda zwraca sta�� referencj� do rz�du operatora
//----------------------------------------------------------------------
const unsigned short & CalcTree::getOpPrecedence() const
{
	return usOpPrecedence;
}

// Metoda zwraca sta�� referencj� do typu operatora
//-----------------------------------------------------------------------
const CALC_OP_ASSOC & CalcTree::getOpAssocType() const
{
	return eOpAssoc;
}

// Metoda zwraca sta�y wska�nik do rodzica
//----------------------------------------------------------------------- 
CalcTree * CalcTree::getParent() const
{
	return opParent;
}

// Metoda wy�wietla reprezentacj� wyra�enia w postaci ci�gu znak�w
// i ustala w niej indeksy ko�c�w i pocz�tk�w w�z��w (do b�ed�w)
//-----------------------------------------------------------------------
std::string CalcTree::print(CALC_PRINT_MODE eMode)
{
	if (opParent != nullptr)
		throw CalcDevError(CalcDevError::PRINT_NOT_ROOT, "metod� print() mo�na wywo�a� tylko na korzeniu");

	size_t idx = 0U;
	return printIntern(eMode, idx);
}


//**********************************************************************************************************************
// IMPLEMENTACJE METOD DLA WSYSTKICH W�Z��W
//
//    checkSymbol ()
// Dla w�z�a typu NODE_VAR lub NODE_FUNCT sprawdza, czy zmienna/funkcja o podanej nazwie istnieje i jak nie, dodaje
// flag� b��du. Wywo�uj� metod� checkSymbols na wszystkich dzieciach
//
//    count ()
// Oblicza warto�� w�z�a - warto�ci dzieci oblicza poprzez wywo�anie count () na nich
//
//    printIntern(CALC_PRINT_MODE eMode, size_t & idx)
// Generuje reprezentacj� w�z�a w postaci ci�gu znak�w - reprezentacj� dzieci pobiera poprzez wywo�anie printIntern na nich.
// Dost�pne tryby - liniowy (PRINT_INLINE), blokowy (PRINT_BLOCK), blokowy z klamrami (PRINT_BLOCK_BRACKETS)
//
//    appendNode(CalcTree * opNode)
// Je�li w�ze� ma jedno dziecko, podmienia je danym (i w razie potrzeby usuwa stare), a je�li dwa, to podmienia drugie.
// W�z�y NODE_VALUE, NODE_VAR, NODE_FUNCT i NODE_BRACKETS nie obs�uguj� metody i nie powinna by� wywo�ana.
//
//    detachNode()
// Je�li w�ze� ma jedno dziecko, odpina je, a je�li dwa, to odpina drugie. Zwraca wska�nik odpi�tego.
// W�z�y NODE_VALUE, NODE_VAR, NODE_FUNCT i NODE_BRACKETS nie obs�uguj� metody i nie powinna by� wywo�ana.
//*********************************************************************************************************************


// Metody CalcTreeEmpty
//---------------------------------------------------------------------

CalcTreeEmpty::CalcTreeEmpty() : CalcTree(NODE_EMPTY) {}

void CalcTreeEmpty::checkSymbols()
{
	throw CalcDevError(CalcDevError::EMPTY_CHILD_CALL, "pr�ba dost�pu do pustego w�z�a (metoda checkSymbols)");
}

double CalcTreeEmpty::count()
{
	throw CalcDevError(CalcDevError::EMPTY_CHILD_CALL, "pr�ba dost�pu do pustego w�z�a (metoda count)");
	return 0.0;
}

std::string CalcTreeEmpty::printIntern(CALC_PRINT_MODE eMode, size_t & idx)
{
	return std::string(eMode == PRINT_INLINE ? "{brak}" : "PUSTY W�ZE�");
}

void CalcTreeEmpty::appendNode(CalcTree * opNode)
{
	throw CalcDevError(CalcDevError::EMPTY_CHILD_CALL, "pr�ba dost�pu do pustego w�z�a (metoda appendNode)");
}

CalcTree * CalcTreeEmpty::detachNode()
{
	throw CalcDevError(CalcDevError::EMPTY_CHILD_CALL, "pr�ba dost�pu do pustego w�z�a (metoda detachNode)");
	return nullptr;
}


// Metody CalcTreeValue
//---------------------------------------------------------------------


// Konstruktor przyjmuj�cy warto��
//---------------------------------------------------------------------
CalcTreeValue::CalcTreeValue(double dValue) : CalcTree(NODE_VALUE), dValue(dValue) {}

void CalcTreeValue::checkSymbols()
{
	return;
}

double CalcTreeValue::count()
{ 
	//throw CalcError(CalcError::UNKNOWN, "warto��", uiNodeBegIdx, uiNodeEndIdx);
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
		szValueStream << "Warto��(" << dValue << ")";
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


// Konstruktor przyjmuj�cy nazw� zmiennej
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

	 // Sprawd�, czy zmienna istnieje i czy nie ma pr�by u�ycia flagi
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


// Konstruktor przyjmuj�cy nazw� funkcji, tablic� wska�nik�w do parametr�w i liczb� parametr�w
//--------------------------------------------------------------------------------------------
CalcTreeFunct::CalcTreeFunct(std::string _szName, std::vector<CalcTree*> _opParamsVector) : CalcTree(NODE_FUNCT), szName(_szName), opParamsVector(_opParamsVector)
{
	// Zamie� nazw� funkcji na g��wn�, je�li podano jedn� z alternatywnych
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

	// Sprawd�, czy funkcja istnieje
	if ((idx = gFunctEngine.getIdx(szName)) == FunctEngine::nidx)
		throw CalcError(CalcError::UNKNOWN_FUNCT, "nieznana funkcja \"" + szName + "\"", uiNodeBegIdx, uiNodeBegIdx + szName.length() - 1);

	// Sprawd�, czy zgadza si� liczba parametr�w (z uwzgl�dnieniem domy�lnych)
	if (!gFunctEngine[idx].isNumParamsValid(opParamsVector.size()))
		throw CalcError(CalcError::BAD_PARAM_NUM, "z�a liczba parametr�w funkcji; zobacz @flist " + szName, uiNodeBegIdx + szName.length(), uiNodeEndIdx);

	std::vector<double>	dParamVector(opParamsVector.size());
	CalcError			oErrors;
	size_t				uiValueIdx = 0U;
	size_t				uiParamIdx = 0U;
	size_t				uiFlagIdx;
	double				dResult;

	// Pr�buj pobra� wszystkie parametry - w razie czego dodawaj b��dy
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
			// Parametr nie jest zmienn�
			if (i->eNodeType != NODE_VAR)
				oErrors.appendError(CalcError::EXPECTED_FLAG, "oczekiwana flaga; zobacz @flist " + szName, i->uiNodeBegIdx, i->uiNodeEndIdx);

			// Zmienna nie istnieje
			else if ((uiFlagIdx = gVarEngine.getIdx(dynamic_cast<CalcTreeVar*>(i)->getName())) == VarEngine::nidx)
				oErrors.appendError(CalcError::BAD_FLAG, "niepoprawna flaga; zobacz @flist " + szName, i->uiNodeBegIdx, i->uiNodeEndIdx);

			// Zmienna nie jest na li�cie flag
			else if (gFunctEngine[idx][uiParamIdx].uParam.flag.uiFlagIdxSet.find(uiFlagIdx) == gFunctEngine[idx][uiParamIdx].uParam.flag.uiFlagIdxSet.end())
				oErrors.appendError(CalcError::BAD_FLAG, "niepoprawna flaga; zobacz @flist " + szName, i->uiNodeBegIdx, i->uiNodeEndIdx);

			// Wszystko ok - dodaj warto�� flagi do wektora
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

			// Zmniejsz indeks w wektorze parametr�w, aby przy nast�pnym obiegu wr�ci� do zmiennej listy parametr�w
			uiParamIdx--;
			break;
		}

		// Zwi�ksz indeks w wektorze obliczonych warto�ci i w wektorze parametr�w
		uiValueIdx++;
		uiParamIdx++;
	}

	// Je�li wyst�pi�y b��dy, wyrzu�
	if (oErrors.getNum())
		throw oErrors;

	// Uzupe�nij funkcj� o niewprowadzone parametry z warto�ciami 
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
			
			 // Usu� niepotrzebne ", " na ko�cu
			szReturn.pop_back();
			szReturn.pop_back();
			idx -= 1;		// Przesuwamy indeks o ")" i zmniejszamy o usuni�te ", " 
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

		 // Je�li brak parametr�w - wypisz BRAK PARAMETR�W
		if (opParamsVector.empty())
			szReturn += "\n\tBRAK PARAMETR�W";
		 // Je�li s� jakie�, wypisz w postaci PARAMETR: cu�
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

			 // Usu� niepotrzebny przecinek na ko�cu
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


// Konstruktor z samym wyra�eniem w nawiasie - domy�lny typ nawias�w
//---------------------------------------------------------------------
CalcTreeBrackets::CalcTreeBrackets(CalcTree * opExp) : CalcTree(NODE_BRACKETS), opExp(opExp), cSigns(BRACKETS_ARRAY[PARENTHESES])
{
	 // Zapobiegnij podw�jnemu przypisaniu rodzica
	if (opExp->opParent != nullptr)
		throw CalcDevError(CalcDevError::DOUBLE_PARENT_ASSOC, std::string("pr�ba podw�jnego przypisania rodzica"));

	opExp->opParent = this;
}

// Konstruktor z wyra�eniem w nawiasie i podanym typem nawias�w
//-------------------------------------------------------------------
CalcTreeBrackets::CalcTreeBrackets(CALC_BRACKET_TYPE eType, CalcTree * opExp) : CalcTree(NODE_BRACKETS), opExp(opExp), cSigns(BRACKETS_ARRAY[eType]) 
{
	 // Zapobiegnij podw�jnemu przypisaniu rodzica
	if (opExp->opParent != nullptr)
		throw CalcDevError(CalcDevError::DOUBLE_PARENT_ASSOC, std::string("pr�ba podw�jnego przypisania rodzica"));

	opExp->opParent = this;
}

CalcTreeBrackets::~CalcTreeBrackets()
{
	delete opExp;
}

void CalcTreeBrackets::checkSymbols()
{
	 // Wywo�aj metod� na dziecku
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
		 // Przygotuj pocz�tek bloku
		std::stringstream szResultStream;
		std::stringstream szExpStream(opExp->printIntern(eMode, idx));

		szResultStream << "Nawiasy " << cSigns[0] << " " << cSigns[1];
		if (eMode == PRINT_BLOCK_BRACKETS) 
			szResultStream << "\n{";

		 // Pobierz wydruk dziecka i zwi�ksz zag��bienie tabulacji
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


// Konstruktor z wyra�eniem "pod minusem"
//---------------------------------------------------------------------
CalcTreeAddInv::CalcTreeAddInv(CalcTree * opExp) : CalcTree(NODE_ADD_INV), opExp(opExp)
{
	 // Zapobiegnij podw�jnemu przypisaniu rodzica
	if (opExp->opParent != nullptr)
		throw CalcDevError(CalcDevError::DOUBLE_PARENT_ASSOC, std::string("pr�ba podw�jnego przypisania rodzica"));

	opExp->opParent = this;
}

CalcTreeAddInv::~CalcTreeAddInv()
{
	delete opExp;
}

void CalcTreeAddInv::checkSymbols()
{
	 // Wywo�aj metod� na dziecku
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
		 // Je�li rodzic to operator dwuargumentowy, silnia, lub minus unarny, dodaj nawiasy
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
		 // Przygotuj pocz�tek bloku
		std::stringstream szResultStream;
		std::stringstream szExpStream(opExp->printIntern(eMode, idx));

		szResultStream << "Liczba przeciwna";
		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n{";

		 // Pobierz wydruk dziecka i zwi�ksz zag��bienie tabulacji
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


// Konstruktor ze wska�nikami do czynnik�w
//---------------------------------------------------------------------
CalcTreeSum::CalcTreeSum(CalcTree * opSummand1, CalcTree * opSummand2) : CalcTree(NODE_SUM), opSummand1(opSummand1), opSummand2(opSummand2)
{
	 // Zapobiegnij podw�jnemu przypisaniu rodzica
	if (opSummand1->opParent != nullptr || opSummand2->opParent != nullptr)
		throw CalcDevError(CalcDevError::DOUBLE_PARENT_ASSOC, std::string("pr�ba podw�jnego przypisania rodzica"));

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
	 // Pr�buj wywo�a� na dzieciach i przeka� b��dy
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
	 // Pr�buj obliczy� sk�adniki i w razie czego wychwy� b��dy i przeka� dalej
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
		 // Je�li rodzic ma wi�kszy priorytet, dodaj nawiasy
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
		 // Przygotuj pocz�tek bloku
		std::stringstream szResultStream;
		std::stringstream szSummand1Stream(opSummand1->printIntern(eMode, idx));
		std::stringstream szSummand2Stream(opSummand2->printIntern(eMode, idx));

		szResultStream << "Suma";
		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n{";

		 // Pobierz wydruk sk�adnik�w i zwi�ksz zag��bienie tabulacji
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


// Konstruktor pobieraj�cy odjemn� i odjemnik
//---------------------------------------------------------------------
CalcTreeDiff::CalcTreeDiff(CalcTree * opMinuend, CalcTree * opSubtrahend) : CalcTree(NODE_DIFF), opMinuend(opMinuend), opSubtrahend(opSubtrahend)
{
	 // Zapobiegnij podw�jnemu przypisaniu rodzica
	if (opMinuend->opParent != nullptr || opSubtrahend->opParent != nullptr)
		throw CalcDevError(CalcDevError::DOUBLE_PARENT_ASSOC, std::string("pr�ba podw�jnego przypisania rodzica"));

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
	 // Pr�buj wywo�a� na dzieciach i przeka� b��dy
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
	 // Pr�buj obliczy� odjemn� i odjemnik i w razie czego wychwy� b��d i przeka� dalej
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
		// Je�li rodzic ma wi�kszy priorytet, dodaj nawiasy
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
		// Przygotuj pocz�tek bloku
		std::stringstream szResultStream;
		std::stringstream szMinuendStream(opMinuend->printIntern(eMode, idx));
		std::stringstream szSubtrahendStream(opSubtrahend->printIntern(eMode, idx));

		szResultStream << "R�nica";
		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n{";

		// Pobierz wydruk odjemnej i odjemnika i zwi�ksz zag��bienie tabulacji
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
	 // Zapobiegnij podw�jnemu przypisaniu rodzica
	if (opFactor1->opParent != nullptr || opFactor2->opParent != nullptr)
		throw CalcDevError(CalcDevError::DOUBLE_PARENT_ASSOC, std::string("pr�ba podw�jnego przypisania rodzica"));

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
	 // Pr�buj wywo�a� na dzieciach i przeka� b��dy
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
	 // Pr�buj obliczy� czynniki i w razie czego wychwy� b��d i przeka� dalej
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
		 // Je�li rodzic ma wi�kszy priorytet, dodaj nawiasy
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
		 // Przygotuj pocz�tek bloku
		std::stringstream szResultStream;
		std::stringstream szFactor1Stream(opFactor1->printIntern(eMode, idx));
		std::stringstream szFactor2Stream(opFactor2->printIntern(eMode, idx));

		szResultStream << "Iloczyn";
		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n{";

		 // Pobierz wydruk czynnik�w i zwi�ksz zag��bienie tabulacji
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


// Konstruktor pobieraj�cy dzieln� i dzielnik
//---------------------------------------------------------------------
CalcTreeQuot::CalcTreeQuot(CalcTree * opDividend, CalcTree * opDivisor) : CalcTree(NODE_QUOT), opDividend(opDividend), opDivisor(opDivisor)
{
	 // Zapobiegnij podw�jnemu przypisaniu rodzica
	if (opDividend->opParent != nullptr || opDivisor->opParent != nullptr)
		throw CalcDevError(CalcDevError::DOUBLE_PARENT_ASSOC, std::string("pr�ba podw�jnego przypisania rodzica"));

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
	 // Pr�buj wywo�a� na dzieciach i przeka� b��dy
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
	 // Pr�buj obliczy� dzieln� i dzielnik i w razie czego wychwy� b��d i przeka� dalej
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
		throw CalcError(CalcError::DIV_ZERO, "dzielenie przez 0... jeste� debilem xD", opDivisor->uiNodeBegIdx, opDivisor->uiNodeEndIdx);
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
		// Je�li rodzic ma wi�kszy priorytet, dodaj nawiasy
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
		// Przygotuj pocz�tek bloku
		std::stringstream szResultStream;
		std::stringstream szDividendStream(opDividend->printIntern(eMode, idx));
		std::stringstream szDivisorStream(opDivisor->printIntern(eMode, idx));

		szResultStream << "Iloraz";
		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n{";

		// Pobierz wydruk dzielnej i dzielnika i zwi�ksz zag��bienie tabulacji
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


// Konstruktor pobieraj�cy podstaw� i wyk�adnik
//---------------------------------------------------------------------
CalcTreePow::CalcTreePow(CalcTree * opBase, CalcTree * opExponent) : CalcTree(NODE_POW), opBase(opBase), opExponent(opExponent)
{
	 // Zapobiegnij podw�jnego przypisania rodzica
	if (opBase->opParent != nullptr || opExponent->opParent != nullptr)
		throw CalcDevError(CalcDevError::DOUBLE_PARENT_ASSOC, std::string("pr�ba podw�jnego przypisania rodzica"));

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
	 // Pr�buj wywo�a� na dzieciach i przeka� b��dy
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
	 // Pr�buj obliczy� podstaw� i wyk�adnik i w razie czego wychwy� b��d i przeka� dalej
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

	 // Wy�apano b��d
	CalcError::checkFenv(FENV_DOMAIN, "nieokre�lone pot�gowanie", uiNodeBegIdx, uiNodeEndIdx);
	CalcError::checkFenv(FENV_DIVBYZERO, "dzielenie przez 0 - ujemny wyk�adnik", opExponent->uiNodeBegIdx, opExponent->uiNodeEndIdx);
	CalcError::checkFenv(FENV_RANGE, "", uiNodeBegIdx, uiNodeEndIdx);

	return dResult;
}

std::string CalcTreePow::printIntern(CALC_PRINT_MODE eMode, size_t & idx)
{
	std::string szReturn = "";
	switch (eMode)
	{
	case PRINT_INLINE:
		// Je�li rodzic ma wi�kszy priorytet, dodaj nawiasy
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
		// Przygotuj pocz�tek bloku
		std::stringstream szResultStream;
		std::stringstream szBaseStream(opBase->printIntern(eMode, idx));
		std::stringstream szExponentStream(opExponent->printIntern(eMode, idx));

		szResultStream << "Pot�ga";
		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n{";

		// Pobierz wydruk podstawy i wyk�adnika i zwi�ksz zag��bienie tabulacji
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


// Konstruktor pobieraj�cy wyra�enie pod silni�
//---------------------------------------------------------------------
CalcTreeFactor::CalcTreeFactor(CalcTree * opExp) : CalcTree(NODE_FACTOR), opExp(opExp) 
{
	 // Zapobiegnij podw�jnemu przypisaniu rodzica
	if (opExp->opParent != nullptr)
		throw CalcDevError(CalcDevError::DOUBLE_PARENT_ASSOC, std::string("pr�ba podw�jnego przypisania rodzica"));

	opExp->opParent = this;
}

CalcTreeFactor::~CalcTreeFactor()
{
	delete opExp;
}

void CalcTreeFactor::checkSymbols()
{
	 // Wywo�aj metod� na dziecku
	opExp->checkSymbols();
}

double CalcTreeFactor::count()
{
	double dResult;

	CalcError::clearFenv();
	dResult = tgamma(opExp->count() + 1.0);
	CalcError::checkFenv(FENV_RANGE, "", uiNodeBegIdx, uiNodeEndIdx);
	CalcError::checkFenv(FENV_DOMANDZERO, "funkcja gamma - parametr poza dziedzin�", opExp->uiNodeBegIdx, opExp->uiNodeEndIdx);

	return dResult;
}

std::string CalcTreeFactor::printIntern(CALC_PRINT_MODE eMode, size_t & idx)
{
	std::string szReturn = "";
	switch (eMode)
	{
	case PRINT_INLINE:
		 // Je�li rodzic to pot�ga, dodaj nawiasy
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
		 // Przygotuj pocz�tek bloku
		std::stringstream szResultStream;
		std::stringstream szExpStream(opExp->printIntern(eMode, idx));

		szResultStream << "Silnia";
		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n{";

		 // Pobierz wydruk dziecka i zwi�ksz zag��bienie tabulacji
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


// Konstruktor przyjmuj�cy dzieln� i dzielnik
//--------------------------------------------------------------------
CalcTreeMod::CalcTreeMod(CalcTree * opDividend, CalcTree * opDivisor) : CalcTree(NODE_MOD), opDividend(opDividend), opDivisor(opDivisor) 
{
	 // Zapobiegnij podw�jnemu przypisaniu rodzica
	if (opDividend->opParent != nullptr || opDivisor->opParent != nullptr)
		throw CalcDevError(CalcDevError::DOUBLE_PARENT_ASSOC, std::string("pr�ba podw�jnego przypisania rodzica"));

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
	 // Pr�buj wywo�a� na dzieciach i przeka� b��dy
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
	 // Pr�buj obliczy� dzieln� i dzielnik i w razie czego wychwy� b��d i przeka� dalej
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

	// Sprawd�, czy podano liczby ca�kowite
	if (!pkutl::intel_is_int(dDividend))
		oErrors.appendError(CalcError(CalcError::EXPECTED_INT, "modulo przyjmuje tylko ca�kowite warto�ci", opDividend->uiNodeBegIdx, opDividend->uiNodeEndIdx));
	if (!pkutl::intel_is_int(dDivisor))
		oErrors.appendError(CalcError(CalcError::EXPECTED_INT, "modulo przyjmuje tylko ca�kowite warto�ci", opDivisor->uiNodeBegIdx, opDivisor->uiNodeEndIdx));

	if (oErrors.getNum())
	{
		oErrors.sortBegIdx();
		throw oErrors;
	}

	 // Zabezpiecz przed dzieleniem przez 0
	if (abs(dDivisor) < std::numeric_limits<double>::epsilon())
	{
		opDivisor->eErrorFlag = CalcError::DIV_ZERO;
		CalcError error(CalcError::DIV_ZERO, "dzielenie przez 0... jeste� debilem xD");
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
		 // Je�li rodzic ma wi�kszy priorytet, dodaj nawiasy
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
		 // Przygotuj pocz�tek bloku
		std::stringstream szResultStream;
		std::stringstream szDividendStream(opDividend->printIntern(eMode, idx));
		std::stringstream szDivisorStream(opDivisor->printIntern(eMode, idx));

		szResultStream << "Modulo";
		if (eMode == PRINT_BLOCK_BRACKETS)
			szResultStream << "\n{";

		 // Pobierz wydruk dzielnej i dzielnika i zwi�ksz zag��bienie tabulacji
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