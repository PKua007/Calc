// class_calctree.h
//---------------------------------------------------------------------
// Plik nag��wkowy z klas� reprezentuj�c� drzewo wyra�enia
//---------------------------------------------------------------------
// Obiekty przechowuj� wyra�enie w postaci drzewo. Metody pozwalaj�
// na obliczanie wyra�enia, wydrukowanie go w postaci ci�gu znak�w,
// zapisywane s� tak�e informacje o b��dach logicznych
// (nieznane funkcje, zmienne, warto�ci spoza dziedziny, itd.)
//---------------------------------------------------------------------
// (C) 2016 PKua
//---------------------------------------------------------------------

#pragma once

#include <string>
#include <sstream>
#include <math.h>
#include <fenv.h>

#include "pkua_utils.h"
#include "error_handling.h"
#include "var_engine.h"
#include "funct_engine.h"

#define friendzone	friend

// Wyliczenie rodzaj�w w�z��w drzewa
//
// NAZWA STA�EJ		ODPOWIADAJ�CA KLASA			OPIS						SK�ADNIA												KOLEJNO�� DZIA�A�		��CZNO��
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

enum CALC_NODE_TYPE
{
	NODE_EMPTY,		// class CalcTreeEmpty		- pusty w�ze� jak Malwina   ---														0						---
	NODE_VALUE,		// class CalcTreeValue		- liczba					[liczba]												0						---
	NODE_VAR,		// class CalcTreeVar		- zmienna/sta�a				[nazwa]													0						---
	NODE_FUNCT,		// class CalcTreeFunct		- funkcja					[nazwa] ([param], [param], ..., [paramn])				0						---
	NODE_BRACKETS,	// class CalcTreeBrackets	- wyra�enie w nawiasie		([wyra�enie]) lub \[[wyra�enie]\] lub {[wyra�enie]}		0						---
	NODE_ADD_INV,	// class CalcTreeAddInv		- liczba przeciwna			-[wyra�enie]											3						prawostronna
	NODE_SUM,		// class CalcTreeSum		- suma						[wyra�enie] + [wyra�enie]								1						prawostronna
	NODE_DIFF,		// class CalcTreeDiff		- r�nica					[wyra�enie] - [wyra�enie]								1						prawostronna
	NODE_PRODUCT,	// class CalcTreeProduct	- mno�enie					[wyra�enie] * [wyra�enie]								2						prawostronna
	NODE_QUOT,		// class CalcTreeQuot		- dzielenie					[wyra�enie] / [wyra�enie]								2						prawostronna
	NODE_POW,		// class CalcTreePow		- pot�gowanie				[wyra�enie] ^ [wyra�enie]								4						lewostronna
	NODE_FACTOR,	// class CalcTreeFactor		- silnia					[wyra�enie]!											5						prawostronna
	NODE_MOD,		// class CalcTreeMod		- dzielenie z reszt�		[wyra�enie] % [wyra�enie]								2						prawostronna

	NODE_NUM_OF
};

 // Wyliczenie rozdaj�w ��czno�ci operator�w
enum CALC_OP_ASSOC
{
	OP_ASSOC_NO,
	OP_ASSOC_LEFT,
	OP_ASSOC_RIGHT
};

 // Wyliczenie rodzaj�w operatora
enum CALC_OP_TYPE
{
	OP_NOT_OP,		// Nie jest to operator
	OP_UNARY,		// Operator unarny
	OP_BINARY		// Operator binarny
};

 // Wyliczenie tryb�w wy�wietlania
enum CALC_PRINT_MODE
{
	PRINT_INLINE,
	PRINT_BLOCK,
	PRINT_BLOCK_BRACKETS
};

 // Wyliczenie rodzaj�w nawias�w
enum CALC_BRACKET_TYPE
{
	PARENTHESES,
	SQUARE_BRACKETS,
	BRACES,
	TUPLES,
	BRACKET_TYPE_NUM
};

 // Tablica ze znakami nawias�w
const char BRACKETS_ARRAY[4][2] =
	{ {'(', ')'},
	  {'[', ']'},
	  {'{', '}'},
	  {'<', '>'} };

 // Struktura informacji o rodzaju w�z�a
struct CalcTreeNodeData
{
	const unsigned short	usOpPrec;
	const CALC_OP_ASSOC		eOpAssoc;
	const CALC_OP_TYPE		eOpNaryType;

