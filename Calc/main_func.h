// main_func.h
//---------------------------------------------------------------------
// Plik nag³ówkowy z g³ównymi funkcjami okna
//---------------------------------------------------------------------

#pragma once

#include "stdafx.h"
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <regex>
#include <Windows.h>
#include "config.h"
#include "newconio.h"
#include "error_handling.h"
#include "class_calcparser.h"
#include "class_calctree.h"

// Definicje preprocesora
//---------------------------------------------------------------------

#define		BG_BLACK				0
#define		BG_DARK_RED				BACKGROUND_RED
#define		BG_DARK_GREEN			BACKGROUND_GREEN
#define		BG_DARK_BLUE			BACKGROUND_BLUE
#define		BG_DARK_YELLOW			BACKGROUND_RED | BACKGROUND_GREEN
#define		BG_DARK_CYAN			BACKGROUND_GREEN | BACKGROUND_BLUE
#define		BG_DARK_MAGENTA			BACKGROUND_BLUE | BACKGROUND_RED
#define		BG_GREY					BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE
#define		BG_DARK_GREY			BACKGROUND_INTENSITY
#define		BG_RED					BACKGROUND_RED | BACKGROUND_INTENSITY
#define		BG_GREEN				BACKGROUND_GREEN | BACKGROUND_INTENSITY
#define		BG_BLUE					BACKGROUND_BLUE | BACKGROUND_INTENSITY
#define		BG_YELLOW				BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY
#define		BG_CYAN					BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY
#define		BG_MAGENTA				BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_INTENSITY
#define		BG_WHITE				BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY

#define		FG_BLACK				0
#define		FG_DARK_RED				FOREGROUND_RED
#define		FG_DARK_GREEN			FOREGROUND_GREEN
#define		FG_DARK_BLUE			FOREGROUND_BLUE
#define		FG_DARK_YELLOW			FOREGROUND_RED | FOREGROUND_GREEN
#define		FG_DARK_CYAN			FOREGROUND_GREEN | FOREGROUND_BLUE
#define		FG_DARK_MAGENTA			FOREGROUND_BLUE | FOREGROUND_RED
#define		FG_GREY					FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#define		FG_DARK_GREY			FOREGROUND_INTENSITY
#define		FG_RED					FOREGROUND_RED | FOREGROUND_INTENSITY
#define		FG_GREEN				FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define		FG_BLUE					FOREGROUND_BLUE | FOREGROUND_INTENSITY
#define		FG_YELLOW				FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define		FG_CYAN					FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
#define		FG_MAGENTA				FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY
#define		FG_WHITE				FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY

using namespace std;

// Wyliczenia
//---------------------------------------------------------------------
enum NOTATION
{
	NOT_DEFAULT,
	NOT_FIXED,
	NOT_SCIENTIFIC
};

// Zmienne zewnêtrzne
//---------------------------------------------------------------------
extern unsigned char	ucPrecision;
extern NOTATION			eNotation;

// Zmienne globalne
//---------------------------------------------------------------------
/*unsigned short			usFlistPos;			// Indeks pocz¹tku listy
unsigned short			usFlistCurrPos;		// Indeks kursora na liœcie*/

// Funkcje
//---------------------------------------------------------------------
void ShowStartInfo();
void ShowHelp();
void ShowErrors(std::string szExp, const std::string & szSaveVar, CalcError oErrors);
void ShowResult(const std::string & szExp, const std::string & szSaveVar, double dResult);
void ShowCList(unsigned short ucPage, unsigned short ucMaxPage);
void ShowFList(unsigned short usPos, unsigned short usCurrPos);

void ProceedMain();
void ProceedCmd(std::vector<std::string> szCmdVector);
void ProceedCList(unsigned short ucPage, unsigned short ucMaxPage);
void ProceedFList(unsigned short usStartPos);

bool FetchCmd(const std::string & sInput, vector<string> &sCmdVector);
std::string ParseVar(std::string & szExp);
void ResetColor();
void FormatExp(std::string & szExp);
std::string FormatedNumber(double dVal, unsigned char ucPrecision, NOTATION eNotation);