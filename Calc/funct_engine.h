// funct_engine.h
//---------------------------------------------------------------------
// Plik nag��wkowy z sinlikiem funkcji
//---------------------------------------------------------------------
// Silnik zmiennych oferuje obs�ug� bazy funkcji - mo�na do niej
// dodawa� pozycje, przegl�da�, wykonywa� funkcje i pobiera� opisy.
// Obs�uguje funkcje z flagami (np. sin(pi, deg)) i zmienn� liczb�
// parametr�w (np. max(5, 6, 7, 24, pi))
//---------------------------------------------------------------------
// (C) 2016 PKua
//---------------------------------------------------------------------

#pragma once

#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <limits>
#include <iterator>
#include <sstream>
#include <iostream>

#include "pkua_utils.h"
#include "var_engine.h"


// Klasa silnika zmiennych - singleton
//----------------------------------------------------------
class FunctEngine
{
private:
	struct Struct;
	typedef double(*FunctPtr)(std::vector<double>);

public:

	// Struktura parametru
	//----------------------------------------------------------
	struct Param
	{
	public:

		// Wyliczenie rodaju parametru
		enum PARAM_TYPE
		{
			NONE,
			VALUE,
			FLAG,
			VA_LIST
		};

		// Parametr - warto��
		struct Value
		{
			bool	bHasDefault;	// Czy ma warto�� domy�ln�?
			double	dDefaultVal;	// Warto�� domy�lna

			explicit Value();
			explicit Value(double dDefaultVal);
		};

		// Parametr - flaga
		class Flag
		{
		public:
			std::set<size_t>	uiFlagIdxSet;	// Set z indeksami flag w silniku zmiennych
			bool				bHasDefault;	// Czy jest domy�lna flaga?
			size_t				uiDefaultIdx;	// Indeks domy�lnej flagi

			Flag() {}
			explicit Flag(const std::string & szFlags, const std::string & szDefaultFlag = "");
		};

		// Parametr - zmienna lista parametr�w
		struct VaList
		{
			unsigned short	usMinParams;		// Minimalna liczba parametr�w
			unsigned short	usMaxParams;		// Maksymalna liczba parametr�w

			explicit VaList(unsigned short usMinParams = 0, unsigned short usMaxParams = (unsigned short)(-1));

			inline bool hasMin() const
			{
				return usMinParams != 0;
			}

			inline bool hasMax() const
			{
				return usMaxParams != (unsigned short)(-1);
			}
		};

		// Unia rodzaju parametru
		union ParamType
		{
			Value		value;
			Flag		flag;
			VaList		va_list;

			ParamType() {};
			~ParamType() {};
		};

		~Param();
		Param(const Param & other);
		Param(Param && other);
		void operator=(const Param & other) = delete;
		void operator=(Param && other) = delete;

		PARAM_TYPE		eType;
		ParamType		uParam;
		std::string		szName;
		std::string		szDesc;


	private:

		// Prywatny konstuktor klasy - tylko zaprzyja�niona FunctEngine::Struct ma dost�p
		Param(const Value & value, const std::string szName, const std::string szDesc);
		Param(const Flag & flag, const std::string szName, const std::string szDesc);
		Param(const VaList & va_list, const std::string szName, const std::string szDesc);

		friend struct FunctEngine::Struct;
	};

private:

	FunctEngine() {}; // Prywatny konstruktor singletonu

	// Struktura danych funkcji
	// Podklasa jest prywatna, aby nie by�o mo�liwo�ci jawnego jej tworzenia. Tylko FunctEngine ma mo�liwo�� tworzenia i zwracania instancji 
	struct Struct
	{
	public:
		std::string					szName;						// Nazwa funkcji
		std::vector<std::string>	szAltNamesVector;			// Wektor z alternatywnymi nazwami
		FunctPtr					pFunctPointer;				// Wska�nik funkcji
		std::string					szDesc;						// Opis funkcji
		mutable std::vector<Param>	oParamsVector;				// Wektor parametr�w funkcji

		Struct() = delete;
		Struct(const std::string & szName, FunctPtr pFunctPointer, const std::string & szDesc, const std::vector<std::string> & szAltNamesVector);

		size_t			addParam(Param::Value value, const std::string szName, const std::string szDesc) const;
		size_t			addParam(Param::Flag flag, const std::string szName, const std::string szDesc) const;
		size_t			addParam(Param::VaList va_list, const std::string szName, const std::string szDesc) const;
		size_t			getParamIdx(const std::string & szName) const;
		bool			paramExists(const std::string & szName) const;
		size_t			numOfParams() const { return oParamsVector.size(); };
		bool			isNumParamsValid(size_t uiNum) const;
		void			autoCompParams(std::vector<double> & dParamsVector) const;

		// Operatory zakresu
		const Param		&operator[](size_t idx) const;
		const Param		&operator[](const std::string & szName) const;
	};

	std::vector<Struct>		oFunctVector;			// Tablica z funkcjami


public:
	FunctEngine(FunctEngine const &) = delete;
	void operator=(FunctEngine const &) = delete;

	// Nieokre�lony indeks
	static constexpr size_t nidx = (std::numeric_limits<size_t>::max)();

	// Pobieranie (jedynej) instancji singletonu
	inline static FunctEngine & get()
	{
		static FunctEngine singleton;
		return singleton;
	}

	size_t			add(const std::string & szName, FunctPtr pFunctPointer, const std::string & szDesc, const std::string & szAltNames = "");
	bool			exists(const std::string & szName) const;
	size_t			getIdx(const std::string & szName) const;
	size_t			numOf() { return oFunctVector.size(); };

	// Operatory zakresu
	const Struct	&operator[](size_t idx);
	const Struct	&operator[](const std::string & szName);
};


// "Zmienna globalna" singletonu
#define gFunctEngine FunctEngine::get()