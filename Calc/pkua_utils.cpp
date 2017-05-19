// pkua_utils.cpp
//---------------------------------------------------------------------
// Plik �rod�owy z przydatnymi funkcjami, definicjami, zmiennymi, itd.
//---------------------------------------------------------------------
// (C) PKua 2016
//---------------------------------------------------------------------

#include "stdafx.h"
#include "pkua_utils.h"
#include <stdio.h>

#pragma warning( disable : 4996 )


// Funkcja przycina na pocz�tku i ko�cu stringa wszystkie podane znaki.
// Zwraca rezultat
//---------------------------------------------------------------------
// str - string do przyci�cia,
// chars - ci�g przycinanych znak�w
//---------------------------------------------------------------------
std::string pkutl::trim_str(const std::string &str, const char * chars)
{
	size_t uiBeg = str.find_first_not_of(chars);
	size_t uiEnd = str.find_last_not_of(chars);

	if (uiBeg == std::string::npos)
		return std::string("");

	return str.substr(uiBeg, uiEnd - uiBeg + 1);
}

// Funkcja przycina na pocz�tku i ko�cu stringa wszytskie podane znaki
// - korzysta z referencji
//---------------------------------------------------------------------
// str - string do przyci�cie
// chars - ci�g przycinanych znak�w
//---------------------------------------------------------------------
void pkutl::trim_str_ref(std::string &str, const char * chars)
{
	size_t uiBeg = str.find_first_not_of(chars);
	size_t uiEnd = str.find_last_not_of(chars);

	if (uiBeg == std::string::npos)
		str = "";
	else
		str = str.substr(uiBeg, uiEnd - uiBeg + 1);
}

// Funkcja przycina na pocz�tku i ko�cu stringa wszystkie bia�e znaki.
// Zwraca rezultat
//---------------------------------------------------------------------
// str - string do przyci�cia,
//---------------------------------------------------------------------
std::string pkutl::trim_ws(const std::string & str) 
{
	return trim_str(str, WHITE_CHARS); 
}

// Funkcja przycina na pocz�tku i ko�cu stringa wszytskie bia�e znaki
// - korzysta z referencji
//---------------------------------------------------------------------
// str - string do przyci�cie
//---------------------------------------------------------------------
void pkutl::trim_ws_ref(std::string & str) 
{
	trim_str_ref(str, WHITE_CHARS); 
}

// Funkcja robzija ci�g znak�w na cz�ci rozdzielone okre�lonym znakiem
//---------------------------------------------------------------------
// str - string do rozbicia
// delim - znak rozdzielaj�cy
//---------------------------------------------------------------------
std::vector<std::string> pkutl::explode(const std::string & str, char delim)
{
	 // Utw�rz wektor z odpowiedni� liczb� slot�w
	std::vector<std::string> oReturnVector{ (str == "") ? 0U : (size_t)(std::count(str.begin(), str.end(), delim) + 1) };

	if (str.empty())
		return oReturnVector;

	std::string szItem;
	size_t uiBegIdx = 0U;
	size_t uiEndIdx;
	size_t uiLen = str.length();
	size_t i = 0U;

	while (uiBegIdx != uiLen && (uiEndIdx = str.find(delim, uiBegIdx)) != std::string::npos)
	{
		szItem = str.substr(uiBegIdx, uiEndIdx - uiBegIdx);
		oReturnVector[i++] = szItem;
		uiBegIdx = uiEndIdx + 1;
	}
	oReturnVector[i] = (uiBegIdx != uiLen) ? str.substr(uiBegIdx) : "";

	return oReturnVector;
}

