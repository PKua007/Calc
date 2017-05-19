// Zbi�r procedur obs�ugi konsoli znakowej w Windows
//--------------------------------------------------
//          (C)2006 mgr Jerzy Wa�aszek
//			edytowane 2016 PKua
//--------------------------------------------------

#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <string>
#include <sstream>
#include "newconio.h"
#include "pkua_utils.h"

using namespace std;

extern "C" {BOOL WINAPI SetConsoleDisplayMode(HANDLE, DWORD, PCOORD); }

static WORD   _txattr;
static HANDLE _hcout, _hcin;
static int    _cextend = -1;

COORD		  cbuffer_size;

//----------------------------------------------------------------------
// Poni�sza tablica odwzorowuje wirtualne kody klawiszy Windows na kody
// BIOS dla klawiatury IBM PC. Dla ka�dego wirtualnego kodu klawisza
// dost�pne s� cztery warto�ci BIOS: zwyk�a, z shift, z Ctrl i z Alt.
// Kombinacje klawiszy nie posiadaj�ce odpowiednika BIOS maj� warto��
// -1 i s� ignorowane. Rozszerzone klawisze (nie ze zbioru ASCII)
// maj� ustawiony na 1 bit 8 przy pomocy makra EXT.
//----------------------------------------------------------------------

#define EXT(key)    ((key)+0x100)
#define ISEXT(val)  ((val)&0x100)
#define EXTVAL(val) ((val)&0xff)

struct kbd
{
	short keycode; // kod wirtualny
	short normal;  // normalny kod BIOS
	short shift;   // kod BIOS z shift
	short ctrl;    // kod BIOS z Ctrl
	short alt;     // kod BIOS z Alt
} kbdtab[] =
{
	//  wirtualny     normalny   shift       ctrl         alt

	{ VK_BACK,      0x08,       0x08,       0x7f,       EXT(14) },
	{ VK_TAB,       0x09,       EXT(15),    EXT(148),   EXT(165) },
	{ VK_RETURN,    0x0d,       0x0d,       0x0a,       EXT(166) },
	{ VK_ESCAPE,    0x1b,       0x1b,       0x1b,       EXT(1) },
	{ VK_SPACE,     0x20,       0x20,       EXT(3),     0x20, },
	{ '0',          '0',        ')',        -1,         EXT(129) },
	{ '1',          '1',        '!',        -1,         EXT(120) },
	{ '2',          '2',        '@',        EXT(3),     EXT(121) },
	{ '3',          '3',        '#',        -1,         EXT(122) },
	{ '4',          '4',        '$',        -1,         EXT(123) },
	{ '5',          '5',        '%',        -1,         EXT(124) },
	{ '6',          '6',        '^',        0x1e,       EXT(125) },
	{ '7',          '7',        '&',        -1,         EXT(126) },
	{ '8',          '8',        '*',        -1,         EXT(127) },
	{ '9',          '9',        '(',        -1,         EXT(128) },

	{ 'A',          'a',        'A',        0x01,       EXT(30) },
	{ 'B',          'b',        'B',        0x02,       EXT(48) },
	{ 'C',          'c',        'C',        0x03,       EXT(46) },
	{ 'D',          'd',        'D',        0x04,       EXT(32) },
	{ 'E',          'e',        'E',        0x05,       EXT(18) },
	{ 'F',          'f',        'F',        0x06,       EXT(33) },
	{ 'G',          'g',        'G',        0x07,       EXT(34) },
	{ 'H',          'h',        'H',        0x08,       EXT(35) },
	{ 'I',          'i',        'I',        0x09,       EXT(23) },
	{ 'J',          'j',        'J',        0x0a,       EXT(36) },
	{ 'K',          'k',        'K',        0x0b,       EXT(37) },
	{ 'L',          'l',        'L',        0x0c,       EXT(38) },
	{ 'M',          'm',        'M',        0x0d,       EXT(50) },
	{ 'N',          'n',        'N',        0x0e,       EXT(49) },
	{ 'O',          'o',        'O',        0x0f,       EXT(24) },
	{ 'P',          'p',        'P',        0x10,       EXT(25) },
	{ 'Q',          'q',        'Q',        0x11,       EXT(16) },
	{ 'R',          'r',        'R',        0x12,       EXT(19) },
	{ 'S',          's',        'S',        0x13,       EXT(31) },
	{ 'T',          't',        'T',        0x14,       EXT(20) },
	{ 'U',          'u',        'U',        0x15,       EXT(22) },
	{ 'V',          'v',        'V',        0x16,       EXT(47) },
	{ 'W',          'w',        'W',        0x17,       EXT(17) },
	{ 'X',          'x',        'X',        0x18,       EXT(45) },
	{ 'Y',          'y',        'Y',        0x19,       EXT(21) },
	{ 'Z',          'z',        'Z',        0x1a,       EXT(44) },

	{ VK_PRIOR,     EXT(73),    EXT(73),    EXT(132),   EXT(153) },
	{ VK_NEXT,      EXT(81),    EXT(81),    EXT(118),   EXT(161) },
	{ VK_END,       EXT(79),    EXT(79),    EXT(117),   EXT(159) },
	{ VK_HOME,      EXT(71),    EXT(71),    EXT(119),   EXT(151) },
	{ VK_LEFT,      EXT(75),    EXT(75),    EXT(115),   EXT(155) },
	{ VK_UP,        EXT(72),    EXT(72),    EXT(141),   EXT(152) },
	{ VK_RIGHT,     EXT(77),    EXT(77),    EXT(116),   EXT(157) },
	{ VK_DOWN,      EXT(80),    EXT(80),    EXT(145),   EXT(160) },
	{ VK_INSERT,    EXT(82),    EXT(82),    EXT(146),   EXT(162) },
	{ VK_DELETE,    EXT(83),    EXT(83),    EXT(147),   EXT(163) },
	{ VK_NUMPAD0,   '0',        EXT(82),    EXT(146),   -1 },
	{ VK_NUMPAD1,   '1',        EXT(79),    EXT(117),   -1 },
	{ VK_NUMPAD2,   '2',        EXT(80),    EXT(145),   -1 },
	{ VK_NUMPAD3,   '3',        EXT(81),    EXT(118),   -1 },
	{ VK_NUMPAD4,   '4',        EXT(75),    EXT(115),   -1 },
	{ VK_NUMPAD5,   '5',        EXT(76),    EXT(143),   -1 },
	{ VK_NUMPAD6,   '6',        EXT(77),    EXT(116),   -1 },
	{ VK_NUMPAD7,   '7',        EXT(71),    EXT(119),   -1 },
	{ VK_NUMPAD8,   '8',        EXT(72),    EXT(141),   -1 },
	{ VK_NUMPAD9,   '9',        EXT(73),    EXT(132),   -1 },
	{ VK_MULTIPLY,  '*',        '*',        EXT(150),   EXT(55) },
	{ VK_ADD,       '+',        '+',        EXT(144),   EXT(78) },
	{ VK_SUBTRACT,  '-',        '-',        EXT(142),   EXT(74) },
	{ VK_DECIMAL,   '.',        '.',        EXT(83),    EXT(147) },
	{ VK_DIVIDE,    '/',        '/',        EXT(149),   EXT(164) },
	{ VK_F1,        EXT(59),    EXT(84),    EXT(94),    EXT(104) },
	{ VK_F2,        EXT(60),    EXT(85),    EXT(95),    EXT(105) },
	{ VK_F3,        EXT(61),    EXT(86),    EXT(96),    EXT(106) },
	{ VK_F4,        EXT(62),    EXT(87),    EXT(97),    EXT(107) },
	{ VK_F5,        EXT(63),    EXT(88),    EXT(98),    EXT(108) },
	{ VK_F6,        EXT(64),    EXT(89),    EXT(99),    EXT(109) },
	{ VK_F7,        EXT(65),    EXT(90),    EXT(100),   EXT(110) },
	{ VK_F8,        EXT(66),    EXT(91),    EXT(101),   EXT(111) },
	{ VK_F9,        EXT(67),    EXT(92),    EXT(102),   EXT(112) },
	{ VK_F10,       EXT(68),    EXT(93),    EXT(103),   EXT(113) },
	{ VK_F11,       EXT(133),   EXT(135),   EXT(137),   EXT(139) },
	{ VK_F12,       EXT(134),   EXT(136),   EXT(138),   EXT(140) },
	{ 0xdc,         '\\',       '|',        0x1c,       EXT(43) },
	{ 0xbf,         '/',        '?',        -1,         EXT(53) },
	{ 0xbd,         '-',        '_',        0x1f,       EXT(130) },
	{ 0xbb,         '=',        '+',        -1,         EXT(131) },
	{ 0xdb,         '[',        '{',        0x1b,       EXT(26) },
	{ 0xdd,         ']',        '}',        0x1d,       EXT(27) },
	{ 0xba,         ';',        ':',        -1,         EXT(39) },
	{ 0xde,         '\'',       '"',        -1,         EXT(40) },
	{ 0xbc,         ',',        '<',        -1,         EXT(51) },
	{ 0xbe,         '.',        '>',        -1,         EXT(52) },
	{ 0xc0,         '`',        '~',        -1,         EXT(41) },

	{ -1,           -1,         -1,         -1,         -1 }  // KONIEC
};

 // Tablica z kolorami