	CalcTreeNodeData() = delete;
	CalcTreeNodeData(unsigned short usOpPrec, CALC_OP_ASSOC eOpAssoc, CALC_OP_TYPE eOpNaryType) : usOpPrec(usOpPrec), eOpAssoc(eOpAssoc), eOpNaryType(eOpNaryType) {}
};

 // Tablica z informacjami o rodzaju w�z�a
const CalcTreeNodeData CALC_NODE_DATA[NODE_NUM_OF] = {
	CalcTreeNodeData(0, OP_ASSOC_NO, OP_NOT_OP),		// NODE_EMPTY
	CalcTreeNodeData(0, OP_ASSOC_NO, OP_NOT_OP),		// NODE_VALUE
	CalcTreeNodeData(0, OP_ASSOC_NO, OP_NOT_OP),		// NODE_VAR
	CalcTreeNodeData(0, OP_ASSOC_NO, OP_NOT_OP),		// NODE_FUNCT
	CalcTreeNodeData(0, OP_ASSOC_NO, OP_NOT_OP),		// NODE_BRACKETS
	CalcTreeNodeData(3, OP_ASSOC_RIGHT, OP_UNARY),		// NODE_ADD_INV
	CalcTreeNodeData(1, OP_ASSOC_RIGHT, OP_BINARY),		// NODE_SUM
	CalcTreeNodeData(1, OP_ASSOC_RIGHT, OP_BINARY),		// NODE_DIFF
	CalcTreeNodeData(2, OP_ASSOC_RIGHT, OP_BINARY),		// NODE_PRODUCT
	CalcTreeNodeData(2, OP_ASSOC_RIGHT, OP_BINARY),		// NODE_QUOT
	CalcTreeNodeData(4, OP_ASSOC_LEFT, OP_BINARY),		// NODE_POW
	CalcTreeNodeData(5, OP_ASSOC_RIGHT, OP_UNARY),		// NODE_FACTOR
	CalcTreeNodeData(2, OP_ASSOC_RIGHT, OP_BINARY)		// NODE_MOD
};
 

// Klasa abstrakcyjna CalcTree
//---------------------------------------------------------------------
// Stanowi baz� dla wszystkich rodzaj�w w�z��w w drzewie wyra�enia.
// Nie ma rozr�nienia dla korzenia i ga��zi, ka�da podstruktura
// drzewa jest pe�noprawnym drzewem
//---------------------------------------------------------------------
class CalcTree
{
private:
	CalcTree *				opParent = nullptr;				// Wska�nik do rodzica
	CalcErrType				eErrorFlag = CalcError::NO;		// Flaga b��du

	const CALC_NODE_TYPE	eNodeType;						// Typ w�z�a
	const unsigned short	usOpPrecedence;					// Rz�d operatora
	const CALC_OP_ASSOC		eOpAssoc;						// Rodzaj ��czno�ci operatora
	size_t					uiNodeBegIdx;					// Wsp�rz�dna pocz�tku w�z�a w wyniku printIntern(PRINT_INLINE) na korzeniu
	size_t					uiNodeEndIdx;					// Wsp�rz�dna ko�ca w�z�a w wyniku printIntern(PRINT_INLINE) na korzeniu

	virtual	std::string		printIntern(CALC_PRINT_MODE eMode, size_t & idx) = 0;

protected:
	CalcTree(CALC_NODE_TYPE	eNodeType);

public:
	virtual	void			checkSymbols() = 0;
	virtual	double			count() = 0;
	virtual void			appendNode(CalcTree * opNode) = 0;
	virtual CalcTree *		detachNode() = 0;
	virtual ~CalcTree() {};

	const CALC_NODE_TYPE	&getType() const;
	const unsigned short	&getOpPrecedence() const;
	const CALC_OP_ASSOC		&getOpAssocType() const;
	CalcTree *				getParent() const;
	std::string				print(CALC_PRINT_MODE eMode = PRINT_INLINE);

	friendzone class CalcTreeValue;
	friendzone class CalcTreeVar;
	friendzone class CalcTreeFunct;
	friendzone class CalcTreeBrackets;
	friendzone class CalcTreeAddInv;
	friendzone class CalcTreeSum;
	friendzone class CalcTreeDiff;
	friendzone class CalcTreeProduct;
	friendzone class CalcTreeQuot;
	friendzone class CalcTreePow;
	friendzone class CalcTreeFactor;
	friendzone class CalcTreeMod;
};