// Funkcja robzija ci�g znak�w na cz�ci rozdzielone okre�lonym znakiem
//---------------------------------------------------------------------
// str - string do rozbicia
// delim - znak rozdzielaj�cy
// array - tablica string�w do zapisania wyniku
// numtokens - licza element�w tablicy do zapisania
//---------------------------------------------------------------------
// UWAGA!!! Ta fukncja dynamicznie alokuje pami��! Po u�yciu tablicy
// token�w nale�y iterowa� po wszystkich ci�gach znak�w i je usuwa�
// za pomoc� delete [], a nast�pnie za pomoc� delete [] usun�� wska�nik
// do tablicy ci�g�w znak�w. Zalecane u�ycie wersji zwracaj�cej wektor
//---------------------------------------------------------------------
void pkutl::explode(const std::string & str, char delim, std::string * &array, size_t &numtokens)
{
	numtokens = (str == "") ? 0 : std::count(str.begin(), str.end(), delim) + 1;

	if (numtokens == 0)
	{
		array = nullptr;
	}
	else
	{
		size_t uiBegIdx = 0U;
		size_t uiEndIdx;
		size_t uiLen = str.length();
		size_t i = 0U;

		array = new std::string[numtokens];

		while (uiBegIdx != uiLen && (uiEndIdx = str.find(delim, uiBegIdx)) != std::string::npos)
		{
			array[i++] = str.substr(uiBegIdx, uiEndIdx - uiBegIdx);
			uiBegIdx = uiEndIdx + 1;
		}
		array[i] = (uiBegIdx != uiLen) ? str.substr(uiBegIdx) : "";
	}
}

// Funkcja robzija ci�g znak�w na cz�ci rozdzielone okre�lonym znakiem
//---------------------------------------------------------------------
// str - string do rozbicia
// delim - znak rozdzielaj�cy
// array - tablica ci�g�w do zapisania wyniku
// numtokens - licza element�w tablicy do zapisania
//---------------------------------------------------------------------
// UWAGA!!! Ta fukncja dynamicznie alokuje pami��! Po u�yciu tablicy
// token�w nale�y j� usun�� za pomoc� delete [].
// Zalecane u�ycie wersji zwracaj�cej wektor
//---------------------------------------------------------------------
void pkutl::explode(const std::string & str, char delim, char ** &array, size_t & numtokens)
{
	numtokens = (str == "") ? 0 : std::count(str.begin(), str.end(), delim) + 1;

	if (numtokens == 0)
	{
		array = nullptr;
	}
	else
	{
		size_t uiBegIdx = 0U;
		size_t uiEndIdx;
		size_t uiLen = str.length();
		size_t i = 0U;
		std::string szItem;

		array = new char*[numtokens];

		while (uiBegIdx != uiLen && (uiEndIdx = str.find(delim, uiBegIdx)) != std::string::npos)
		{
			szItem = str.substr(uiBegIdx, uiEndIdx - uiBegIdx);
			array[i] = new char[szItem.length() + 1];
			szItem.copy (array[i], szItem.length());
			array[i][szItem.length()] = '\0';
			uiBegIdx = uiEndIdx + 1;

			i++;
		}
		szItem = (uiBegIdx != uiLen) ? str.substr(uiBegIdx) : "";
		array[i] = new char[szItem.length() + 1];
		szItem.copy(array[i], szItem.length());
		array[i][szItem.length()] = '\0';
	}
}

// Funkcja sprawdza, czy podany ci�g znak�w jest liczb� ca�kowit�
//---------------------------------------------------------------------
bool pkutl::is_int(const std::string & str)
{
	size_t i = 0U;
	for (auto ch : str)
	{
		if ((ch < '0' || ch > '9') || ((ch == '-' || ch == '+') && i != 0U))
			return false;

		i++;
	}
	return true;
}

// Funkcja sprawdza, czy podany ci�g znak�w jest liczb� naturaln�
//---------------------------------------------------------------------
bool pkutl::is_uint(const std::string & str)
{
	for (auto ch : str)
		if (ch < '0' || ch > '9')
			return false;

	return true;
}

// Funkcja oblicza maksymalny numer strony z wpisami przy danej licznie
// wpis�w i ilo�ci wpis�w na stron�. Zwraca wynik
//---------------------------------------------------------------------
// num_items - liczba wszystkich wpis�w
// items_per_page - liczba wpis�w na stron�
//---------------------------------------------------------------------
unsigned short pkutl::count_max_page(unsigned short num_items, unsigned short items_per_page)
{
	return num_items / items_per_page + ((num_items % items_per_page) ? 1 : 0);
}

int pkutl::array_find_idx(const char ** array, size_t num_of, const std::string & item)
{
	const char ** end = std::find(array, array + num_of, item);
	if (end == array + num_of)
		return -1;
	else
		return end - array;
}

