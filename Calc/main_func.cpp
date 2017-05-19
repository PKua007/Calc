// main_func.cpp
//---------------------------------------------------------------------
// Plik �rod�owy z g��wnymi funkcjami okna
//---------------------------------------------------------------------

#include "stdafx.h"
#include "main_func.h"
#include "newconio.h"
#include "var_engine.h"
#include "funct_engine.h"


// Tablica z kolorami kolejnych zagnie�d�e� nawias�w
#define	BRACKET_COLORS_NUM 14
WORD usBracketColorArr[] = {
	FG_DARK_CYAN,
	FG_GREEN,
	FG_YELLOW,
	FG_MAGENTA,
	FG_CYAN,
	FG_WHITE,
	FG_RED,
	FG_BLUE,
	FG_DARK_MAGENTA,
	FG_DARK_RED,
	FG_DARK_GREEN,
	FG_DARK_GREY,
	FG_DARK_YELLOW,
	FG_GREY
};


// Inicjalizacja zmiennych extern
//---------------------------------------------------------------------
unsigned char	ucPrecision = _DEFAULT_PREC;
NOTATION		eNotation = NOT_DEFAULT;

// Funkcja pokazuje info startowe kalkulatora
//-------------------------------------------
void ShowStartInfo()
{
	FillFrame(FRAME_DOUBLE, FG_GREEN | BG_DARK_GREEN, 0, 0, GetBufferSize().X, 5);

	TextAttr(FG_WHITE | BG_DARK_GREEN);
	Center(1, _pl("KALKULATOR NAUKOWY"));
	CenterFormatted(3, "+, -, *, /, %, (), ^, !, sta�e {yellow}@clist{*}, funkcje {yellow}@flist");
	TextAttr(FG_GREEN | BG_DARK_GREEN);
	Center(4, " (C) Piotr \"PKua\" Kubala 2016 ");

	std::string szHelp[2] = { _pl("@exit, @q - wyj�cie"), _pl("@help - lista komend") };
	GotoXY((SHORT)(GetBufferSize().X - 2 - szHelp[0].length()), 2);
	std::cout << szHelp[0];
	GotoXY((SHORT)(GetBufferSize().X - 2 - szHelp[1].length()), 3);
	std::cout << szHelp[1];

	GotoXY(0, 5);
	TextAttr(FG_DARK_GREEN);
	std::cout << std::endl;
	WriteFormatted(">> Wpisz wyra�enie, kt�re chcesz obliczy� i wci�nij {black|green} ENTER {*}. Przyk�ady:\r\n");
	WriteFormatted("{grey}>> 2 + 2\r\n");
	WriteFormatted("{grey}>> (18 + 4 - 6!) / (2.0 + 4,3 + .8^3)\r\n");
	WriteFormatted("{grey}>> sqrt(sin(45; deg) + cos(pi/6, rad))\r\n");
	WriteFormatted(">> Dost�pne s� r�wnie� komendy zaczynaj�ce si� od {green}@{*}. Mo�esz wy�wietli� ich list� wpisuj�c {green}@help{*}.\r\n\r\n");

	ResetColor();
}

// Funkcja pokazuje pomoc
//-------------------------------------------------------------
void ShowHelp()
{
	std::stringstream szHelpStream;

	TextAttr(FG_DARK_GREEN);
	szHelpStream << ">> {green}@exit{*}, {green}@q{*} - wyj�cie" << std::endl;
	szHelpStream << ">> {green}@prec {dark_yellow}l. cyfr znacz�cych{*} - ustaw liczb� wy�wietlanych cyfr znacz�cych";
	szHelpStream << " ({dark_cyan}" STRFY(_MIN_PREC) "{*} - {dark_cyan}" STRFY(_MAX_PREC) << _pl("{*}; domy�lnie {dark_cyan}") << STRFY(_DEFAULT_PREC) "{*})" << std::endl;
	szHelpStream << ">> {green}@not {dark_yellow}fixed|scientific|default{*} - wymu� wy�wietlanie w notacji zwyk�ej/naukowej/domy�lnej (mieszana)" << std::endl;
	szHelpStream << ">> {green}@clist{*} - wy�wietl list� zmiennych" << std::endl;
	szHelpStream << ">> {green}@flist{*} - wy�wietl list� funkcji" << std::endl;
	szHelpStream << ">> {green}@flist{*} {dark_yellow}indeks funkcji {dark_cyan}0{dark_yellow} - {dark_cyan}" << (gFunctEngine.numOf() - 1) << "{dark_yellow}{*} - wy�wietl list� funkcji na okre�lonej pozycji" << std::endl;
	szHelpStream << ">> {green}@flist{*} {dark_yellow}nazwa funkcji{*} - wy�wietl list� funkcji na okre�lonej funkcji" << std::endl;
	szHelpStream << ">> {green}@cls{*} - wyczy�� ekran" << std::endl;
	szHelpStream << std::endl;

	WriteFormatted(szHelpStream.str());
	ResetColor();
}