// Klasa CalcTreeEmpty
//-------------------------------------------------------------------
// Reprezentuje pusty w�ze�
//-------------------------------------------------------------------
// NAZWA STA�EJ		ODPOWIADAJ�CA KLASA			SK�ADNIA			KOLEJNO�� DZIA�A�		��CZNO��
// NODE_EMPTY		class CalcTreeEmpty			---					0						---
//--------------------------------------------------------------------------------------------------
class CalcTreeEmpty : public CalcTree
{
private:
	virtual	std::string		printIntern(CALC_PRINT_MODE eMode, size_t & idx);

public:
	CalcTreeEmpty();

	virtual void			checkSymbols();
	virtual	double			count();
	virtual	void			appendNode(CalcTree * opNode);
	virtual CalcTree *		detachNode();
};

// Klasa CalcTreeValue
//-------------------------------------------------------------------
// Reprezentuje warto�� liczbow�
//-------------------------------------------------------------------
// NAZWA STA�EJ		ODPOWIADAJ�CA KLASA			SK�ADNIA			KOLEJNO�� DZIA�A�		��CZNO��
// NODE_VALUE		class CalcTreeValue			[liczba]			0						---
//--------------------------------------------------------------------------------------------------
class CalcTreeValue : public CalcTree
{
private:
	double					dValue;		// Przechowywana warto��

	virtual	std::string		printIntern(CALC_PRINT_MODE eMode, size_t & idx);

public:
	CalcTreeValue(double dValue);

	virtual void			checkSymbols();
	virtual	double			count();
	virtual	void			appendNode(CalcTree * opNode);
	virtual CalcTree *		detachNode();
};

// Klasa CalcTreeVar
//-------------------------------------------------------------------
// Reprezentuje zmienn�/sta��
//-------------------------------------------------------------------
// NAZWA STA�EJ		ODPOWIADAJ�CA KLASA			SK�ADNIA			KOLEJNO�� DZIA�A�		��CZNO��
// NODE_VAR			class CalcTreeVar			[nazwa]				0						---
//--------------------------------------------------------------------------------------------------
class CalcTreeVar : public CalcTree
{
private:
	std::string				szName;		// Przechowywana nazwa zmiennej

	virtual	std::string		printIntern(CALC_PRINT_MODE eMode, size_t & idx);

public:
	CalcTreeVar(std::string szName);

	virtual	void			checkSymbols();
	virtual	double			count();
	virtual	void			appendNode(CalcTree * opNode);
	virtual CalcTree *		detachNode();

	const std::string		&getName() const { return szName; }
};

// Klasa CalcTreeFunct
//-------------------------------------------------------------------
// Reprezentuje funkcj�
//-------------------------------------------------------------------
// NAZWA STA�EJ		ODPOWIADAJ�CA KLASA			SK�ADNIA										KOLEJNO�� DZIA�A�		��CZNO��
// NODE_FUNCT		class CalcTreeFunct			[nazwa] ([param], [param], ..., [paramn])		0						---
//------------------------------------------------------------------------------------------------------------------------------
class CalcTreeFunct : public CalcTree
{
private:
	std::string				szName;				// Przechowywana nazwa funkcji
	std::vector<CalcTree*>	opParamsVector;		// Przechowywana tablica parametr�w funkcji

	virtual	std::string		printIntern(CALC_PRINT_MODE eMode, size_t & idx);

public:
	CalcTreeFunct(std::string szName, std::vector<CalcTree*> opParamsVector);

	virtual	void			checkSymbols();
	virtual	double			count();
	virtual	void			appendNode(CalcTree * opNode);
	virtual CalcTree *		detachNode();
};

// Klasa CalcTreeBrackets
//-------------------------------------------------------------------
// Reprezentuje wyra�enie w nawiasie
//-------------------------------------------------------------------
// NAZWA STA�EJ		ODPOWIADAJ�CA KLASA			SK�ADNIA												KOLEJNO�� DZIA�A�		��CZNO��
// NODE_BRACKETS	class CalcTreeBrackets		([wyra�enie]) lub \[[wyra�enie]\] lub {[wyra�enie]}		0						---
//--------------------------------------------------------------------------------------------------------------------------------------
class CalcTreeBrackets : public CalcTree
{
private:
	CalcTree *				opExp;				// Przechowywane wyra�enie
	const char *			cSigns;				// Znak otwieraj�ce i zamykaj�ce