// Funkcja inteligentnie sumuje dwa czynniki, zaokr�glaj�c do liczby
// ca�kowitej, je�li jest blisko i uznaj�c za zero odpowiednie wyniki
//--------------------------------------------------------------------
double pkutl::intel_sum(double x1, double x2)
{
	double dMax = std::fmax(std::abs(x1), std::abs(x2));
	double dSum = x1 + x2;
	double dRound = std::round(dSum);

	// Je�li po zsumowaniu wynik jest na pograniczu dok�adno�ci sumowania, uznaj za 0
	if (std::abs(dSum) < 10 * dMax * std::numeric_limits<double>::epsilon())
		return 0.0;
	// Je�li r�nica mi�dzy najbli�sz� ca�kowit� jest na pograniczu dok�adno�ci sumowania, uznaj za ca�k.
	if (std::abs(dRound - dSum) < 10 * dMax * std::numeric_limits<double>::epsilon())
		return dRound;
	else
		return dSum;
}

// Funkcja inteligentnie odejmuje dwa czynniki, zaokr�glaj�c do liczby
// ca�kowitej, je�li jest blisko i uznaj�c za zero odpowiednie wyniki
//--------------------------------------------------------------------
double pkutl::intel_diff(double x1, double x2)
{
	double dMax = std::fmax(std::abs(x1), std::abs(x2));
	double dDiff = x1 - x2;
	double dRound = std::round(dDiff);

	// Je�li po obliczeniu r�nicy wynik jest na pograniczu dok�adno�ci odejmowania, uznaj za 0
	if (std::abs(dDiff) < 10 * dMax * std::numeric_limits<double>::epsilon())
		return 0.0;
	// Je�li r�nica mi�dzy najbli�sz� ca�kowit� jest na pograniczu dok�adno�ci odejmowania, uznaj za ca�k.
	if (std::abs(dRound - dDiff) < 10 * dMax * std::numeric_limits<double>::epsilon())
		return dRound;
	else
		return dDiff;
}

// Funkcja inteligentnie por�wnuje dwie liczby zmiennoprzecinkowe,
// bior�c pod uwag� dok�adno�� reprezentacji
//--------------------------------------------------------------------
bool pkutl::intel_equals(double x1, double x2)
{
	return std::abs(x1 - x2) < 10 * std::fmax(std::abs(x1), std::abs(x2)) * std::numeric_limits<double>::epsilon();
}

// Funkcja sprawdza, czy liczba (przetwarzana funkcjami intel_ )
// jest ca�kowita
//--------------------------------------------------------------------
bool pkutl::intel_is_int(double x)
{
	return std::round(x) - x == 0.0;
}

// Funkcja zaokr�gla liczb� zmiennoprzecinkow� do ca�kowitej,
// je�li jest odpowiednio blisko. Je�li nie, zwraca bez zmian
//--------------------------------------------------------------------
double pkutl::intel_round_if_int(double x)
{
	double dRound = std::round(x);
	if (std::abs(dRound - x) < 10 * x * std::numeric_limits<double>::epsilon())
		return dRound;
	else
		return x;
}

// Funkcja dope�nia ci�g znak�w, je�li trzeba, odpowiedni� ilo�ci�
// spacji i zwraca wynik
//-------------------------------------------------------------------
// size - rozmiar ci�gu znak�w do dope�neinia spacjami
//-------------------------------------------------------------------
std::string pkutl::str_fill_to_size(std::string str, size_t size)
{
	if (size > str.length())
		str += std::string(size - str.length(), ' ');
	return str;
}

// Funkcja dope�nia ci�g znak�w, je�li trzeba, odpowiedni� ilo�ci�
// spacji i nadpisuje wynik w przekazanej referencji
//-------------------------------------------------------------------
// size - rozmiar ci�gu znak�w do dope�neinia spacjami
//-------------------------------------------------------------------
void pkutl::str_fill_to_size_ref(std::string &str, size_t size)
{
	if (size > str.length())
		str += std::string(size - str.length(), ' ');
}