const char * szColorNames [] =
{
	"black",
	"dark_blue",
	"dark_green",
	"dark_cyan",
	"dark_red",
	"dark_magenta",
	"dark_yellow",
	"grey",
	"dark_grey",
	"blue",
	"green",
	"cyan",
	"red",
	"magenta",
	"yellow",
	"white"
};


// Funkcja inicjuje parametry konsoli - nale�y
// j� wywo�a� na samym pocz�tku programu
//--------------------------------------------

void _cinit(bool _fullscreen, bool forbid_resize)
{
	_hcout = GetStdHandle(STD_OUTPUT_HANDLE);
	_hcin = GetStdHandle(STD_INPUT_HANDLE);

	if (_fullscreen)
		Fullscreen(true);

	if (forbid_resize)
	{
		HWND hWindow = GetConsoleWindow();
		LONG lStyle = GetWindowLongA(hWindow, GWL_STYLE);
		pkutl::unset_flags(lStyle, WS_SIZEBOX | WS_MAXIMIZEBOX);
		SetWindowLongA(hWindow, GWL_STYLE, lStyle);
	}

	CONSOLE_SCREEN_BUFFER_INFO bufferinfo;
	GetConsoleScreenBufferInfo(_hcout, &bufferinfo);
	cbuffer_size.X = bufferinfo.dwSize.X;
	cbuffer_size.Y = bufferinfo.dwSize.Y;

	SetConsoleOutputCP(852);
	SetConsoleCP(852);
	_txattr = 7; // Czarne t�o i bia�e znaki
	Clrscr();
}

// Funkcja konwertuje tekst ze standardu Windows 1250
// na standard konsoli znakowej Latin II.
//---------------------------------------------------
// s - tekst kodowany wg Windows 1250
//---------------------------------------------------
string _pl(string s)
{
	char c;
	string x;
	int l = s.length();

	x = "";
	for (int i = 0; i < l; i++)
	{
		switch (s[i])
		{
		case '�': c = (char)165; break;
		case '�': c = (char)134; break;
		case '�': c = (char)169; break;
		case '�': c = (char)136; break;
		case '�': c = (char)228; break;
		case '�': c = (char)162; break;
		case '�': c = (char)152; break;
		case '�': c = (char)190; break;
		case '�': c = (char)171; break;
		case '�': c = (char)164; break;
		case '�': c = (char)143; break;
		case '�': c = (char)168; break;
		case '�': c = (char)157; break;
		case '�': c = (char)227; break;
		case '�': c = (char)224; break;
		case '�': c = (char)151; break;
		case '�': c = (char)189; break;
		case '�': c = (char)141; break;
		default: c = s[i]; break;
		}
		x += c;
	}
	return x;
}

// Funkcja wy�wietla tekst na �rodku obszaru
//------------------------------------------
// y - rz�d do wy�wietlenie
// s - ci�g znak�w do wy�wietlenia
// leftbound - wsp�rz�dna x lewego ko�ca
//    obszaru
// fieldwidth - szeroko�� pola; -1 - do ko�ca
//------------------------------------------
void Center(SHORT y, string s, SHORT leftbound, SHORT fieldwidth)
{
	if (leftbound < 0)	leftbound = 0;
	if (leftbound >= cbuffer_size.X)	return;
	if (fieldwidth == 32767 || leftbound + fieldwidth > cbuffer_size.X)		fieldwidth = cbuffer_size.X - leftbound;

	stringstream sStream(s);
	bool bFirst = true;
	while (getline(sStream, s))
	{
		if (s.length() > (size_t)fieldwidth)
			s.erase(fieldwidth);

		if (!bFirst)
			cout << endl;
		GotoXY((SHORT)(fieldwidth - s.length()) / 2 + leftbound, y++);

		cout << s;
		bFirst = false;
	}
}