// Funkcja wy�wietla wyra�enie z podkre�lonymi i wypisanymi b��dami
//-----------------------------------------------------------------------------------------------------------------------------------------------
// szExp - wyra�enie z b��dem
// CalcError - b��dy do wy�wietlenia
//-----------------------------------------------------------------------------------------------------------------------------------------------
void ShowErrors(std::string szExp, const std::string & szSaveVar, CalcError oErrors)
{
	size_t	uiErrBegIdx = 0U;
	size_t	uiErrEndIdx = 0U;
	size_t	uiErrNum = oErrors.getNum();

	 // Na wszelki wypadek posortuj b��dy w rosn�cej kolejno�ci
	oErrors.sortBegIdx();

	 // Wy�wietl wyra�enie z pod�wietlonym b��dem
	TextAttr(FG_RED);
	std::cout << ">> ";

	// Je�li podano zmienn� do zapisu, wypisz j�
	if (!szSaveVar.empty())
	{
		TextAttr(FG_GREY);
		std::cout << szSaveVar << " <- ";
	}

	for (size_t i = 0U; i < uiErrNum; i++)
	{
		 // Je�li b��d o nieokre�lonym po�o�eniu, pomi�
		uiErrBegIdx = oErrors[i].uiBegIdx;
		if (oErrors[i].uiBegIdx == std::string::npos)
			continue;

		if (uiErrBegIdx == szExp.length())
			szExp += " ";

		TextAttr(FG_DARK_GREY);
		std::cout << szExp.substr(uiErrEndIdx, uiErrBegIdx - uiErrEndIdx);

		uiErrEndIdx = oErrors[i].uiEndIdx;
		TextAttr(BG_RED | FG_BLACK);
		std::cout << szExp.substr(uiErrBegIdx, ++uiErrEndIdx - uiErrBegIdx);
	}

	if (uiErrEndIdx < szExp.length())
	{
		TextAttr(FG_DARK_GREY);
		std::cout << szExp.substr(uiErrEndIdx);
	}

	 // Wy�wietl tre�� b��d - liniowo, je�li jeden, a w nast�pnych linijkach, je�li wi�cej
	if (uiErrNum == 1)
	{
		TextAttr(FG_RED);
		std::cout << "   <- " << _pl(oErrors[0].szMsg) << "  (#" << CalcError::szName[oErrors[0].eType] << ")" << std::endl << std::endl;
	}
	else
	{
		TextAttr(FG_RED);
		for (size_t i = 0U; i < uiErrNum; i++)
			std::cout << std::endl << "  (" << i + 1 << ") " << _pl(oErrors[i].szMsg) << "  (#" << CalcError::szName[oErrors[i].eType] << ")";
		std::cout << std::endl << std::endl;
	}

	ResetColor();
}

// Funkcja wy�wietla wynik
//-----------------------------------------------------------------------------------------------------------------------------------------------
// szExp - wyra�enie z b��dem
// CalcError - b��dy do wy�wietlenia
//-----------------------------------------------------------------------------------------------------------------------------------------------
void ShowResult(const std::string & szExp, const std::string & szSaveVar, double dResult)
{
	TextAttr(FG_GREY);
	std::cout << ">> ";

	// Je�li podano zmienn� do zapisania wyniku, wypisz j�
	if (!szSaveVar.empty())
	{
		TextAttr(FG_GREEN);
		std::cout << szSaveVar << " <- ";
	}

	// Wy�wietl, koloruj�c nawiasy i operatory
	unsigned short usBracketLevel = 0;
	for (const char ch : szExp)
	{
		// Zresetuj kolor
		TextAttr(FG_GREY);

		switch (ch)
		{
		// Nawias otwieraj�cy
		case '(':
			// Ustaw odpowiedni kolor nawiasu - je�li wci�cie przekroczy�o liczb� dost�pnych kolor�w, wybierz ostatni
			if (usBracketLevel < 14)
				TextAttr(usBracketColorArr[usBracketLevel]);
			else
				TextAttr(usBracketColorArr[13]);

			// Zwi�ksz wci�cie
			usBracketLevel++;
			break;

		// Nawias zamykaj�cy
		case ')':
			// Zmniejsz wci�cie
			usBracketLevel--;

			// Ustaw odpowiedni kolor nawiasu - je�li wci�cie przekroczy�o liczb� dost�pnych kolor�w, wybierz ostatni
			if (usBracketLevel < BRACKET_COLORS_NUM)
				TextAttr(usBracketColorArr[usBracketLevel]);
			else
				TextAttr(usBracketColorArr[BRACKET_COLORS_NUM - 1]);

			break;

		// Operatory
		/*case '+':
		case '-':
		case '*':
		case '/':
		case '%':
		case '^':
		case '!':
			TextAttr(FG_DARK_CYAN);
			break;*/

		// �rednik w funkcji
		case ';':
			// Ustaw odpowiedni kolor przecinka - taki, jak nawiasy funkcji
			if (usBracketLevel < BRACKET_COLORS_NUM + 1)
				TextAttr(usBracketColorArr[usBracketLevel - 1]);
			else
				TextAttr(usBracketColorArr[BRACKET_COLORS_NUM - 1]);

			break;
		}

		// Wypisz znak
		std::cout << ch;
	}

	TextAttr(FG_YELLOW);
	std::cout << " = " << FormatedNumber(dResult, ucPrecision, eNotation) << std::endl;

	// Zapisz wynik w podanej zmiennej (je�li podano)
	if (!szSaveVar.empty())
	{
		size_t uiIdx = gVarEngine.getIdx(szSaveVar);
		
		// Zmienna nie istnieje, utw�rz i zapisz
		if (uiIdx == VarEngine::nidx)
		{
			// Je�li zmienna jest za d�uga, powiadom
			if (szSaveVar.length() > _MAX_VAR_LENGTH)
			{
				TextAttr(FG_RED);
				WriteFormatted(">> Nazwa zmiennej {yellow}" + szSaveVar + "{*} jest za d�uga (" + std::to_string(szSaveVar.length()) + " " +
					pkutl::declension(szSaveVar.length(), "znak", "znaki", "znak�w") + ", max " STRFY(_MAX_VAR_LENGTH) ")");
			}
			else
			{
				gVarEngine.add(szSaveVar, dResult, "zmienna u�ytkownika", VarEngine::NONCONST_VAR);
				TextAttr(FG_DARK_GREEN);
				WriteFormatted(">> Utworzono zmienn� {yellow}" + szSaveVar + "{*} i przypisano jej powy�szy wynik");
			}
		}
		else
		{
			switch (gVarEngine[uiIdx].type())
			{
			// Zmienna jest sta��, nie mo�na nadpisa�
			case VarEngine::CONST_VAR:
				TextAttr(FG_RED);
				WriteFormatted(">> Nie mo�na nadpisa� sta�ej {yellow}" + szSaveVar + "{*}!");
				break;

			// Zmienna jest flag�, nie mo�na nadpisa�
			case VarEngine::FLAG_VAR:
				TextAttr(FG_RED);
				WriteFormatted(">> Symbol {yellow}" + szSaveVar + "{*} jest zarezerwowany dla flagi funkcji!");
				break;

			// Zmienna nie jest sta�a, mo�na nadpisa�
			case VarEngine::NONCONST_VAR:
				
				// Je�li zmienna to ans, wy�wietl monit o bezsensowno�ci
				if (szSaveVar == "ans")
				{
					TextAttr(FG_DARK_GREEN);
					WriteFormatted(">> Zapisanie ostatniego wyniku do {yellow}ans{*} jest automatyczne");
				}
				else
				{
					TextAttr(FG_DARK_GREEN);
					gVarEngine[uiIdx].changeValue(dResult);
					WriteFormatted(">> Nadpisano zmienn� {yellow}" + szSaveVar + "{*} powy�szym wynikiem");
				}
				break;

			default:
				throw std::runtime_error("ShowResult: niepoprawny typ nadpisywanej zmiennej");
			}
		}

		std::cout << std::endl;
	}
	std::cout << std::endl;
	ResetColor();
}

