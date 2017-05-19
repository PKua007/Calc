// Zbiór procedur obs³ugi konsoli znakowej w Windows
//--------------------------------------------------
//          (C)2006 mgr Jerzy Wa³aszek
//			edytowane 2016 PKua
//--------------------------------------------------

#if !defined(__NEW_CON_FUNCT__)
#define __NEW_CON_FUNCT__

#include "stdafx.h"
//#define NOMINMAX
#include <windows.h>
#include <string>

using namespace std;

#if !defined(__COLORS)
#define __COLORS

enum COLORS
{
	BLACK,          // ciemne kolory
	BLUE,
	GREEN,
	CYAN,
	RED,
	MAGENTA,
	BROWN,
	LIGHTGRAY,
	DARKGRAY,       // jasne kolory
	LIGHTBLUE,
	LIGHTGREEN,
	LIGHTCYAN,
	LIGHTRED,
	LIGHTMAGENTA,
	YELLOW,
	WHITE
};

#endif

#if !defined(__FRAMES_TYPES__)
#define __FRAMES_TYPES__

enum FRAMES
{
	FRAME_EMPTY,
	FRAME_SINGLE,
	FRAME_DOUBLE,
	FRAME_SOLID,
	FRAME_SHADED
};

#endif

#if !defined(__SCROLL_DIRECTION__)
#define __SCROLL_DIRECTION__

enum SCROLLS
{
	SCROLL_UP,
	SCROLL_RIGHT,
	SCROLL_DOWN,
	SCROLL_LEFT
};

#endif

#if !defined(__FILL_MODE__)
#define __FILL_MODE__

enum FILL_MODE
{
	FILL_ATTR,
	FILL_CH,
	FILL_CH_ATTR
};

#endif

#define	NC_KEY_LEFT		75
#define NC_KEY_RIGHT	77
#define NC_KEY_UP		72
#define NC_KEY_DOWN		80
#define NC_KEY_HOME		71
#define NC_KEY_END		79

void _cinit(bool _fullscreen = false, bool forbid_resize = false);
string _pl(string s);
void Center(SHORT y, string s, SHORT leftbound = 0, SHORT fieldwidth = 32767);
void CenterFormatted(SHORT y, string s, SHORT leftbound = 0, SHORT fieldwidth = 32767);
void Clrscr(SHORT xb, SHORT yb);
void Clrscr(void);
void CursorOff(void);
void CursorOn(void);
void Delay(DWORD msec);
std::string EraseFormatted(std::string text);
void FillRectGeneral(CHAR ch, WORD attr, SHORT xb, SHORT yb, SHORT width, SHORT height, FILL_MODE mode);
void FillRectAttr(WORD attr, SHORT xb, SHORT yb, SHORT width, SHORT height);
void FillRectCh(CHAR ch, SHORT xb, SHORT yb, SHORT width, SHORT height);
void FillRect(CHAR ch, WORD attr, SHORT xb, SHORT yb, SHORT width, SHORT height);
void Frame(FRAMES type, WORD attr, SHORT xb, SHORT yb, SHORT width, SHORT height);
void FillFrame(FRAMES type, WORD attr, SHORT xb, SHORT yb, SHORT width, SHORT height);
void FrameCenter(FRAMES type, WORD attr, SHORT yb, SHORT width, SHORT height);
void FillFrameCenter(FRAMES type, WORD attr, SHORT yb, SHORT width, SHORT height);
void FitFormattedToSize(std::string &str, size_t size);
void Fullscreen(bool);
int  GetAttrXY(SHORT, SHORT);
COORD GetBufferSize(void);
int  GetCh(void);
char GetChXY(SHORT, SHORT);
WORD * GetRect(int, int, int, int);
void GotoXY(SHORT, SHORT);
void HighVideo(void);
bool KbHit(void);
void LowVideo(void);
void PutAttrXY(WORD, SHORT, SHORT);
void PutChXY(char, SHORT, SHORT);
void PutRect(WORD *);
void PutXY(CHAR, WORD, SHORT, SHORT);
void ScrollRect(int, unsigned, int, int, int, int);
void ScrollWindowTo(SHORT y);
void TextAttr(WORD attr);
void TextBackground(WORD attr);
void TextColor(WORD attr);
void WriteFormatted(const std::string & text);
SHORT WhereX(void);
SHORT WhereY(void);

#endif //__NEW_CON_FUNCT__