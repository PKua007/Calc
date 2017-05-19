// pkua_utils.cpp
//---------------------------------------------------------------------
// Plik Ÿrod³owy z przydatnymi funkcjami, definicjami, zmiennymi, itd.
//---------------------------------------------------------------------
// (C) PKua 2016
//---------------------------------------------------------------------

#include "stdafx.h"
#include "pkua_utils.h"
#include <stdio.h>

#pragma warning( disable : 4996 )


// Funkcja przycina na pocz¹tku i koñcu stringa wszystkie podane znaki.
// Zwraca rezultat
//---------------------------------------------------------------------
// str - string do przyciêcia,
// chars - ci¹g przycinanych znaków
//---------------------------------------------------------------------
std::string pkutl::trim_str(const std::string &str, const char * chars)
{
	size_t uiBeg = str.find_first_not_of(chars);
	size_t uiEnd = str.find_last_not_of(chars);

	if (uiBeg == std::string::npos)
		return std::string("");

	return str.substr(uiBeg, uiEnd - uiBeg + 1);
}

// Funkcja przycina na pocz¹tku i koñcu stringa wszytskie podane znaki
// - korzysta z referencji
//---------------------------------------------------------------------
// str - string do przyciêcie
// chars - ci¹g przycinanych znaków
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

// Funkcja przycina na pocz¹tku i koñcu stringa wszystkie bia³e znaki.
// Zwraca rezultat
//---------------------------------------------------------------------
// str - string do przyciêcia,
//---------------------------------------------------------------------
std::string pkutl::trim_ws(const std::string & str) 
{
	return trim_str(str, WHITE_CHARS); 
}

// Funkcja przycina na pocz¹tku i koñcu stringa wszytskie bia³e znaki
// - korzysta z referencji
//---------------------------------------------------------------------
// str - string do przyciêcie
//---------------------------------------------------------------------
void pkutl::trim_ws_ref(std::string & str) 
{
	trim_str_ref(str, WHITE_CHARS); 
}