	virtual	std::string		printIntern(CALC_PRINT_MODE eMode, size_t & idx);

public:
	CalcTreeBrackets(CalcTree * opExp);
	CalcTreeBrackets(CALC_BRACKET_TYPE eType, CalcTree * opExp);
	~CalcTreeBrackets();

	virtual	void			checkSymbols();
	virtual	double			count();
	virtual	void			appendNode(CalcTree * opNode);
	virtual CalcTree *		detachNode();
};

// Klasa CalcTreeAddInv
//-------------------------------------------------------------------
// Reprezentuje liczb� przeciwn� (minus unarny)
//-------------------------------------------------------------------
// NAZWA STA�EJ		ODPOWIADAJ�CA KLASA			SK�ADNIA			KOLEJNO�� DZIA�A�		��CZNO��
// NODE_ADD_INV		class CalcTreeAddInv		-[wyra�enie]		3						prawostronna
//------------------------------------------------------------------------------------------------------
class CalcTreeAddInv : public CalcTree
{
private:
	CalcTree *				opExp;			// Wyra�enie "pod minusem"

	virtual	std::string		printIntern(CALC_PRINT_MODE eMode, size_t & idx);

public:
	CalcTreeAddInv(CalcTree * opExp);
	~CalcTreeAddInv();

	virtual	void			checkSymbols();
	virtual	double			count();
	virtual	void			appendNode(CalcTree * opNode);
	virtual CalcTree *		detachNode();
};

// Klasa CalcTreeSum
//-------------------------------------------------------------------
// Reprezentuje sum� liczb
//-------------------------------------------------------------------
// NAZWA STA�EJ		ODPOWIADAJ�CA KLASA			SK�ADNIA						KOLEJNO�� DZIA�A�		��CZNO��
// NODE_SUM			class CalcTreeSum			[wyra�enie] + [wyra�enie]		1						prawostronna
//------------------------------------------------------------------------------------------------------------------
class CalcTreeSum : public CalcTree
{
private:
	CalcTree *				opSummand1;			// Pierwszy sk�adnik sumy
	CalcTree *				opSummand2;			// Drugi sk�adnik sumy

	virtual	std::string		printIntern(CALC_PRINT_MODE eMode, size_t & idx);

public:
	CalcTreeSum(CalcTree * opSummand1, CalcTree * opSummand2);
	~CalcTreeSum();

	virtual	void			checkSymbols();
	virtual	double			count();
	virtual	void			appendNode(CalcTree * opNode);
	virtual CalcTree *		detachNode();
};

// Klasa CalcTreeDiff
//-------------------------------------------------------------------
// Reprezentuje r�nic� liczb
//-------------------------------------------------------------------
// NAZWA STA�EJ		ODPOWIADAJ�CA KLASA			SK�ADNIA							KOLEJNO�� DZIA�A�		��CZNO��
// NODE_DIFF		class CalcTreeDiff			[wyra�enie] - [wyra�enie]			1						prawostronna
//----------------------------------------------------------------------------------------------------------------------
class CalcTreeDiff : public CalcTree
{
private:
	CalcTree *				opMinuend;			// Odjemna r�nicy
	CalcTree *				opSubtrahend;		// Odjemnik r�nicy

	virtual	std::string		printIntern(CALC_PRINT_MODE eMode, size_t & idx);

public:
	CalcTreeDiff(CalcTree * opMinuend, CalcTree * opSubtrahend);
	~CalcTreeDiff();

	virtual	void			checkSymbols();
	virtual	double			count();
	virtual	void			appendNode(CalcTree * opNode);
	virtual CalcTree *		detachNode();
};

// Klasa CalcTreeProduct
//-------------------------------------------------------------------
// Reprezentuje mno�enie liczb
//-------------------------------------------------------------------
// NAZWA STA�EJ		ODPOWIADAJ�CA KLASA			SK�ADNIA						KOLEJNO�� DZIA�A�		��CZNO��
// NODE_PRODUCT		class CalcTreeProduct		[wyra�enie] * [wyra�enie]		2						prawostronna
//------------------------------------------------------------------------------------------------------------------
class CalcTreeProduct : public CalcTree
{
private:
	CalcTree *				opFactor1;		// Pierwszy czynnik iloczynu
	CalcTree *				opFactor2;		// Drugi czynnik iloczynu

