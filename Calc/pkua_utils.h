// pkua_utils.h
//---------------------------------------------------------------------
// Plik nag³ówkowy z przydatnymi funkcjami, definicjami, zmiennymi,
// itd.
//---------------------------------------------------------------------
// (C) PKua 2016
//---------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <limits>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <iostream>

//*********************************************************************
// PRZYDATNE DEFINICJE
//*********************************************************************


 // Ci¹g bia³ych znaków
#ifndef WHITE_CHARS
#	define WHITE_CHARS		" \v\f\r\n\t"
#endif

 // Przypadki case ze znakami literowymi
#ifndef CASE_ALPHA_C
#	define CASE_ALPHA_C		case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':case 'g':case 'h':case 'i':case 'j':case 'k':case 'l':case 'm':case 'n':case 'o':case 'p':case 'q':case 'r':case 's':\
							case 't':case 'u':case 'v':case 'w':case 'x':case 'y':case 'z':case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':case 'G':case 'H':case 'I':case 'J':case 'K':case 'L':\
							case 'M':case 'N':case 'O':case 'P':case 'Q':case 'R':case 'S':case 'T':case 'U':case 'V':case 'W':case 'X':case 'Y':case 'Z'
#endif

 // Przypadki case ze znakami liczbowymi
#ifndef CASE_NUM_C
#	define CASE_NUM_C		case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9'
#endif

 // Przypadki case z bialymi znakami
#ifndef CASE_WHITE_C
#	define CASE_WHITE_C		case ' ':case '\v':case '\f':case '\r':case '\n':case '\t'
#endif

 // Stringizacja definicji
#define STRFY(def)			_STRFY(def)
#define _STRFY(def)			#def


namespace pkutl
{

	//*********************************************************************
	// PRZYDATNE FUNKCJE
	//*********************************************************************


	std::string					trim_str(const std::string &str, const char * chars);
	void						trim_str_ref(std::string &str, const char * chars);
	std::string					trim_ws(const std::string &str);
	void						trim_ws_ref(std::string &str);
	std::vector<std::string>	explode(const std::string &str, char delim);
	void						explode(const std::string &str, char delim, std::string * &array, size_t &numtokens);
	void						explode(const std::string &str, char delim, char ** &array, size_t &numtokens);
	bool						is_int(const std::string &str);
	bool						is_uint(const std::string &str);
	unsigned short				count_max_page(unsigned short num_items, unsigned short items_per_page);
	/*export template<typename _T>
		inline int					array_find_idx(const _T * array, size_t num_of, _T item);*/
	/*{
		_T * end = std::find(array, array + num_of, item);
		if (end == array + num_of)
			return -1;
		else
			return end - array;
	}*/

	int							array_find_idx(const char ** array, size_t num_of, const std::string & item);
	//int							array_find_idx(const std::string * array, size_t num_of, const std::string & item);
	double						intel_sum(double x1, double x2);
	double						intel_diff(double x1, double x2);
	bool						intel_equals(double x1, double x2);
	bool						intel_is_int(double x);
	double						intel_round_if_int(double x);
	std::string					str_fill_to_size(std::string str, size_t size);
	void						str_fill_to_size_ref(std::string &str, size_t size);
	double						round_signif(double x, unsigned short n);

	template<typename _T>
	inline void					set_flags(_T & settings, _T flags)
	{
		settings = settings | flags;
	}

	template<typename _T>
	inline void					unset_flags(_T & settings, _T flags)
	{
		settings = settings & ~flags;
	}

	std::string					format_fixed(double x, unsigned char signif, unsigned short max_zeros = (unsigned short)(-1));
	const std::string &			declension(unsigned long long x, const std::string & first, const std::string & second, const std::string & third);
}