// Funkcja zwraca liczb� zaokr�glon� do okre�lonej ilo�ci cyfr znacz�cych
//-------------------------------------------------------------------
double pkutl::round_signif(double x, unsigned short n)
{
	if (x == 0.0 || n == 0)
		return 0.0;

	double magnitude = std::pow(10, n - (int)std::ceil(std::log10(x < 0 ? -x : x)));
	return std::round(x * magnitude) / magnitude;
}

// Funkcja konwertuj� liczb� zmiennoprzecinkow� na ci�g znak�w
// z okre�lon� ilo�ci� cyfr znacz�cych
//-------------------------------------------------------------------
// x - liczba do sformatowania
// signif - liczba cyfr znacz�cych do pokazania
// max_zeros - maksymalna liczba zer przed "zawini�ciem" - 0(18 zer)0
//-------------------------------------------------------------------
// �r�d�o: http://stackoverflow.com/a/17211620
// w�asne poprawki
//-------------------------------------------------------------------
std::string pkutl::format_fixed(double x, unsigned char signif, unsigned short max_zeros)
{
	if (x == 0 || signif == 0)
		return "0";
	if (signif > 15)
		signif = 15;

	// Liczba cyfr przed przecinkiem do najbli�szej znacz�cej
	int		iDigitsBeforeDec	= (int)std::ceil(std::log10(x < 0 ? -x : x));

	// Pot�ga 10, kt�ra po domno�eniu do x przesunie przecinek tak, �e przed nim znajdzie si� dok��dnie signif cyfr
	double	dOrder = std::pow(10., signif - iDigitsBeforeDec);

	// Sformatuj
	std::stringstream ss;
	if (iDigitsBeforeDec >= 15)
		ss << std::fixed << std::setprecision(0) << std::round(x * dOrder) << std::setfill('0') << std::setw(iDigitsBeforeDec - signif) << std::right << "";
	else
		ss << std::fixed << std::setprecision(std::max(signif - iDigitsBeforeDec, 0)) << std::round(x * dOrder) / dOrder;
	std::string szResult = ss.str();

	// Usu� nadmiar zer po przecinku
	if (szResult.find('.') != std::string::npos)
	{
		size_t uiNonzeroPos = szResult.find_last_not_of('0');
		if (uiNonzeroPos == std::string::npos)
			szResult = "0";
		else
		{
			szResult.erase(szResult[uiNonzeroPos] == '.' ? uiNonzeroPos : uiNonzeroPos + 1);

			// Napraw wy�wietlanie -0
			if (szResult == "-0")
				szResult = "0";
		}
	}

	// Zamie� nadmiar zer na napis (18 zer)
	if (max_zeros != (unsigned short)(-1))
	{
		std::string szMaxZeros(max_zeros + 1, '0');
		size_t uiZeroBegIdx, uiZeroEndIdx;
		size_t uiTruncated;
		uiZeroBegIdx = szResult.find(szMaxZeros);
		if (uiZeroBegIdx != std::string::npos)
		{
			uiZeroEndIdx = szResult.find_first_not_of('0', uiZeroBegIdx + max_zeros);
			if (uiZeroEndIdx == std::string::npos)
				uiZeroEndIdx = szResult.length();

			uiTruncated = uiZeroEndIdx - uiZeroBegIdx - 2;
			szResult.replace(uiZeroBegIdx + 1, uiTruncated, "{" + std::to_string(uiTruncated) + " " + declension(uiTruncated, "zero", "zera", "zer") + "}");
		}
	}

	return szResult;
}

// Funkcja odmienia rzeczownik przez przypadki, w zale�no�ci od liczby
//-------------------------------------------------------------------
// x - liczba 
// first - mianownik l. poj. (1 jab�ko)
// first - mianownik l. mn. (2 jab�ka)
// third - dope�niacz l. mn. (5 jab�ek)
//-------------------------------------------------------------------
const std::string & pkutl::declension(unsigned long long x, const std::string & first, const std::string & second, const std::string & third)
{
	if (x == 1)
		return first;
	else if (x % 10 >= 2 && x % 10 <= 4 && (x % 100 > 14 || x % 100 < 12))
		return second;
	else
		return third;
}

/*inline int pkutl::array_find_idx(const std::string * array, size_t num_of, const std::string & item)
{
	const std::string * end = std::find(array, array + num_of, item);
	if (end == array + num_of)
		return -1;
	else
		return end - array;
}*/