// Funkcja pokazuje list� zmiennych na okre�lonej parametrem stronie
//-------------------------------------------------------------------
// ucPage - strona do wy�wietlenia
// ucMaxPage - maksymalny numer strony
//-------------------------------------------------------------------
void ShowCList(unsigned short ucPage, unsigned short ucMaxPage)
{
	// Narysuj belk� z etykietami
	TextAttr(FG_GREEN | BG_DARK_GREEN);
	GotoXY(_VAR_LIST_X_OFFSET, WhereY());
	std::cout << "     [NAZWA]     ";
	GotoXY(_VAR_LIST_X_OFFSET + 18, WhereY());
	std::cout << _pl("      [WARTO��]       ");
	GotoXY(_VAR_LIST_X_OFFSET + 41, WhereY());
	std::cout << "                        [OPIS]                        ";

	// Zmierz g�rn� belk�
	SHORT uiLowerBarSize = WhereX() - _VAR_LIST_X_OFFSET;
	std::cout << std::endl;

	// Wyczy�� miejsce pod zmienne
	COORD stcSize = GetBufferSize();
	FillRect(' ', FG_GREY, 0, WhereY(), stcSize.X, _VARS_PER_PAGE);

	// Zabezpiecz na wzselki wypadek przed z�� stron�
	if (ucPage == 0 || ucPage > ucMaxPage)
	{
		GotoXY(_VAR_LIST_X_OFFSET, WhereY());
		TextAttr(FG_RED);
		std::cout << "Niepoprawna strona";
		for (size_t i = 0; i < _VARS_PER_PAGE; i++)
			std::cout << std::endl;
	}
	else
	{
		// Wy�wietl zmienne
		size_t uiNumDisplayed = 0;

		for (size_t i = (ucPage - 1) * _VARS_PER_PAGE; uiNumDisplayed < _VARS_PER_PAGE && i < gVarEngine.numOf(); i++)
		{
			if (gVarEngine[i].type() == VarEngine::FLAG_VAR)
				continue;

			if (gVarEngine[i].type() == VarEngine::CONST_VAR)
				TextAttr(FG_GREEN);
			else
				TextAttr(FG_YELLOW);

			GotoXY(_VAR_LIST_X_OFFSET, WhereY());
			std::cout << gVarEngine[i].name();

			GotoXY(_VAR_LIST_X_OFFSET + 18, WhereY());
			TextAttr(FG_GREY);
			std::cout << FormatedNumber(gVarEngine[i].value(), 15, NOT_DEFAULT);

			GotoXY(_VAR_LIST_X_OFFSET + 41, WhereY());
			if (gVarEngine[i].type() == VarEngine::CONST_VAR)
				TextAttr(FG_DARK_GREEN);
			else
				TextAttr(FG_DARK_YELLOW);
			std::cout << _pl(gVarEngine[i].desc()) << std::endl;

			uiNumDisplayed++;
		}

		// Je�li nie wype�niono ca�ej list, dope�nij enterami
		while (uiNumDisplayed++ < _VARS_PER_PAGE)
			std::cout << std::endl;
	}

	// Wy�wietl doln� belk� z numerem strony
	FillRect(' ', BG_DARK_GREEN, _VAR_LIST_X_OFFSET, WhereY(), uiLowerBarSize, 1);

	// "^ v - nawigacja"
	GotoXY(_VAR_LIST_X_OFFSET + 1, WhereY());
	WriteFormatted("{black|green}   {green|dark_green} {black|green}   {green|dark_green} - nawigacja");
	PutXY(30, ucPage == 1 ? FG_DARK_GREEN | BG_GREEN : FG_BLACK | BG_GREEN, _VAR_LIST_X_OFFSET + 2, WhereY());
	PutXY(31, ucPage == ucMaxPage ? FG_DARK_GREEN | BG_GREEN : FG_BLACK | BG_GREEN, _VAR_LIST_X_OFFSET + 6, WhereY());
	
	 // "Strona x/x"
	TextAttr(FG_GREEN | BG_DARK_GREEN);
	Center(WhereY(), "Strona " + to_string(ucPage) + "/" + to_string(ucMaxPage), _VAR_LIST_X_OFFSET, uiLowerBarSize);

	 // "ESC - zamknij list�"
	GotoXY(uiLowerBarSize - 22 + _VAR_LIST_X_OFFSET, WhereY());
	WriteFormatted("{black|green} ESC {green|dark_green} - zamknij list�");
	std::cout << std::endl << std::endl;
	ResetColor();
}