	virtual	std::string		printIntern(CALC_PRINT_MODE eMode, size_t & idx);

public:
	CalcTreeProduct(CalcTree * opFactor1, CalcTree * opFactor2);
	~CalcTreeProduct();

	virtual	void			checkSymbols();
	virtual	double			count();
	virtual	void			appendNode(CalcTree * opNode);
	virtual CalcTree *		detachNode();
};

// Klasa CalcTreeQuot
//-------------------------------------------------------------------
// Reprezentuje dzielenie liczb
//-------------------------------------------------------------------
// NAZWA STA�EJ		ODPOWIADAJ�CA KLASA			SK�ADNIA						KOLEJNO�� DZIA�A�		��CZNO��
// NODE_QUOT		class CalcTreeQuot			[wyra�enie] / [wyra�enie]		2						prawostronna
//------------------------------------------------------------------------------------------------------------------
class CalcTreeQuot : public CalcTree
{
private:
	CalcTree *				opDividend;		// Dzielna ilorazu
	CalcTree *				opDivisor;		// Dzielnik ilorazu

	virtual	std::string		printIntern(CALC_PRINT_MODE eMode, size_t & idx);

public:
	CalcTreeQuot(CalcTree * opDividend, CalcTree * opDivisor);
	~CalcTreeQuot();

	virtual	void			checkSymbols();
	virtual	double			count();
	virtual	void			appendNode(CalcTree * opNode);
	virtual CalcTree *		detachNode();
};

// Klasa CalcTreePow
//-------------------------------------------------------------------
// Reprezentuje pot�gowanie liczb
//-------------------------------------------------------------------
// NAZWA STA�EJ		ODPOWIADAJ�CA KLASA			SK�ADNIA						KOLEJNO�� DZIA�A�		��CZNO��
// NODE_POW			class CalcTreePow			[wyra�enie] ^ [wyra�enie]		4						lewostronna
//------------------------------------------------------------------------------------------------------------------
class CalcTreePow : public CalcTree
{
private:
	CalcTree *				opBase;			// Podstawa pot�gi
	CalcTree *				opExponent;		// Wyk�adnik pot�gi

	virtual	std::string		printIntern(CALC_PRINT_MODE eMode, size_t & idx);

public:
	CalcTreePow(CalcTree * opBase, CalcTree * opExponent);
	~CalcTreePow();

	virtual	void			checkSymbols();
	virtual	double			count();
	virtual	void			appendNode(CalcTree * opNode);
	virtual CalcTree *		detachNode();
};

// Klasa CalcTreeFactor
//-------------------------------------------------------------------
// Reprezentuje silni�
//-------------------------------------------------------------------
// NAZWA STA�EJ		ODPOWIADAJ�CA KLASA			SK�ADNIA			KOLEJNO�� DZIA�A�		��CZNO��
// NODE_FACTOR		class CalcTreeFactor		[wyra�enie]!		5						prawostronna
//------------------------------------------------------------------------------------------------------
class CalcTreeFactor : public CalcTree
{
private:
	CalcTree *				opExp;		// Wyra�enie do obliczenia silni

	virtual	std::string		printIntern(CALC_PRINT_MODE eMode, size_t & idx);

public:
	CalcTreeFactor(CalcTree * opExp);
	~CalcTreeFactor();

	virtual	void			checkSymbols();
	virtual	double			count();
	virtual	void			appendNode(CalcTree * opNode);
	virtual CalcTree *		detachNode();
};

// Klasa CalcTreeMod
//-------------------------------------------------------------------
// Reprezentuje warto�� liczbow�
//-------------------------------------------------------------------
// NAZWA STA�EJ		ODPOWIADAJ�CA KLASA			SK�ADNIA						KOLEJNO�� DZIA�A�		��CZNO��
// NODE_MOD			class CalcTreeMod			[wyra�enie] % [wyra�enie]		2						prawostronna
//------------------------------------------------------------------------------------------------------------------
class CalcTreeMod : public CalcTree
{
private:
	CalcTree *				opDividend;		// Dzielna ilorazu
	CalcTree *				opDivisor;		// Dzielnik ilorazu

	virtual	std::string		printIntern(CALC_PRINT_MODE eMode, size_t & idx);

public:
	CalcTreeMod(CalcTree * opDividend, CalcTree * opDivisor);
	~CalcTreeMod();

	virtual	void			checkSymbols();
	virtual	double			count();
	virtual	void			appendNode(CalcTree * opNode);
	virtual CalcTree *		detachNode();
};