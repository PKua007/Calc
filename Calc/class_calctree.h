// class_calctree.h
//---------------------------------------------------------------------
// Plik nag³ówkowy z klas¹ reprezentuj¹c¹ drzewo wyra¿enia
//---------------------------------------------------------------------
// Obiekty przechowuj¹ wyra¿enie w postaci drzewo. Metody pozwalaj¹
// na obliczanie wyra¿enia, wydrukowanie go w postaci ci¹gu znaków,
// zapisywane s¹ tak¿e informacje o b³êdach logicznych
// (nieznane funkcje, zmienne, wartoœci spoza dziedziny, itd.)
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

// Wyliczenie rodzajów wêz³ów drzewa
//
// NAZWA STA£EJ		ODPOWIADAJ¥CA KLASA			OPIS						SK£ADNIA												KOLEJNOŒÆ DZIA£AÑ		£¥CZNOŒÆ
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

enum CALC_NODE_TYPE
{
	NODE_EMPTY,		// class CalcTreeEmpty		- pusty wêze³ jak Malwina   ---														0						---
	NODE_VALUE,		// class CalcTreeValue		- liczba					[liczba]												0						---
	NODE_VAR,		// class CalcTreeVar		- zmienna/sta³a				[nazwa]													0						---
	NODE_FUNCT,		// class CalcTreeFunct		- funkcja					[nazwa] ([param], [param], ..., [paramn])				0						---
	NODE_BRACKETS,	// class CalcTreeBrackets	- wyra¿enie w nawiasie		([wyra¿enie]) lub \[[wyra¿enie]\] lub {[wyra¿enie]}		0						---
	NODE_ADD_INV,	// class CalcTreeAddInv		- liczba przeciwna			-[wyra¿enie]											3						prawostronna
	NODE_SUM,		// class CalcTreeSum		- suma						[wyra¿enie] + [wyra¿enie]								1						prawostronna
	NODE_DIFF,		// class CalcTreeDiff		- ró¿nica					[wyra¿enie] - [wyra¿enie]								1						prawostronna
	NODE_PRODUCT,	// class CalcTreeProduct	- mno¿enie					[wyra¿enie] * [wyra¿enie]								2						prawostronna
	NODE_QUOT,		// class CalcTreeQuot		- dzielenie					[wyra¿enie] / [wyra¿enie]								2						prawostronna
	NODE_POW,		// class CalcTreePow		- potêgowanie				[wyra¿enie] ^ [wyra¿enie]								4						lewostronna
	NODE_FACTOR,	// class CalcTreeFactor		- silnia					[wyra¿enie]!											5						prawostronna
	NODE_MOD,		// class CalcTreeMod		- dzielenie z reszt¹		[wyra¿enie] % [wyra¿enie]								2						prawostronna

	NODE_NUM_OF
};

 // Wyliczenie rozdajów ³¹cznoœci operatorów
enum CALC_OP_ASSOC
{
	OP_ASSOC_NO,
	OP_ASSOC_LEFT,
	OP_ASSOC_RIGHT
};

 // Wyliczenie rodzajów operatora
enum CALC_OP_TYPE
{
	OP_NOT_OP,		// Nie jest to operator
	OP_UNARY,		// Operator unarny
	OP_BINARY		// Operator binarny
};

 // Wyliczenie trybów wyœwietlania
enum CALC_PRINT_MODE
{
	PRINT_INLINE,
	PRINT_BLOCK,
	PRINT_BLOCK_BRACKETS
};

 // Wyliczenie rodzajów nawiasów
enum CALC_BRACKET_TYPE
{
	PARENTHESES,
	SQUARE_BRACKETS,
	BRACES,
	TUPLES,
	BRACKET_TYPE_NUM
};

 // Tablica ze znakami nawiasów
const char BRACKETS_ARRAY[4][2] =
	{ {'(', ')'},
	  {'[', ']'},
	  {'{', '}'},
	  {'<', '>'} };

 // Struktura informacji o rodzaju wêz³a
struct CalcTreeNodeData
{
	const unsigned short	usOpPrec;
	const CALC_OP_ASSOC		eOpAssoc;
	const CALC_OP_TYPE		eOpNaryType;