// Funkcja pokazuje list� funkcji z opisem wybranej
//-------------------------------------------------------------------
// usPos - pozycja, od kt�rej si� zaczyna lista funkcji
// usCurrPos - pozycja kursora (zaznaczonej funkcji) z wy�wietlonym info
//-------------------------------------------------------------------
void ShowFList(unsigned short usPos, unsigned short usCurrPos)
{
	std::string	szToShow;			// Sformatowany fragment tabelki do wy�wietlenia
	std::string	szFunctInfo;		// Dolna informacja o funkcji
	COORD		sctSize = GetBufferSize(); // Obecny rozmiar bufora
	std::string	szSavedParamInfo;	// Zapisana lista parametr�w z pod�wietlonej funkcji (pod kursorem)
	SHORT		sFunctInfoBeg;		// Pozycja pocz�tku (linijki nad) opisu funkcji

	//-------------------------------------------------------------------------------------
	// BELKA Z ETYKIETAMI
	//-------------------------------------------------------------------------------------

	TextAttr(FG_GREEN | BG_DARK_GREEN);
	GotoXY(_FUNCT_LIST_X_OFFSET, WhereY());
	std::cout << "     [NAZWA]     ";
	GotoXY(_FUNCT_LIST_X_OFFSET + 18, WhereY());
	std::cout << _pl("         [PARAMETRY]          ");
	GotoXY(_FUNCT_LIST_X_OFFSET + 49, WhereY());
	std::cout << "                    [OPIS]                    ";

	// Zmierz g�rn� belk�
	SHORT uiLowerBarSize = WhereX() - _FUNCT_LIST_X_OFFSET;
	std::cout << std::endl;

	// Zabezpiecz na wzselki wypadek przed z�ymi indeksami
	if (usPos > gFunctEngine.numOf() - _FUNCT_PER_PAGE)
		usPos = (unsigned short)(gFunctEngine.numOf() - _FUNCT_PER_PAGE);
	if (usCurrPos < usPos)
		usCurrPos = usPos;
	else if (usCurrPos >= usPos + _FUNCT_PER_PAGE)
		usCurrPos = usPos + _FUNCT_PER_PAGE - 1;

	//-------------------------------------------------------------------------------------
	// LISTA FUNKCJI
	//-------------------------------------------------------------------------------------

	size_t		uiNumParams;
	for (size_t i = usPos; i < usPos + _FUNCT_PER_PAGE; i++)
	{
		// Je�eli to pozycja kursora, pod�wietl
		if (i == usCurrPos)
			TextAttr(FG_GREY | BG_DARK_BLUE);
		else
			TextAttr(FG_GREY | BG_BLACK);

		// Nazwa
		GotoXY(_FUNCT_LIST_X_OFFSET, WhereY());
		szToShow = "{green}" + gFunctEngine[i].szName;
		FitFormattedToSize(szToShow, 18);
		WriteFormatted(szToShow);

		// Lista parametr�w
		GotoXY(_FUNCT_LIST_X_OFFSET + 18, WhereY());
		uiNumParams = gFunctEngine[i].numOfParams();

		if (uiNumParams == 0)
			szToShow = "{yellow}brak parametr�w";
		else
		{
			szToShow = "{yellow}";
			for (size_t param = 0; param < uiNumParams - 1; param++)
				szToShow += gFunctEngine[i][param].szName + "{white}; {yellow}";
			szToShow += gFunctEngine[i][uiNumParams - 1].szName;
		}

		// Je�li to pozycja pod kursorem, zapisz list� parametr�w do wy�wietlenia pod spodem
		if (usCurrPos == i)
			szSavedParamInfo = szToShow;

		FitFormattedToSize(szToShow, 49 - 18);
		WriteFormatted(szToShow);

		// Opis funkcji
		GotoXY(_FUNCT_LIST_X_OFFSET + 49, WhereY());
		szToShow = "{dark_green}" + gFunctEngine[i].szDesc;
		FitFormattedToSize(szToShow, uiLowerBarSize - 49);
		szToShow += "\r\n";
		WriteFormatted(szToShow);
	}

	//-------------------------------------------------------------------------------------
	// DOLNA BELKA Z NAWIGACJ�
	//-------------------------------------------------------------------------------------

	FillRect(' ', BG_DARK_GREEN, _FUNCT_LIST_X_OFFSET, WhereY(), uiLowerBarSize, 1);

	// "^ v - nawigacja"
	GotoXY(_FUNCT_LIST_X_OFFSET + 1, WhereY());
	WriteFormatted("{black|green}   {green|dark_green} {black|green}   {green|dark_green} {black|green} home {green|dark_green} {black|green} end {green|dark_green} - nawigacja");
	PutXY(30, usCurrPos == 0 ? FG_DARK_GREEN | BG_GREEN : FG_BLACK | BG_GREEN, _FUNCT_LIST_X_OFFSET + 2, WhereY());
	PutXY(31, usCurrPos == gFunctEngine.numOf() - 1 ? FG_DARK_GREEN | BG_GREEN : FG_BLACK | BG_GREEN, _FUNCT_LIST_X_OFFSET + 6, WhereY());

	// "Strona x/x"
	TextAttr(FG_GREEN | BG_DARK_GREEN);
	Center(WhereY(), "Funkcja " + to_string(usCurrPos + 1) + "/" + to_string(gFunctEngine.numOf()), _FUNCT_LIST_X_OFFSET, uiLowerBarSize);

	// "ESC - zamknij list�"
	GotoXY(uiLowerBarSize - 22 + _FUNCT_LIST_X_OFFSET, WhereY());
	WriteFormatted("{black|green} ESC {green|dark_green} - zamknij list�\r\n");

	//-------------------------------------------------------------------------------------
	// DOLNE POLE Z INFORMACJAMI O FUNKCJI
	//-------------------------------------------------------------------------------------

	if (gFunctEngine[usCurrPos].numOfParams() != 0)
	{
		TextAttr(FG_WHITE | BG_DARK_GREEN);

		// Zapami�taj pocz�tek
		sFunctInfoBeg = WhereY();

		// Nazwa funkcji z parametrami
		szToShow = " Funkcja {cyan}" + gFunctEngine[usCurrPos].szName + "{*}(" + szSavedParamInfo + "{*}):";
		FitFormattedToSize(szToShow, uiLowerBarSize - 2);
		GotoXY(_FUNCT_LIST_X_OFFSET + 1, WhereY() + 1);  // + 1, bo kursor wci�� jest nad wype�nionym prostok�tem
		WriteFormatted(szToShow);
		
		// Pusta linijka mi�dzy funkcj� i parametrami
		GotoXY(_FUNCT_LIST_X_OFFSET + 1, WhereY() + 1);
		FillRect(' ', FG_WHITE | BG_DARK_GREEN, WhereX(), WhereY(), uiLowerBarSize - 2, 1);

		// Parametry funkcji
		for (size_t i = 0; i < gFunctEngine[usCurrPos].numOfParams(); i++)
		{
			szToShow = " {yellow}" + gFunctEngine[usCurrPos][i].szName;
			FitFormattedToSize(szToShow, 13);
			szToShow += "{*}" + gFunctEngine[usCurrPos][i].szDesc;
			
			switch (gFunctEngine[usCurrPos][i].eType)
			{
			// Je�li to Param::Value z warto�ci� domy�ln�, opisz
			case FunctEngine::Param::VALUE:
				if (gFunctEngine[usCurrPos][i].uParam.value.bHasDefault)
					szToShow += " {cyan}(domy�lna warto��: " + FormatedNumber(gFunctEngine[usCurrPos][i].uParam.value.dDefaultVal, ucPrecision, eNotation) + ")";
				break;

			// Je�li to flaga, wy�wietl wszystkie dost�pne
			case FunctEngine::Param::FLAG:
				for (auto const & flag : gFunctEngine[usCurrPos][i].uParam.flag.uiFlagIdxSet)
				{
					FitFormattedToSize(szToShow, uiLowerBarSize - 2);
					GotoXY(_FUNCT_LIST_X_OFFSET + 1, WhereY() + 1);
					WriteFormatted(szToShow);

					szToShow = "    {green}" + gVarEngine[flag].name();
					FitFormattedToSize(szToShow, 13);
					szToShow += "{grey}" + gVarEngine[flag].desc();

					// Je�li to flaga domy�lna, opisz
					if (gFunctEngine[usCurrPos][i].uParam.flag.bHasDefault && gFunctEngine[usCurrPos][i].uParam.flag.uiDefaultIdx == flag)
						szToShow += " {cyan}(domy�lnie)";
				}
				break;

			// Je�li to zmienna lista argument�w, wy�wietl dolny i g�rny limit
			case FunctEngine::Param::VA_LIST:
				if (gFunctEngine[usCurrPos][i].uParam.va_list.hasMin())
				{
					if (gFunctEngine[usCurrPos][i].uParam.va_list.hasMax())
						szToShow += " {cyan}(od " + std::to_string(gFunctEngine[usCurrPos][i].uParam.va_list.usMinParams) + " do " + std::to_string(gFunctEngine[usCurrPos][i].uParam.va_list.usMaxParams) + " warto�ci)";
					else
						szToShow += " {cyan}(przynajmniej " + std::to_string(gFunctEngine[usCurrPos][i].uParam.va_list.usMinParams) + " warto�ci)";

				}
				else if (gFunctEngine[usCurrPos][i].uParam.va_list.hasMax())
					szToShow += " {cyan}(do " + std::to_string(gFunctEngine[usCurrPos][i].uParam.va_list.usMaxParams) + " warto�ci)";
			}

			FitFormattedToSize(szToShow, uiLowerBarSize - 2);
			GotoXY(_FUNCT_LIST_X_OFFSET + 1, WhereY() + 1);
			WriteFormatted(szToShow);
		}

		// Wrysuj wok� ramk�
		Frame(FRAME_DOUBLE, FG_GREEN | BG_DARK_GREEN, _FUNCT_LIST_X_OFFSET, sFunctInfoBeg, uiLowerBarSize, WhereY() + 2 - sFunctInfoBeg);
	}

	GotoXY(0, WhereY() + 2);
	ResetColor();
	Clrscr(0, WhereY());
	std::cout << std::endl;
}