// Funkcja wy�wietla tekst na �rodku obszaru.
// Mo�na go formatowa� jak w WriteFormatted.
// Formatowanie trzeba powt�rzy� po nowej
// linii
//------------------------------------------
// y - rz�d do wy�wietlenie
// s - ci�g znak�w do wy�wietlenia
// leftbound - wsp�rz�dna x lewego ko�ca
//    obszaru
// fieldwidth - szeroko�� pola; -1 - do ko�ca
//------------------------------------------
void CenterFormatted(SHORT y, string s, SHORT leftbound, SHORT fieldwidth)
{
	if (leftbound < 0)	leftbound = 0;
	if (leftbound >= cbuffer_size.X)	return;
	if (fieldwidth == 32767 || leftbound + fieldwidth > cbuffer_size.X)		fieldwidth = cbuffer_size.X - leftbound;

	stringstream sStream(s);
	string szErase;
	bool bFirst = true;
	while (getline(sStream, s))
	{
		if (!bFirst)
			cout << endl;

		GotoXY((SHORT)(fieldwidth - EraseFormatted(s).length()) / 2 + leftbound, y++);
		WriteFormatted(s);
		bFirst = false;
	}
}

// Funkcja czy�ci ekran konsoli
//--------------------------------------------------------------
// xb, yb - wsp�rz�dne punktu, od kt�rego ma zacz�� czyszczenie
//--------------------------------------------------------------
void Clrscr(SHORT xb, SHORT yb)
{
	CONSOLE_SCREEN_BUFFER_INFO cinf;
	if (!GetConsoleScreenBufferInfo(_hcout, &cinf)) return;

	COORD cpos = { xb, yb };
	if (xb < 0 || xb >= cinf.dwSize.X || yb < 0 || yb >= cinf.dwSize.Y)
		cpos = { xb, yb };
	
	DWORD cwrt;
	DWORD csize;

	csize = cinf.dwSize.X * (cinf.dwSize.Y - yb) - xb;
	if (!FillConsoleOutputCharacterA(_hcout, ' ', csize, cpos, &cwrt)) return;
	if (!FillConsoleOutputAttribute(_hcout, _txattr, csize, cpos, &cwrt)) return;
	SetConsoleCursorPosition(_hcout, cpos);
	SetConsoleTextAttribute(_hcout, _txattr);
}

// Funkcja czy�ci ca�y ekran konsoli
//--------------------------------------------------------------
void Clrscr()
{
	Clrscr(0, 0);
}

// Funkcja wy��cza kursor
//-----------------------
void CursorOff()
{
	CONSOLE_CURSOR_INFO ccinf;

	GetConsoleCursorInfo(_hcout, &ccinf);
	ccinf.bVisible = false;
	SetConsoleCursorInfo(_hcout, &ccinf);
}

// Funkcja w��cza kursor
//-----------------------
void CursorOn()
{
	CONSOLE_CURSOR_INFO ccinf;

	GetConsoleCursorInfo(_hcout, &ccinf);
	ccinf.bVisible = true;
	SetConsoleCursorInfo(_hcout, &ccinf);
}

// Funkcja wstrzymuje wykonanie na zadan�
// liczb� milisekund
//---------------------------------------
// msec - liczba milisekund
//---------------------------------------
void Delay(DWORD msec)
{
	DWORD t1 = GetTickCount();
	while ((GetTickCount() - t1) < msec);
}

// Funkcja usuwa z ci�gu znak�w znaczniki
// formatowania i zwraca rezultat
//---------------------------------------
// Por�wnaj z WriteFormatted
//---------------------------------------
std::string EraseFormatted(std::string text)
{
	// Je�eli pusty tekst, nic nie r�b
	if (text.empty())
		return std::string{ "" };

	size_t uiBracketBegIdx;
	size_t uiBracketEndIdx = 0U;
	size_t uiBarIdx;
	std::string szBracketString;
	std::string szFg, szBg;
	int sFgColor, sBgColor;

	// Powtarzaj dla ka�dego napotkanego koloru
	do
	{
		// Odnaleziono "{ ... }" - sprawd�, czy zawiera poprawny kolor
		if ((uiBracketEndIdx = text.find('}', uiBracketEndIdx + 1)) != std::string::npos && (uiBracketBegIdx = text.find_last_of('{', uiBracketEndIdx)) != std::string::npos)
		{
			// Wydziel zawarto�� klamer i sprawd� format
			szBracketString = pkutl::trim_ws(text.substr(uiBracketBegIdx + 1, uiBracketEndIdx - uiBracketBegIdx - 1));
			uiBarIdx = szBracketString.find('|');

			// Format {tekst}
			if (uiBarIdx == std::string::npos)
			{
				// Szukaj nazwy koloru i pobierz kod; je�li nie znaleziono, szukaj nast�pnego "{ ... }" - kolejny obieg p�tli
				sFgColor = szBracketString == "*" ? 0 : pkutl::array_find_idx(szColorNames, 16, szBracketString);
				if (sFgColor != -1)
				{
					// Usu� znacznik z tekstu
					text.erase(uiBracketBegIdx, uiBracketEndIdx - uiBracketBegIdx + 1);
					uiBracketEndIdx -= uiBracketEndIdx - uiBracketBegIdx + 1;
					if (uiBracketEndIdx + 1 == text.length())
						return text;
				}
			}
			// Format {tekst|t�o}
			else
			{
				szFg = pkutl::trim_ws(szBracketString.substr(0U, uiBarIdx));
				szBg = pkutl::trim_ws(szBracketString.substr(uiBarIdx + 1));
				sFgColor = (szFg.empty() || szFg == "*") ? 0 : pkutl::array_find_idx(szColorNames, 16, szFg);
				sBgColor = (szBg.empty() || szBg == "*") ? 0 : pkutl::array_find_idx(szColorNames, 16, szBg);

				if (sFgColor != -1 && sBgColor != -1)
				{
					text.erase(uiBracketBegIdx, uiBracketEndIdx - uiBracketBegIdx + 1);
					uiBracketEndIdx -= uiBracketEndIdx - uiBracketBegIdx + 1;
					if (uiBracketEndIdx + 1 == text.length())
						return text;
				}
			}
		}
		else
		{
			return text;
		}
	} while (true);
}