// Funkcja robzija ci¹g znaków na czêœci rozdzielone okreœlonym znakiem
//---------------------------------------------------------------------
// str - string do rozbicia
// delim - znak rozdzielaj¹cy
//---------------------------------------------------------------------
std::vector<std::string> pkutl::explode(const std::string & str, char delim)
{
	 // Utwórz wektor z odpowiedni¹ liczb¹ slotów
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

// Funkcja robzija ci¹g znaków na czêœci rozdzielone okreœlonym znakiem
//---------------------------------------------------------------------
// str - string do rozbicia
// delim - znak rozdzielaj¹cy
// array - tablica stringów do zapisania wyniku
// numtokens - licza elementów tablicy do zapisania
//---------------------------------------------------------------------
// UWAGA!!! Ta fukncja dynamicznie alokuje pamiêæ! Po u¿yciu tablicy
// tokenów nale¿y iterowaæ po wszystkich ci¹gach znaków i je usuwaæ
// za pomoc¹ delete [], a nastêpnie za pomoc¹ delete [] usun¹æ wskaŸnik
// do tablicy ci¹gów znaków. Zalecane u¿ycie wersji zwracaj¹cej wektor
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

// Funkcja robzija ci¹g znaków na czêœci rozdzielone okreœlonym znakiem
//---------------------------------------------------------------------
// str - string do rozbicia
// delim - znak rozdzielaj¹cy
// array - tablica ci¹gów do zapisania wyniku
// numtokens - licza elementów tablicy do zapisania
//---------------------------------------------------------------------
// UWAGA!!! Ta fukncja dynamicznie alokuje pamiêæ! Po u¿yciu tablicy
// tokenów nale¿y j¹ usun¹æ za pomoc¹ delete [].
// Zalecane u¿ycie wersji zwracaj¹cej wektor
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

// Funkcja sprawdza, czy podany ci¹g znaków jest liczb¹ ca³kowit¹
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

// Funkcja sprawdza, czy podany ci¹g znaków jest liczb¹ naturaln¹
//---------------------------------------------------------------------
bool pkutl::is_uint(const std::string & str)
{
	for (auto ch : str)
		if (ch < '0' || ch > '9')
			return false;

	return true;
}

// Funkcja oblicza maksymalny numer strony z wpisami przy danej licznie
// wpisów i iloœci wpisów na stronê. Zwraca wynik
//---------------------------------------------------------------------
// num_items - liczba wszystkich wpisów
// items_per_page - liczba wpisów na stronê
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

// Funkcja inteligentnie sumuje dwa czynniki, zaokr¹glaj¹c do liczby
// ca³kowitej, jeœli jest blisko i uznaj¹c za zero odpowiednie wyniki
//--------------------------------------------------------------------
double pkutl::intel_sum(double x1, double x2)
{
	double dMax = std::fmax(std::abs(x1), std::abs(x2));
	double dSum = x1 + x2;
	double dRound = std::round(dSum);

	// Jeœli po zsumowaniu wynik jest na pograniczu dok³adnoœci sumowania, uznaj za 0
	if (std::abs(dSum) < 10 * dMax * std::numeric_limits<double>::epsilon())
		return 0.0;
	// Jeœli ró¿nica miêdzy najbli¿sz¹ ca³kowit¹ jest na pograniczu dok³adnoœci sumowania, uznaj za ca³k.
	if (std::abs(dRound - dSum) < 10 * dMax * std::numeric_limits<double>::epsilon())
		return dRound;
	else
		return dSum;
}

// Funkcja inteligentnie odejmuje dwa czynniki, zaokr¹glaj¹c do liczby
// ca³kowitej, jeœli jest blisko i uznaj¹c za zero odpowiednie wyniki
//--------------------------------------------------------------------
double pkutl::intel_diff(double x1, double x2)
{
	double dMax = std::fmax(std::abs(x1), std::abs(x2));
	double dDiff = x1 - x2;
	double dRound = std::round(dDiff);

	// Jeœli po obliczeniu ró¿nicy wynik jest na pograniczu dok³adnoœci odejmowania, uznaj za 0
	if (std::abs(dDiff) < 10 * dMax * std::numeric_limits<double>::epsilon())
		return 0.0;
	// Jeœli ró¿nica miêdzy najbli¿sz¹ ca³kowit¹ jest na pograniczu dok³adnoœci odejmowania, uznaj za ca³k.
	if (std::abs(dRound - dDiff) < 10 * dMax * std::numeric_limits<double>::epsilon())
		return dRound;
	else
		return dDiff;
}

// Funkcja inteligentnie porównuje dwie liczby zmiennoprzecinkowe,
// bior¹c pod uwagê dok³adnoœæ reprezentacji
//--------------------------------------------------------------------
bool pkutl::intel_equals(double x1, double x2)
{
	return std::abs(x1 - x2) < 10 * std::fmax(std::abs(x1), std::abs(x2)) * std::numeric_limits<double>::epsilon();
}

// Funkcja sprawdza, czy liczba (przetwarzana funkcjami intel_ )
// jest ca³kowita
//--------------------------------------------------------------------
bool pkutl::intel_is_int(double x)
{
	return std::round(x) - x == 0.0;
}

// Funkcja zaokr¹gla liczbê zmiennoprzecinkow¹ do ca³kowitej,
// jeœli jest odpowiednio blisko. Jeœli nie, zwraca bez zmian
//--------------------------------------------------------------------
double pkutl::intel_round_if_int(double x)
{
	double dRound = std::round(x);
	if (std::abs(dRound - x) < 10 * x * std::numeric_limits<double>::epsilon())
		return dRound;
	else
		return x;
}

// Funkcja dope³nia ci¹g znaków, jeœli trzeba, odpowiedni¹ iloœci¹
// spacji i zwraca wynik
//-------------------------------------------------------------------
// size - rozmiar ci¹gu znaków do dope³neinia spacjami
//-------------------------------------------------------------------
std::string pkutl::str_fill_to_size(std::string str, size_t size)
{
	if (size > str.length())
		str += std::string(size - str.length(), ' ');
	return str;
}

// Funkcja dope³nia ci¹g znaków, jeœli trzeba, odpowiedni¹ iloœci¹
// spacji i nadpisuje wynik w przekazanej referencji
//-------------------------------------------------------------------
// size - rozmiar ci¹gu znaków do dope³neinia spacjami
//-------------------------------------------------------------------
void pkutl::str_fill_to_size_ref(std::string &str, size_t size)
{
	if (size > str.length())
		str += std::string(size - str.length(), ' ');
}

// Funkcja zwraca liczbê zaokr¹glon¹ do okreœlonej iloœci cyfr znacz¹cych
//-------------------------------------------------------------------
double pkutl::round_signif(double x, unsigned short n)
{
	if (x == 0.0 || n == 0)
		return 0.0;

	double magnitude = std::pow(10, n - (int)std::ceil(std::log10(x < 0 ? -x : x)));
	return std::round(x * magnitude) / magnitude;
}

// Funkcja konwertujê liczbê zmiennoprzecinkow¹ na ci¹g znaków
// z okreœlon¹ iloœci¹ cyfr znacz¹cych
//-------------------------------------------------------------------
// x - liczba do sformatowania
// signif - liczba cyfr znacz¹cych do pokazania
// max_zeros - maksymalna liczba zer przed "zawiniêciem" - 0(18 zer)0
//-------------------------------------------------------------------
// Ÿród³o: http://stackoverflow.com/a/17211620
// w³asne poprawki
//-------------------------------------------------------------------
std::string pkutl::format_fixed(double x, unsigned char signif, unsigned short max_zeros)
{
	if (x == 0 || signif == 0)
		return "0";
	if (signif > 15)
		signif = 15;

	// Liczba cyfr przed przecinkiem do najbli¿szej znacz¹cej
	int		iDigitsBeforeDec	= (int)std::ceil(std::log10(x < 0 ? -x : x));

	// Potêga 10, która po domno¿eniu do x przesunie przecinek tak, ¿e przed nim znajdzie siê dok³¹dnie signif cyfr
	double	dOrder = std::pow(10., signif - iDigitsBeforeDec);

	// Sformatuj
	std::stringstream ss;
	if (iDigitsBeforeDec >= 15)
		ss << std::fixed << std::setprecision(0) << std::round(x * dOrder) << std::setfill('0') << std::setw(iDigitsBeforeDec - signif) << std::right << "";
	else
		ss << std::fixed << std::setprecision(std::max(signif - iDigitsBeforeDec, 0)) << std::round(x * dOrder) / dOrder;
	std::string szResult = ss.str();

	// Usuñ nadmiar zer po przecinku
	if (szResult.find('.') != std::string::npos)
	{
		size_t uiNonzeroPos = szResult.find_last_not_of('0');
		if (uiNonzeroPos == std::string::npos)
			szResult = "0";
		else
		{
			szResult.erase(szResult[uiNonzeroPos] == '.' ? uiNonzeroPos : uiNonzeroPos + 1);

			// Napraw wyœwietlanie -0
			if (szResult == "-0")
				szResult = "0";
		}
	}

	// Zamieñ nadmiar zer na napis (18 zer)
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

// Funkcja odmienia rzeczownik przez przypadki, w zale¿noœci od liczby
//-------------------------------------------------------------------
// x - liczba 
// first - mianownik l. poj. (1 jab³ko)
// first - mianownik l. mn. (2 jab³ka)
// third - dope³niacz l. mn. (5 jab³ek)
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