// Funkcja obs�uguje cia�o aplikacji
//-------------------------------------------------------------------
void ProceedMain()
{
	std::string		szExp;			// Napisane wyra�enie
	std::string		szSaveVar;		// Zmienna do zapisania wyra�enia
	COORD			sctCurrExpCur = { 0, 0 };			// Pozycja pocz�tku ostatniego wyra�enia

	CalcParser		oParser;		// Obiekt wykonuj�cy wyra�enie
	CalcTree *		opExpTree;		// Drzewo wyra�enia
	double			dResult;		// Wynik

	while (true)
	{
		// Zapytaj o wyra�enie i je pobierz - usu� bia�e znaki z pocz�tku i ko�ca wyra�enia
		sctCurrExpCur.X = WhereX();
		sctCurrExpCur.Y = WhereY();

		"linijka bez znaczenia. Na choler� tu jest"; 1726;
		/*for (int i = 10; i <= 250; i++)
		{
		std::cout << "0.67e-" << i << " - 0.31e-" << i << " - 0.01e-" << i << " - 0.02e-" << i << " - 0.03e-" << i << " - 0.15e-" << i << " - 0.05e-" << i << " - 0.06e-" << i << " + 0.96e-" << i << " - 1e-" << i << "" << std::endl;
		}*/

		std::cout << ">> ";
		getline(cin, szExp);
		pkutl::trim_ws_ref(szExp);

		if (szExp.empty())
		{
			Clrscr(sctCurrExpCur.X, sctCurrExpCur.Y);
			continue;
		}

#ifdef _IGNORE_LETTER_CASE
		transform(szExp.begin(), szExp.end(), szExp.begin(), tolower);//ptr_fun <int, int>(tolower));
#endif

		// Obs�u� komendy
		vector<std::string> szCmdVector;
		if (FetchCmd(szExp, szCmdVector))
		{
			if (szCmdVector[0] == "@exit" || szCmdVector[0] == "@q")
				return;
			else
				ProceedCmd(szCmdVector);
		}
		// Obs�u� kalkulator
		else
		{
			// Je�li wyra�enie ma by� zapisane do podanej zmiennej, wytnij j�
			szSaveVar = ParseVar(szExp);

			// Uzupe�nij wyra�enie o "ans" na pocz�tku, je�li trzeba i obetnij "=" z ko�ca, je�li podano
			FormatExp(szExp);

			// Sparsuj wyra�enie i ew. wy�wietl b��dy parsingu
			oParser.insertExp(szExp);
			opExpTree = nullptr;

			try {
				opExpTree = oParser.parse();
			}
			catch (CalcError & errors) {
				Clrscr(sctCurrExpCur.X, sctCurrExpCur.Y);
				ShowErrors(oParser.getExp(), szSaveVar, errors);
				continue;
			}

			// Oblicz wyra�enie i ew. wy�wietl b��dy interpretacji
			szExp = opExpTree->print();

			try {
				dResult = opExpTree->count();
			}
			catch (CalcError & errors) {
				Clrscr(sctCurrExpCur.X, sctCurrExpCur.Y);
				ShowErrors(szExp, szSaveVar, errors);
				continue;
			}

			// Je�li wszstko jest ok, wy�wietl wynik, zapisz do zmiennej ans i usu� drzewo wyra�enia
			Clrscr(sctCurrExpCur.X, sctCurrExpCur.Y);
			ShowResult(szExp, szSaveVar, dResult);
			gVarEngine["ans"].changeValue(dResult);
			delete opExpTree;
		}
	}
}