// Funkcja uog�lnia wszystkie funkcje
// wype�niaj�ce obszary
//----------------------------------------------
// attr - atrybut do wype�nienia obszaru
// ch - znak do wype�nienia obszaru
// xb,yb - wsp�rz�dne lewego g�rnego naro�nika
// width, height - rozmiar obszaru. Ujemne
//    warto�ci "odwracaj�" kierunek rysowania
// mode - tryb wype�niania - sam atrybut, sam
//    znak, atrybut i znak
//----------------------------------------------
void FillRectGeneral(CHAR ch, WORD attr, SHORT xb, SHORT yb, SHORT width, SHORT height, FILL_MODE mode)
{
	CONSOLE_SCREEN_BUFFER_INFO cinf;
	COORD pos;
	DWORD crd;

	if (!GetConsoleScreenBufferInfo(_hcout, &cinf))		return;
	if (!width || !height)								return;

	// "Odwrotny" kierunek rysowania
	if (width < 0)
	{
		xb = xb + width + 1;
		width = -width;
	}
	if (height < 0)
	{
		yb = yb + height + 1;
		height = -height;
	}

	// Obci�cie pozabuforowych obszar�w i zabezpieczenie przed wyl�dawaniem prawego g�rnego rogu poza buforem
	if (xb < 0)
	{
		width += xb;
		xb = 0;
	}
	else if (xb >= cinf.dwSize.X)		return;
	if (yb < 0)
	{
		height += yb;
		yb = 0;
	}
	else if (yb >= cinf.dwSize.Y)		return;

	if (xb + width > cinf.dwSize.X)
		width = cinf.dwSize.X - xb;
	else if (xb + width < 1)			return;
	if (yb + height > cinf.dwSize.Y)
		height = cinf.dwSize.Y - yb;
	else if (yb + height < 1)			return;

	pos.X = xb;
	pos.Y = yb;

	switch (mode)
	{
	case FILL_ATTR:
		for (int i = 0; i < height; i++)
		{
			FillConsoleOutputAttribute(_hcout, attr, width, pos, &crd);
			pos.Y++;
		}
		break;
	case FILL_CH:
		for (int i = 0; i < height; i++)
		{
			FillConsoleOutputCharacterA(_hcout, ch, width, pos, &crd);
			pos.Y++;
		}
		break;
	case FILL_CH_ATTR:
		for (int i = 0; i < height; i++)
		{
			FillConsoleOutputAttribute(_hcout, attr, width, pos, &crd);
			FillConsoleOutputCharacterA(_hcout, ch, width, pos, &crd);
			pos.Y++;
		}
		break;
	}
}



// Funkcja wype�nia zadany obszar okna konsoli
// podanym atrybutem. Pozycja kursora oraz znaki
// zawarte w obszarze nie s� zmieniane.
//----------------------------------------------
// attr - atrybut do wype�nienia obszaru
// xb,yb - wsp�rz�dne lewego g�rnego naro�nika
// width, height - rozmiar obszaru. Ujemne
//    warto�ci "odwracaj�" kierunek rysowania
//----------------------------------------------
void FillRectAttr(WORD attr, SHORT xb, SHORT yb, SHORT width, SHORT height)
{
	FillRectGeneral(0, attr, xb, yb, width, height, FILL_ATTR);
}

// Funkcja wype�nia zadany obszar okna konsoli
// podanym znakiem. Pozycja kursora oraz atrybuty
// zawarte w obszarze nie s� zmieniane.
//-----------------------------------------------
// ch - znak do wype�nienia obszaru
// xb,yb - wsp�rz�dne lewego g�rnego naro�nika
// width, height - rozmiar obszaru. Ujemne
//    warto�ci "odwracaj�" kierunek rysowania
//-----------------------------------------------
void FillRectCh(CHAR ch, SHORT xb, SHORT yb, SHORT width, SHORT height)
{
	FillRectGeneral(ch, 0, xb, yb, width, height, FILL_CH);
}

// Funkcja wype�nia zadany obszar okna konsoli
// podanym znakiem i atrybutem. Pozycja kursora
// nie jest zmieniana.
//----------------------------------------------
// c - znak do wype�nienia
// attr - atrybut do wype�nienia obszaru
// xb,yb - wsp�rz�dne lewego g�rnego naro�nika
// width, height - rozmiar obszaru. Ujemne
//    warto�ci "odwracaj�" kierunek rysowania
//----------------------------------------------
void FillRect(CHAR ch, WORD attr, SHORT xb, SHORT yb, SHORT width, SHORT height)
{
	FillRectGeneral(ch, attr, xb, yb, width, height, FILL_CH_ATTR);
}

// Funkcja rysuje prostok�tn� ramk�
// type - okre�la typ ramki. Dozwolone typy, to:
//        FRAME_EMPTY,FRAME_SINGLE,FRAME_DOUBLE,
//        FRAME_SOLID, FRAME_SHADED
// attr - okre�la atrybut koloru dla ramki
// xb,yb - wsp�rz�dne lewego g�rnego wierzcho�ka
// width, height - rozmiar obszaru. Ujemne
//    warto�ci "odwracaj�" kierunek rysowania
//------------------------------------------------
// Funkcja nie poprawia ramek wykraczaj�cych poza
// obszar - mo�na je celowo obcina�
//------------------------------------------------
void Frame(FRAMES type, WORD attr, SHORT xb, SHORT yb, SHORT width, SHORT height)
{
	if (!width || !height || width == 1 || height == 1 || width == -1 || height == -1)		return;

	CHAR c;

	// "Odwrotny" kierunek rysowania
	if (width < 0)
	{
		xb = xb + width + 1;
		width = -width;
	}
	if (height < 0)
	{
		yb = yb + height + 1;
		height = -height;
	}

	switch (type) // lewy g�rny r�g
	{
	case FRAME_EMPTY:	c = (CHAR)32;		break;
	case FRAME_SINGLE:	c = (CHAR)218;	break;
	case FRAME_DOUBLE:	c = (CHAR)201;	break;
	case FRAME_SOLID:	c = (CHAR)219;	break;
	case FRAME_SHADED:	c = (CHAR)177;	break;
	}
	FillRect(c, attr, xb, yb, 1, 1);

	switch (type) // lewy dolny r�g
	{
	case FRAME_SINGLE:	c = (CHAR)192;	break;
	case FRAME_DOUBLE:	c = (CHAR)200;	break;
	}
	FillRect(c, attr, xb, yb + height - 1, 1, 1);

	switch (type) // prawy g�rny r�g
	{
	case FRAME_SINGLE:	c = (CHAR)191;	break;
	case FRAME_DOUBLE:	c = (CHAR)187;	break;
	}
	FillRect(c, attr, xb + width - 1, yb, 1, 1);

	switch (type) // prawy dolny r�g
	{
	case FRAME_SINGLE:	c = (CHAR)217;	break;
	case FRAME_DOUBLE:	c = (CHAR)188;	break;
	}
	FillRect(c, attr, xb + width - 1, yb + height - 1, 1, 1);

	if (width > 2) // g�ra i d�
	{
		switch (type)
		{
		case FRAME_SINGLE:	c = (CHAR)196;	break;
		case FRAME_DOUBLE:	c = (CHAR)205;	break;
		}
		FillRect(c, attr, xb + 1, yb, width - 2, 1);
		FillRect(c, attr, xb + 1, yb + height - 1, width - 2, 1);
	}

	if (height > 2) // lewy i prawy
	{
		switch (type)
		{
		case FRAME_SINGLE:	c = (CHAR)179;	break;
		case FRAME_DOUBLE:	c = (CHAR)186;	break;
		}
		FillRect(c, attr, xb, yb + 1, 1, height - 2);
		FillRect(c, attr, xb + width - 1, yb + 1, 1, height - 2);
	}
}