	CalcTreeNodeData() = delete;
	CalcTreeNodeData(unsigned short usOpPrec, CALC_OP_ASSOC eOpAssoc, CALC_OP_TYPE eOpNaryType) : usOpPrec(usOpPrec), eOpAssoc(eOpAssoc), eOpNaryType(eOpNaryType) {}
};

 // Tablica z informacjami o rodzaju wêz³a
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
// Stanowi bazê dla wszystkich rodzajów wêz³ów w drzewie wyra¿enia.
// Nie ma rozró¿nienia dla korzenia i ga³êzi, ka¿da podstruktura
// drzewa jest pe³noprawnym drzewem
//---------------------------------------------------------------------
class CalcTree
{
private:
	CalcTree *				opParent = nullptr;				// WskaŸnik do rodzica
	CalcErrType				eErrorFlag = CalcError::NO;		// Flaga b³êdu

	const CALC_NODE_TYPE	eNodeType;						// Typ wêz³a
	const unsigned short	usOpPrecedence;					// Rz¹d operatora
	const CALC_OP_ASSOC		eOpAssoc;						// Rodzaj ³¹cznoœci operatora
	size_t					uiNodeBegIdx;					// Wspó³rzêdna pocz¹tku wêz³a w wyniku printIntern(PRINT_INLINE) na korzeniu
	size_t					uiNodeEndIdx;					// Wspó³rzêdna koñca wêz³a w wyniku printIntern(PRINT_INLINE) na korzeniu

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
// Reprezentuje pusty wêze³
//-------------------------------------------------------------------
// NAZWA STA£EJ		ODPOWIADAJ¥CA KLASA			SK£ADNIA			KOLEJNOŒÆ DZIA£AÑ		£¥CZNOŒÆ
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
// Reprezentuje wartoœæ liczbow¹
//-------------------------------------------------------------------
// NAZWA STA£EJ		ODPOWIADAJ¥CA KLASA			SK£ADNIA			KOLEJNOŒÆ DZIA£AÑ		£¥CZNOŒÆ
// NODE_VALUE		class CalcTreeValue			[liczba]			0						---
//--------------------------------------------------------------------------------------------------
class CalcTreeValue : public CalcTree
{
private:
	double					dValue;		// Przechowywana wartoœæ

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
// Reprezentuje zmienn¹/sta³¹
//-------------------------------------------------------------------
// NAZWA STA£EJ		ODPOWIADAJ¥CA KLASA			SK£ADNIA			KOLEJNOŒÆ DZIA£AÑ		£¥CZNOŒÆ
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
// Reprezentuje funkcjê
//-------------------------------------------------------------------
// NAZWA STA£EJ		ODPOWIADAJ¥CA KLASA			SK£ADNIA										KOLEJNOŒÆ DZIA£AÑ		£¥CZNOŒÆ
// NODE_FUNCT		class CalcTreeFunct			[nazwa] ([param], [param], ..., [paramn])		0						---
//------------------------------------------------------------------------------------------------------------------------------
class CalcTreeFunct : public CalcTree
{
private:
	std::string				szName;				// Przechowywana nazwa funkcji
	std::vector<CalcTree*>	opParamsVector;		// Przechowywana tablica parametrów funkcji

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
// Reprezentuje wyra¿enie w nawiasie
//-------------------------------------------------------------------
// NAZWA STA£EJ		ODPOWIADAJ¥CA KLASA			SK£ADNIA												KOLEJNOŒÆ DZIA£AÑ		£¥CZNOŒÆ
// NODE_BRACKETS	class CalcTreeBrackets		([wyra¿enie]) lub \[[wyra¿enie]\] lub {[wyra¿enie]}		0						---
//--------------------------------------------------------------------------------------------------------------------------------------
class CalcTreeBrackets : public CalcTree
{
private:
	CalcTree *				opExp;				// Przechowywane wyra¿enie
	const char *			cSigns;				// Znak otwieraj¹ce i zamykaj¹ce

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
// Reprezentuje liczbê przeciwn¹ (minus unarny)
//-------------------------------------------------------------------
// NAZWA STA£EJ		ODPOWIADAJ¥CA KLASA			SK£ADNIA			KOLEJNOŒÆ DZIA£AÑ		£¥CZNOŒÆ
// NODE_ADD_INV		class CalcTreeAddInv		-[wyra¿enie]		3						prawostronna
//------------------------------------------------------------------------------------------------------
class CalcTreeAddInv : public CalcTree
{
private:
	CalcTree *				opExp;			// Wyra¿enie "pod minusem"

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
// Reprezentuje sumê liczb
//-------------------------------------------------------------------
// NAZWA STA£EJ		ODPOWIADAJ¥CA KLASA			SK£ADNIA						KOLEJNOŒÆ DZIA£AÑ		£¥CZNOŒÆ
// NODE_SUM			class CalcTreeSum			[wyra¿enie] + [wyra¿enie]		1						prawostronna
//------------------------------------------------------------------------------------------------------------------
class CalcTreeSum : public CalcTree
{
private:
	CalcTree *				opSummand1;			// Pierwszy sk³adnik sumy
	CalcTree *				opSummand2;			// Drugi sk³adnik sumy

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
// Reprezentuje ró¿nicê liczb
//-------------------------------------------------------------------
// NAZWA STA£EJ		ODPOWIADAJ¥CA KLASA			SK£ADNIA							KOLEJNOŒÆ DZIA£AÑ		£¥CZNOŒÆ
// NODE_DIFF		class CalcTreeDiff			[wyra¿enie] - [wyra¿enie]			1						prawostronna
//----------------------------------------------------------------------------------------------------------------------
class CalcTreeDiff : public CalcTree
{
private:
	CalcTree *				opMinuend;			// Odjemna ró¿nicy
	CalcTree *				opSubtrahend;		// Odjemnik ró¿nicy

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
// Reprezentuje mno¿enie liczb
//-------------------------------------------------------------------
// NAZWA STA£EJ		ODPOWIADAJ¥CA KLASA			SK£ADNIA						KOLEJNOŒÆ DZIA£AÑ		£¥CZNOŒÆ
// NODE_PRODUCT		class CalcTreeProduct		[wyra¿enie] * [wyra¿enie]		2						prawostronna
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
// NAZWA STA£EJ		ODPOWIADAJ¥CA KLASA			SK£ADNIA						KOLEJNOŒÆ DZIA£AÑ		£¥CZNOŒÆ
// NODE_QUOT		class CalcTreeQuot			[wyra¿enie] / [wyra¿enie]		2						prawostronna
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
// Reprezentuje potêgowanie liczb
//-------------------------------------------------------------------
// NAZWA STA£EJ		ODPOWIADAJ¥CA KLASA			SK£ADNIA						KOLEJNOŒÆ DZIA£AÑ		£¥CZNOŒÆ
// NODE_POW			class CalcTreePow			[wyra¿enie] ^ [wyra¿enie]		4						lewostronna
//------------------------------------------------------------------------------------------------------------------
class CalcTreePow : public CalcTree
{
private:
	CalcTree *				opBase;			// Podstawa potêgi
	CalcTree *				opExponent;		// Wyk³adnik potêgi

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
// Reprezentuje silniê
//-------------------------------------------------------------------
// NAZWA STA£EJ		ODPOWIADAJ¥CA KLASA			SK£ADNIA			KOLEJNOŒÆ DZIA£AÑ		£¥CZNOŒÆ
// NODE_FACTOR		class CalcTreeFactor		[wyra¿enie]!		5						prawostronna
//------------------------------------------------------------------------------------------------------
class CalcTreeFactor : public CalcTree
{
private:
	CalcTree *				opExp;		// Wyra¿enie do obliczenia silni

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
// Reprezentuje wartoœæ liczbow¹
//-------------------------------------------------------------------
// NAZWA STA£EJ		ODPOWIADAJ¥CA KLASA			SK£ADNIA						KOLEJNOŒÆ DZIA£AÑ		£¥CZNOŒÆ
// NODE_MOD			class CalcTreeMod			[wyra¿enie] % [wyra¿enie]		2						prawostronna
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