// Funkcja obs�uguje komendy
//-------------------------------------------------------------------
// Spodziewana zawarto�� szCmdVector:
// [0] - @komenda
// [1], [2], ... - parametry
//-------------------------------------------------------------------
void ProceedCmd(std::vector<std::string> szCmdVector)
{
	// Ustaw do b��du - ich jest wi�cej ;)
	TextAttr(FG_RED);

	// Lista komend
	if (szCmdVector[0] == "@help")
	{
		std::cout << std::endl;
		ShowHelp();
	}
	// Ustawienie precyzji wy�wietlania
	else if (szCmdVector[0] == "@prec")
	{
		unsigned char ucPrec;
		bool bExc = false;
		if (szCmdVector.size() != 2)
			std::cout << _pl(">> Z�a struktura: @prec [l. cyfr znacz�cych]") << std::endl << std::endl;
		else
		{
			// Pr�buj konwertowa� na liczb�
			try { ucPrec = stoi(szCmdVector[1]); }
			catch (...) { bExc = true; }

			if (bExc || ucPrec < _MIN_PREC || ucPrec > _MAX_PREC)
				std::cout << _pl(">> Podaj liczb� cyfr znacz�cych od ") << _MIN_PREC << " do " << _MAX_PREC << std::endl << std::endl;
			else
			{
				ucPrecision = ucPrec;
				TextAttr(FG_DARK_GREEN);
				std::cout << _pl(">> Ustawiono precyzj� na ") << (int)ucPrec << _pl(" cyfr znacz�cych") << std::endl << std::endl;
			}
		}
	}
	// Ustawianie notacji
	else if (szCmdVector[0] == "@not")
	{
		if (szCmdVector.size() != 2)
			std::cout << _pl(">> Z�a struktura: @not (fixed|scientific|default)") << std::endl << std::endl;
		else if (szCmdVector[1] == "fixed")
		{
			TextAttr(FG_DARK_GREEN);
			eNotation = NOT_FIXED;
			std::cout << _pl(">> Wymuszono wy�wietlanie w notacji zwyk�ej") << std::endl << std::endl;
		}
		else if (szCmdVector[1] == "scientific")
		{
			TextAttr(FG_DARK_GREEN);
			eNotation = NOT_SCIENTIFIC;
			std::cout << _pl(">> Wymuszono wy�wietlanie w notacji naukowej") << std::endl << std::endl;
		}
		else if (szCmdVector[1] == "default")
		{
			TextAttr(FG_DARK_GREEN);
			eNotation = NOT_DEFAULT;
			std::cout << _pl(">> Przywr�cono domy�ln� notacj� (mieszana)") << std::endl << std::endl;
		}
		else
		{
			std::cout << _pl(">> Z�a struktura: @not (fixed|scientific|default)") << std::endl << std::endl;
		}
	}
	else if (szCmdVector[0] == "@dlugosc")
	{
		if (szCmdVector.size() != 2)
			std::cout << _pl(">> Z�a struktura: @dlugosc [przedmiot]") << std::endl << std::endl;
		else if (szCmdVector[1] == "pala_swiatka")
			std::cout << _pl(">> Spr�buj cos d�u�szego") << std::endl << std::endl;
		else
			std::cout << ">> Nieznana" << std::endl << std::endl;
	}
	else if (szCmdVector[0] == "@ssij")
	{
		TextAttr(FG_DARK_CYAN);
		WriteFormatted("Mam ci� g��boko w {cyan}dupie{*}. Jeste� wart tyle co {cyan}cos(pi / 2){*}. We� id� si� zabij. {yellow}Ja wychodz�, pa.\n");
		while (!KbHit()) {}
		exit(0);
	}
	else if (szCmdVector[0] == "@clist")
	{
		unsigned short ucPage;

		// Oblicz maksymalny numer strony
		unsigned short ucMaxPage = pkutl::count_max_page((unsigned short)gVarEngine.numOfNonFlags(), _VARS_PER_PAGE);

		if (szCmdVector.size() > 2)
			std::cout << _pl(">> Z�a struktura: @clist [nr strony](opcjonalnie)") << std::endl << std::endl;
		else if (szCmdVector.size() == 2 && !pkutl::is_int(szCmdVector[1]))
			std::cout << _pl(">> Podano niepoprawn� liczb� \"" + szCmdVector[1] + "\"") << std::endl << std::endl;
		else if ((ucPage = (szCmdVector.size() == 1) ? 1 : stoi(szCmdVector[1])) == 0 || ucPage > ucMaxPage)
			std::cout << _pl(">> Podaj numer strony od 1 do ") << ucMaxPage << std::endl << std::endl;
		else
		{
			std::cout << std::endl;
			ProceedCList(ucPage, ucMaxPage);
		}
	}
	else if (szCmdVector[0] == "@flist")
	{
		unsigned short usPos;

		if (szCmdVector.size() > 2)
			std::cout << _pl(">> Z�a struktura: @flist [nr pocz�tkowej pozycji kursora](domy�lnie 0) lub @flist [nazwa funkcji]") << std::endl << std::endl;
		else if (szCmdVector.size() == 2 && !pkutl::is_int(szCmdVector[1]))
		{
			size_t uiIdx = gFunctEngine.getIdx(szCmdVector[1]);
			if (uiIdx == FunctEngine::nidx)
				std::cout << _pl(">> Funkcja \"" + szCmdVector[1] + "\" nie istnieje") << std::endl << std::endl;
			else
			{
				std::cout << std::endl;
				ProceedFList((unsigned short)uiIdx);
			}
		}
		else if ((usPos = (szCmdVector.size() == 1) ? 0 : stoi(szCmdVector[1])) >= gFunctEngine.numOf())
			std::cout << _pl(">> Podaj indeks funkcji od 0 do ") << (gFunctEngine.numOf() - 1) << std::endl << std::endl;
		else
		{
			std::cout << std::endl;
			ProceedFList(usPos);
		}
	}
	// Czyszczenie ekranu
	else if (szCmdVector[0] == "@cls")
	{
		Clrscr();
		ShowStartInfo();
	}
	// Nieznana komenda
	else
	{
		std::cout << ">> Nieznana komenda" << std::endl << std::endl;
	}

	ResetColor();
}