// Funkcja rysuje prostok�tn� ramk� z wype�nieniem
// type - okre�la typ ramki. Dozwolone typy, to:
//   FRAME_EMPTY  - ramka zbudowana ze spacji
//   FRAME_SINGLE - ramka pojedyncza
//   FRAME_DOUBLE - ramka podw�jna
//   FRAME_SOLID  - ramka wype�niona znakiem pe�nym
//   FRAME_SHADED - ramka wype�niona znakiem cieniowanym
// attr - okre�la atrybut koloru dla ramki i t�a
// xb,yb - wsp�rz�dne lewego g�rnego wierzcho�ka
// width, height - rozmiar obszaru. Ujemne
//    warto�ci "odwracaj�" kierunek rysowania
//------------------------------------------------ 
void FillFrame(FRAMES type, WORD attr, SHORT xb, SHORT yb, SHORT width, SHORT height)
{
	if (!width || !height || width == 1 || height == 1 || width == -1 || height == -1)		return;

	 // "Odwrotny" kierunek rysowania
	if (width < 0)
	{
		xb = xb + width + 1;
		width = -width;
	}
	if (height < 0)
	{
		yb = yb + height + 1;
		height = -height;
	}

	Frame(type, attr, xb, yb, width, height);
	
	if (width > 2 && height > 2)
		FillRect(' ', attr, xb + 1, yb + 1, width - 2, height - 2);
}

// Funkcja rysuje prostok�tn� ramk� bez wype�nienia
// type - okre�la typ ramki. Dozwolone typy, to:
//   FRAME_EMPTY  - ramka zbudowana ze spacji
//   FRAME_SINGLE - ramka pojedyncza
//   FRAME_DOUBLE - ramka podw�jna
//   FRAME_SOLID  - ramka wype�niona znakiem pe�nym
//   FRAME_SHADED - ramka wype�niona znakiem cieniowanym
// attr - okre�la atrybut koloru dla ramki i t�a
// yb - wsp�rz�dna y pierwszej linii ramki
// width, height - rozmiar ramki
//------------------------------------------------ 
void FrameCenter(FRAMES type, WORD attr, SHORT yb, SHORT width, SHORT height)
{
	Frame(type, attr, (cbuffer_size.X - width) / 2, yb, width, height);
}

void FillFrameCenter(FRAMES type, WORD attr, SHORT yb, SHORT width, SHORT height)
{
	FillFrame(type, attr, (cbuffer_size.X - width) / 2, yb, width, height);
}

// Funkcja dope�nia ci�g znak�w spacjami do okre�lonego
// rozmiaru, uwzgl�dniaj�c znaczniki formatowania
//-----------------------------------------------------
void FitFormattedToSize(std::string & str, size_t size)
{
	int iToFill = size - EraseFormatted(str).length();
	if (iToFill > 0)
		str += std::string(iToFill, ' ');
}

// Funkcja prze��cza okno konsoli w tryb pe�noekranowy
// lub okienkowy. Pe�ny ekran ma 80 kolumn x 50 wierszy
// mode = true  - pe�ny ekran
// mode = false - okno
//-----------------------------------------------------
void Fullscreen(bool mode)
{
	COORD dim;
	SetConsoleDisplayMode(_hcout, mode ? 1 : 2, &dim);
}

// Funkcja odczytuje atrybut z pozycji x,y ekranu konsoli
//-------------------------------------------------------
// x - pozycja w poziomie (0..79)
// y - pozycja w pionie   (0..49)
//-------------------------------------------------------
int GetAttrXY(SHORT x, SHORT y)
{
	COORD pos = { x,y };
	WORD attr;
	DWORD crd;

	ReadConsoleOutputAttribute(_hcout, &attr, 1, pos, &crd);
	return attr;
}

// Funkcja odczytuje kod klawisza z bufora konsoli. Je�li bufor jest pusty, czeka na klawisz.
// Odczytany znak nie pojawia si� na ekranie. Je�li naci�ni�to klawisz steruj�cy, to pierwsze
// wywo�anie funkcji zawsze zwraca kod 0. Drugie wywo�anie zwraca kod klawiaturowy danego
// klawisza steruj�cego.
//-------------------------------------------------------------------------------------------
int GetCh()
{
	INPUT_RECORD inp;
	DWORD nread;
	DWORD kbdmode;
	struct kbd *k;
	int keycode, state, c;

	// Je�li poprzednie wywo�anie zwr�ci�o rozszerzony kod 0,
	// zwracamy kod klawiaturowy tego klawisza.

	if (_cextend != -1)
	{
		c = _cextend;
		_cextend = -1; // Resetujemy znacznik
		return c;      // Zwracamy rozszerzony kod klawiaturowy
	}

	// Pobieramy tryb pracy konsoli, a nast�pnie ustawiamy go w tryb binarny.
	// Robimy to, aby zapobiec przetwarzaniu przez Windows klawiszy steruj�cych
	// np. Ctrl-C lub Cntrl-S.

	if (!GetConsoleMode(_hcin, &kbdmode)) return -1;
	if (!SetConsoleMode(_hcin, 0))        return -1;

	// Pobieramy zdarzenia klawiatury, a� napotkamy w�a�ciwe

	for (;;)
	{
		if (!ReadConsoleInput(_hcin, &inp, 1, &nread))
		{
			c = -1; break;
		}
		else if ((inp.EventType == KEY_EVENT) && inp.Event.KeyEvent.bKeyDown)
		{
			keycode = inp.Event.KeyEvent.wVirtualKeyCode;
			state = inp.Event.KeyEvent.dwControlKeyState;
			if ((state & NUMLOCK_ON) && (keycode == VK_DELETE)) keycode = VK_DECIMAL;

			// Poszukujemy wirtualnego kodu klawisza w tablicy. Ignorujemy
			// nierozpoznane klawisze

			for (k = &kbdtab[0]; keycode != k->keycode && k->keycode != -1; k++);
			if (k->keycode == -1) continue; // warto�� nieobecna w tablicy

											// Sprawdzamy stan klawiszy steruj�cych. ALT posiada najwy�szy priorytet
											// nast�pnie Ctrl i Shift. W zale�no�ci od stanu tych klawiszy
											// wybieramy odpowiedni� pozycj� w tablicy

			if (state & (RIGHT_ALT_PRESSED | LEFT_ALT_PRESSED))        c = k->alt;
			else if (state & (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED)) c = k->ctrl;
			else if (state & SHIFT_PRESSED)                            c = k->shift;
			else
			{

				// Je�li jest to klawisz znakowy, wykorzystujemy kod ASCII dostarczony 
				// przez Windows i uwzgl�dniamy stan CapsLock.

				if (keycode >= 'A' && keycode <= 'Z')
					c = inp.Event.KeyEvent.uChar.AsciiChar;
				else
					c = k->normal;
			}
			if (c == -1) continue; // brak odpowiednika BIOS

								   // Je�li jest to klawisz rozszerzony, zapami�tujemy jego kod
								   // i zwracamy 0.  Przy nast�pnym wywo�aniu zostanie zwr�cona
								   // warto�� klawisza.

			if (ISEXT(c))
			{
				_cextend = EXTVAL(c);
				c = 0;
			}
			break;
		}
	}

	// Przywracamy normalny tryb pracy konsoli

	if (!SetConsoleMode(_hcin, kbdmode)) return -1;
	return c;
}

// Funkcja odczytuje znak z pozycji x,y ekranu konsoli
//----------------------------------------------------
// x - pozycja w poziomie (0..79)
// y - pozycja w pionie   (0..49)
//----------------------------------------------------
char GetChXY(SHORT x, SHORT y)
{
	COORD pos = { x,y };
	CHAR c;
	DWORD crd;
	
	ReadConsoleOutputCharacterA(_hcout, &c, 1, pos, &crd);
	return c;
}

// Funkcja tworzy bufor na tre�� okna konsoli.
// Nast�pnie umieszcza w tym buforze zawarto��
// prostok�tnego obszaru konsoli i zwraca
// wskazanie do bufora.
// xb,yb - wsp�rz�dne lewego g�rnego rogu
// xe,ye - wsp�rz�dna prawego dolnego rogu
//--------------------------------------------
WORD * GetRect(int xb, int yb, int xe, int ye)
{
	CONSOLE_SCREEN_BUFFER_INFO cinf;
	COORD pos;
	DWORD crd;

	if (!GetConsoleScreenBufferInfo(_hcout, &cinf)) return NULL;
	if ((xb >= cinf.dwSize.X) || (yb >= cinf.dwSize.Y) ||
		(xe < xb) || (ye < yb) || (xe < 0) || (ye < 0)) return NULL; // poza oknem
	if (xb < 0) xb = 0;
	if (yb < 0) yb = 0;
	if (xe >= cinf.dwSize.X) xe = cinf.dwSize.X - 1;
	if (ye >= cinf.dwSize.Y) ye = cinf.dwSize.Y - 1;
	DWORD len_x = xe - xb + 1;
	WORD * buf = new(WORD[4 + 2 * len_x * (ye - yb + 1)]);
	if (buf)
	{
		buf[0] = xb; buf[1] = yb; buf[2] = xe; buf[3] = ye; // wsp�rz�dne obszaru
		int j = 0;
		for (int i = yb; i <= ye; i++)
		{
			pos.X = xb; pos.Y = i;
			ReadConsoleOutputCharacterA(_hcout, (LPSTR)(buf + 4 + (j++)*len_x), len_x, pos, &crd);
			ReadConsoleOutputAttribute(_hcout, buf + 4 + (j++)*len_x, len_x, pos, &crd);
		}
	}
	return buf;
}

// Funkcja umieszcza kursor na zadanej pozycji
//--------------------------------------------
// x - pozycja w poziomie (0..79)
// y - pozycja w pionie   (0..49)
//--------------------------------------------
void GotoXY(SHORT x, SHORT y)
{
	COORD cpos = { x,y };
	SetConsoleCursorPosition(_hcout, cpos);
}

// Funkcja rozja�nia kolor znak�w
//-------------------------------
void HighVideo()
{
	_txattr = _txattr | 0x8;
	SetConsoleTextAttribute(_hcout, _txattr);
}

// Funkcja sprawdza, czy u�ytkownik nacisn�� jaki�
// klawisz na klawiaturze. Je�li tak, zwraca true.
//------------------------------------------------
bool KbHit()
{
	DWORD en, enr; // liczba zdarze� oraz liczba odczytanych zdarze�

	GetNumberOfConsoleInputEvents(_hcin, &en);
	INPUT_RECORD * ir = new INPUT_RECORD[en];  // przydzielamy pami��
	PeekConsoleInput(_hcin, ir, en, &enr);     // odczytujemy zdarzenia

	for (DWORD i = 0; i < enr; i++)               // przegl�damy zdarzenia
	{
		if (ir[i].EventType & KEY_EVENT &&
			ir[i].Event.KeyEvent.bKeyDown)        // je�li zdarzenie klawisza!
		{                                        // ale nie kontrolnego!
			if (ir[i].Event.KeyEvent.wVirtualKeyCode != VK_CONTROL &&
				ir[i].Event.KeyEvent.wVirtualKeyCode != VK_MENU &&
				ir[i].Event.KeyEvent.wVirtualKeyCode != VK_SHIFT)
			{
				delete ir; return true;              // klawisz w buforze konsoli
			}
		}
	}
	delete ir; return false;                   // bufor konsoli jest pusty
}

// Funkcja przyciemnia kolor znak�w
//---------------------------------
void LowVideo()
{
	_txattr = _txattr & 0xf7;
	SetConsoleTextAttribute(_hcout, _txattr);
}