// Funkcja pokazuje list� zmiennych na okre�lonej parametrem stronie
// i obs�uguj� nawigacj� po stronach za pomoc� strza�ek
//-------------------------------------------------------------------
// ucPage - strona do wy�wietlenia
// ucMaxPage - maksymalny numer strony
//-------------------------------------------------------------------
void ProceedCList(unsigned short ucPage, unsigned short ucMaxPage)
{
	SHORT sY = WhereY();
	int ch;

	CursorOff();
	ShowCList(ucPage, ucMaxPage);

	// Wy�wietlamy dot�d, a� naci�ni�to ESC
	while ((ch = GetCh()) != VK_ESCAPE)
	{
		if (!ch)
			continue;

		switch (ch)
		{
			// Nawigacja w lewo
		case NC_KEY_LEFT:
		case NC_KEY_UP:
			// Je�li nie jeste�my na pierwszej stronie, wy�wietl poprzedni�
			if (ucPage > 1)
			{
				ucPage--;
				GotoXY(0, sY);
				ShowCList(ucPage, ucMaxPage);
			}
			break;

			// Nawigacja w prawo
		case NC_KEY_RIGHT:
		case NC_KEY_DOWN:
			// Je�li nie jeste�my na ostatniej stronie, wy�wietl poprzedni�
			if (ucPage < ucMaxPage)
			{
				ucPage++;
				GotoXY(0, sY);
				ShowCList(ucPage, ucMaxPage);
			}
			break;
		}
	}

	CursorOn();
}