// Funkcja zapisuje podany atrybut na pozycji x,y.
// Pozycja kursora i znak na tej pozycji nie s� zmieniane
//-------------------------------------------------------
// attr - atrybut koloru
// x - pozycja w poziomie (0..79)
// y - pozycja w pionie   (0..49)
//-------------------------------------------------------
void PutAttrXY(WORD attr, SHORT x, SHORT y)
{
	COORD pos = { x,y };
	DWORD cwrt;

	WriteConsoleOutputAttribute(_hcout, &attr, 1, pos, &cwrt);
}

// Funkcja zapisuje podany znak na pozycji x,y.
// Pozycja kursora i atrybut pozycji nie s� zmieniane.
//----------------------------------------------------
// c - znak ASCII do zapisu
// x - pozycja w poziomie (0..79)
// y - pozycja w pionie   (0..49)
//----------------------------------------------------
void PutChXY(char c, SHORT x, SHORT y)
{
	COORD pos = { x,y };
	DWORD cwrt;

	WriteConsoleOutputCharacterA(_hcout, &c, 1, pos, &cwrt);
}

// Funkcja odtwarza prostok�tny obszar ekranu
// zapami�tany w buforze przez GetRect(). Bufor
// jest zwalniany do puli pami�ci systemu.
// buf - wska�nik bufora z tre�ci� obszaru
//---------------------------------------------
void PutRect(WORD * buf)
{
	COORD pos;
	DWORD crd;

	if (!buf) return; // na wypadek, gdy bufor jest pusty!
	DWORD len_x = buf[2] - buf[0] + 1;
	int j = 0;
	for (int i = buf[1]; i <= buf[3]; i++)
	{
		pos.X = buf[0]; pos.Y = i;
		WriteConsoleOutputCharacterA(_hcout, (LPSTR)(buf + 4 + (j++)*len_x), len_x, pos, &crd);
		WriteConsoleOutputAttribute(_hcout, buf + 4 + (j++)*len_x, len_x, pos, &crd);
	}
	delete buf;
}

// Funkcja zapisuje podany znak i atrybut na pozycji x,y.
// Pozycja kursora pozycji nie jest zmieniana.
//-------------------------------------------------------
// c - znak ASCII do zapisu
// attr - atrybut koloru
// x - pozycja w poziomie (0..79)
// y - pozycja w pionie   (0..49)
//-------------------------------------------------------
void PutXY(CHAR c, WORD attr, SHORT x, SHORT y)
{
	COORD pos = { x,y };
	DWORD cwrt;

	WriteConsoleOutputCharacterA(_hcout, &c, 1, pos, &cwrt);
	WriteConsoleOutputAttribute(_hcout, &attr, 1, pos, &cwrt);
}

// Funkcja przesuwa zawarto�� prostok�tnego obszaru
// w wyznaczonym kierunku o zadan� liczb� pozycji.
// Powsta�y pusty obszar wype�niony zostaje spacjami
// w aktualnym kolorze t�a.
//------------------------------------------------------------------------
// dir - kierunek przesuwu okna. Dozwolone s� nast�puj�ce kierunki:
//   SCROLL_UP    - przesuw w g�r�
//   SCROLL_RIGHT - przesuw w prawo
//   SCROLL_DOWN  - przesuw w d�
//   SCROLL_LEFT  - przesuw w lewo
// howfar - okre�la liczb� kolumn lub wierszy, o kt�re b�dzie przesuni�cie
// xb,yb - wsp�rz�dna lewego g�rnego wierzcho�ka
// xe,ye - wsp�rz�dna prawego dolnego wierzcho�ka
//------------------------------------------------------------------------ 
void ScrollRect(int dir, unsigned howfar, int xb, int yb, int xe, int ye)
{
	SMALL_RECT r, cr;
	COORD dest;
	CONSOLE_SCREEN_BUFFER_INFO cinf;

	if (!GetConsoleScreenBufferInfo(_hcout, &cinf)) return;
	if ((xb >= cinf.dwSize.X) || (yb >= cinf.dwSize.Y) || (dir > 3) || !howfar ||
		(xe < xb) || (ye < yb) || (xe < 0) || (ye < 0)) return; // poza oknem
	if (xb < 0) xb = 0;
	if (yb < 0) yb = 0;
	if (xe >= cinf.dwSize.X) xe = cinf.dwSize.X - 1;
	if (ye >= cinf.dwSize.Y) ye = cinf.dwSize.Y - 1;
	dest.X = r.Left = cr.Left = xb;
	dest.Y = r.Top = cr.Top = yb;
	r.Right = cr.Right = xe;
	r.Bottom = cr.Bottom = ye;
	switch (dir)
	{
	case SCROLL_UP: r.Top += howfar; break;
	case SCROLL_RIGHT: r.Right -= howfar; dest.X += howfar; break;
	case SCROLL_DOWN: r.Bottom -= howfar; dest.Y += howfar; break;
	case SCROLL_LEFT: r.Left += howfar; break;
	}
	CHAR_INFO chinf = { ' ',_txattr };
	ScrollConsoleScreenBuffer(_hcout, &r, &cr, dest, &chinf);
}

// Funkcja scrolluje okno tak, �eby podany
// rz�d znalaz� si� na g�rze okna
//--------------------------------------------
void ScrollWindowTo(SHORT y)
{
	CONSOLE_SCREEN_BUFFER_INFO csbiBuff;
	SMALL_RECT srWindow;
	SHORT sRelChange;

	// Pobierz informacj� o obecnej pozycji
	if (!GetConsoleScreenBufferInfo(_hcout, &csbiBuff))
		return;

	srWindow = csbiBuff.srWindow;
	sRelChange = y - srWindow.Top;

	// Przesu�
	srWindow.Top += sRelChange;
	srWindow.Bottom += sRelChange;

	// Popraw, je�li wyjecha�o poza bufor
	if (srWindow.Top < 0)
	{
		srWindow.Bottom -= srWindow.Top;
		srWindow.Top = 0;
	}
	else if (srWindow.Bottom >= csbiBuff.dwSize.Y)
	{
		srWindow.Top -= srWindow.Bottom - csbiBuff.dwSize.Y + 1;
		srWindow.Bottom = csbiBuff.dwSize.Y - 1;
	}

	// Uaktualnij
	SetConsoleWindowInfo(_hcout, TRUE, &srWindow);
}

// Funkcja ustawia atrybut koloru
//-------------------------------
// attr - nowy atrybut
//-------------------------------
void TextAttr(WORD attr)
{
	_txattr = attr & 0xff;
	SetConsoleTextAttribute(_hcout, _txattr);
}