// Funkcja pokazuje list� funkcji na okre�lonej i obs�uguj� nawigacj�
// za pomoc� strza�ek
//-------------------------------------------------------------------
void ProceedFList(unsigned short usCurrPos)
{
	SHORT sY = WhereY();
	int ch;
	unsigned short usStartPos;

	CursorOff();
	
	// Oblicz pocz�tkow� pozycj� listy
	if (usCurrPos < _FUNCT_SCROLL_OFFSET)
		usStartPos = 0;
	else
		usStartPos = usCurrPos - _FUNCT_SCROLL_OFFSET;

	ShowFList(usStartPos, usCurrPos);
	//ScrollWindowTo(sY - 2);

	// Wy�wietlamy dot�d, a� naci�ni�to ESC
	while ((ch = GetCh()) != VK_ESCAPE)
	{
		if (!ch)
			continue;

		switch (ch)
		{
			// Nawigacja w g�r�
		case NC_KEY_UP:
			// Przesu� kursor, je�li mo�na
			if (usCurrPos > 0)
			{
				usCurrPos--;
				if (usCurrPos - usStartPos < _FUNCT_SCROLL_OFFSET && usStartPos > 0)
					usStartPos--;

				GotoXY(0, sY);
				ShowFList(usStartPos, usCurrPos);
				//ScrollWindowTo(sY - 2);
			}
			break;

			// Nawigacja w d�
		case NC_KEY_DOWN:
			// Je�li nie jeste�my na ostatniej stronie, wy�wietl poprzedni�
			if (usCurrPos < gFunctEngine.numOf() - 1)
			{
				usCurrPos++;
				if (usStartPos + _FUNCT_PER_PAGE - 1 - usCurrPos < _FUNCT_SCROLL_OFFSET && usStartPos + _FUNCT_PER_PAGE < gFunctEngine.numOf())
					usStartPos++;

				GotoXY(0, sY);
				ShowFList(usStartPos, usCurrPos);
				//ScrollWindowTo(sY - 2);
			}
			break;

			// Skocz na pocz�tek
		case NC_KEY_HOME:
			usCurrPos = 0;
			usStartPos = 0;

			GotoXY(0, sY);
			ShowFList(usStartPos, usCurrPos);
			//ScrollWindowTo(sY - 2);
			break;

			// Skocz na koniec
		case NC_KEY_END:
			usCurrPos = (unsigned short)(gFunctEngine.numOf() - 1);
			usStartPos = usCurrPos - _FUNCT_PER_PAGE + 1;

			GotoXY(0, sY);
			ShowFList(usStartPos, usCurrPos);
			//ScrollWindowTo(sY - 2);
			break;
		}
	}

	CursorOn();
}

// Funkcja sprawdza, czy wprowadzona linijka jest komend�. Je�li jest, zapisuje std::string�w postaci {"/cmd", "param1", "param2", ...} i zwraca true.
// Je�li nie, zwraca false
//-----------------------------------------------------------------------------------------------------------------------------------------------
// sInput - wyra�enie do sprawdzenie
// &sCmdVector - wektor std::string�w do zapisania
//-----------------------------------------------------------------------------------------------------------------------------------------------
bool FetchCmd(const std::string & sInput, vector<std::string> &sCmdVector)
{
	std::stringstream	sInputSplit(sInput);
	std::string			sItem;

	sCmdVector.clear();

	while (sInputSplit >> sItem)
		sCmdVector.push_back(sItem);

	if (sCmdVector.empty() || sCmdVector[0][0] != '@' || sCmdVector[0] == "@")
		return false;

	// Zamie� du�e znaki w komendzie na ma�e
	std::transform(sCmdVector[0].begin(), sCmdVector[0].end(), sCmdVector[0].begin(), tolower);

	return true;
}

// Funkcja sprawdza, czy podane wyra�enie jest w formacje zmienna <- wyra�enie
// Je�li tak, to zwraca [zmienna] i obcina wyra�enie do [wyra�enie].
// Je�li nie, zwraca "" i nie zmienie wyra�enia
//-------------------------------------------------------------------
std::string ParseVar(std::string & szExp)
{
	static std::regex oRegex("^([[:alpha:]][[:alnum:]_]*)\\s*(?:<-|<=|=|<|\\:)\\s*(.+)$");
	std::smatch	oMatch;
	std::string szVarSave;

	// Sprawd�, czy wyra�enie pasuje do wzorca
	if (std::regex_match(szExp, oMatch, oRegex))
	{
		if (oMatch.size() != 3)
			throw std::runtime_error("ParseVar: wewn�trzny b��d regex_match");

		szVarSave = oMatch.str(1);
		szExp = oMatch.str(2);
		return szVarSave;
	}

	return "";
}

// Funkcja resetuje kolor konsoli na domy�lny
//-------------------------------------------------------------------
void ResetColor()
{
	TextAttr (FG_GREY);
}

// Funkcja automatycznie uzupe�nia "ans" na pocz�tku wyra�enia,
// je�li rozpozna tak� intencj� u�ytkownika
//-------------------------------------------------------------------
void FormatExp(std::string & szExp)
{
	if (szExp.empty())
		return;

	 // Dodaj "ans" na pocz�tku, je�li zaczyna si� znakiem dzia�ania (nie - i nie +)
	switch (szExp[0])
	{
	case '*':
	case '/':
	case '^':
	case '%':
	case '!':
		szExp = "ans" + szExp;
	}
	 
	 // Usu� "=" z ko�ca, je�li podano
	if (szExp[szExp.length() - 1] == '=')
	{
		szExp.pop_back();
		pkutl::trim_ws_ref(szExp);
	}
}

// Funkcja generuje ci�g znak�w reprezentuj�cy liczb� w okre�lonym
// formacie
//-------------------------------------------------------------------
// dVal - warto�� do zaprezentowania
// ucPrecision - ilo�� cyfr znacz�cych
// eNotation - spos�b wy�wietlania
//-------------------------------------------------------------------
std::string FormatedNumber(double dVal, unsigned char ucPrecision, NOTATION eNotation)
{
	std::stringstream	szResultStream;
	std::string			szResult;

	switch (eNotation)
	{
	case NOT_SCIENTIFIC:
		szResultStream << std::scientific;

	case NOT_DEFAULT:
		szResultStream << std::setprecision(ucPrecision) << dVal;
		szResult = szResultStream.str();

		// Popraw wy�wietlanie -0
		if (dVal == 0 && szResult[0] == '-')
			return szResult.substr(1);
		else
			return szResult;

		break;

	case NOT_FIXED:
		return pkutl::format_fixed(dVal, ucPrecision, _MAX_ZEROS);
		break;

	default:
		throw std::invalid_argument("FormattedNumber: niepoprawny rodzaj notacji");
	}
}