// Funkcja ustawia kolor t�a tekstu
//---------------------------------
// attr - kod koloru tekstu
//---------------------------------
void TextBackground(WORD attr)
{
	_txattr = (_txattr & 0xf) | ((attr & 0xf) << 4);
	SetConsoleTextAttribute(_hcout, _txattr);
}

// Funkcja ustawia kolor tekstu i t�a
//--------------------------------------------
// attr - kod atrybutu dla koloru tekstu i t�a
//--------------------------------------------
void TextColor(WORD attr)
{
	_txattr = (_txattr & 0xf0) | (attr & 0xf);
	SetConsoleTextAttribute(_hcout, _txattr);
}

// Funkcja wy�wietla na bie��cej pozycji kursora sformatowany tekst. Formatoranie odbywa
// si� poprzez umieszczenie w tek�cie {kolor_tekstu|kolor_t�a}.
// Je�li wyra�enie w klamrach nie mo�e by� w 100% zinterpretowane, zostanie wy�wietlone,
// mo�na wi�c bez problemu wy�wietla� klamry
//--------------------------------------------------------------------------------------
// Dost�pne kolory: 
//   black, dark_red, dark_green, dark_blue, dark_yellow, dark_cyan, dark_magenta, grey, 
//   dark_grey, red, green, blue, yellow, cyan, magenta, white
// Wielko�� liter bez znaczenia. Bia�e znaki mi�dzy {, }, | i nazwami kolor�w ignorowane
// Znacznik {red|} (albo {red}) spowoduj� zmian� koloru tekstu bez zmiany koloru t�a.
// Podobnie {|red} spowoduje zmian� t�a bez zmiany koloru tekstu, a * reprezentuje
// wyj�ciowy kolor {*|red}, {red|*}, {*|*} (mo�na kr�cej {*})
//--------------------------------------------------------------------------------------
// Jest to rozwi�zanie wygodne w przypadku cz�stej zmianie atrybut�w w wy�wietlanym
// ci�giem tek�cie, aczkolwiek mniej optymalne od "r�cznego" TextAttr()
//--------------------------------------------------------------------------------------
// Funkcja korzysta ze strumienia cout i zachowuje jego zachowanie - przechodzenie do
// nowej linii i przesuwanie kursora
//--------------------------------------------------------------------------------------
void WriteFormatted(const std::string & text)
{
	 // Je�eli pusty tekst, nic nie r�b
	if (text.empty())
		return;

	size_t uiBegIdx = 0U;
	size_t uiEndIdx;
	size_t uiBracketEndIdx = 0U;
	size_t uiBarIdx;
	size_t uiLength = text.length();
	std::string szBracketString;
	std::string szFg, szBg;
	int sFgColor, sBgColor;
	WORD usOldAttr = _txattr;

	 // Powtarzaj dla ka�dego napotkanego koloru
	do
	{
		 // Nie odnaleziono "{kolor}" w obecnej iteracji - wy�wietl reszt� tekstu od ostatniego "{kolor}" (lub pocz�tku) i przerwij
		if ((uiBracketEndIdx = text.find('}', uiBracketEndIdx + 1)) == std::string::npos || (uiEndIdx = text.find_last_of('{', uiBracketEndIdx)) == std::string::npos)
		{
			cout << _pl(text.substr(uiBegIdx));
			break;
		}
		 // Odnaleziono "{ ... }" - sprawd�, czy zawiera poprawny kolor
		else
		{
			 // Wydziel zawarto�� klamer i sprawd� format
			szBracketString = pkutl::trim_ws(text.substr(uiEndIdx + 1, uiBracketEndIdx - uiEndIdx - 1));
			uiBarIdx = szBracketString.find('|');

			 // Format {tekst} lub {*}
			if (uiBarIdx == std::string::npos)
			{
				 // Szukaj nazwy koloru i pobierz kod; je�li nie znaleziono, szukaj nast�pnego "{ ... }" - kolejny obieg p�tli
				if (szBracketString == "*")
					sFgColor = usOldAttr;
				else
					sFgColor = pkutl::array_find_idx(szColorNames, 16, szBracketString) | (_txattr & 0xf0);

				if (sFgColor != -1)
				{
					 // Wy�wietl tekst od poprzedniego "{kolor}" (lub pocz�tku) do obecnego "{kolor}"; zmie� atrybuty na nowe; je�li ju� koniec tekstu, przerwij
					cout << _pl(text.substr(uiBegIdx, uiEndIdx - uiBegIdx));
					if ((uiBegIdx = uiBracketEndIdx + 1) == uiLength)
						break;
					else
						TextAttr(sFgColor);
				}
			}
			 // Format {tekst|t�o}
			else
			{
				szFg = pkutl::trim_ws(szBracketString.substr(0U, uiBarIdx));
				szBg = pkutl::trim_ws(szBracketString.substr(uiBarIdx + 1));

				if (szFg.empty())		sFgColor = _txattr & 0x0f;
				else if (szFg == "*")	sFgColor = usOldAttr & 0x0f;
				else					sFgColor = pkutl::array_find_idx(szColorNames, 16, szFg);

				if (szBg.empty())		sBgColor = (_txattr & 0xf0) >> 4;
				else if (szBg == "*")	sBgColor = (usOldAttr & 0xf0) >> 4;
				else					sBgColor = pkutl::array_find_idx(szColorNames, 16, szBg);

				if (sFgColor != -1 && sBgColor != -1)
				{
					cout << _pl(text.substr(uiBegIdx, uiEndIdx - uiBegIdx));
					if ((uiBegIdx = uiBracketEndIdx + 1) == uiLength)
						break;
					else
						TextAttr(sBgColor << 4 | sFgColor);
				}
			}
		}
	} while (true);

	 // Przywr�� poprzedni atrybut sprzed u�ycia funkcji
	TextAttr(usOldAttr);
}

// Funkcja odczytuje bie��c� pozycj� x kursora
//--------------------------------------------
SHORT WhereX()
{
	CONSOLE_SCREEN_BUFFER_INFO sinf;

	GetConsoleScreenBufferInfo(_hcout, &sinf);
	return sinf.dwCursorPosition.X;
}

// Funkcja odczytuje bie��c� pozycj� y kursora
//--------------------------------------------
SHORT WhereY()
{
	CONSOLE_SCREEN_BUFFER_INFO sinf;

	GetConsoleScreenBufferInfo(_hcout, &sinf);
	return sinf.dwCursorPosition.Y;
}

// Funkcja zwraca rozmiar bufora konsoli
//------------------------------------------------------
COORD GetBufferSize(void)
{
	return cbuffer_size;
}
