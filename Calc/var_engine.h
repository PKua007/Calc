// var_engine.h
//---------------------------------------------------------------------
// Plik nag��wkowy z sinlikiem zmiennych
//---------------------------------------------------------------------
// Silnik zmiennych oferuje obs�ug� bazy zmiennych - mo�na do niej
// dodawa� pozycje, przegl�da�, pobiera� warto�ci. Obs�uguje specjalne
// zmienne steruj�ce do funkcji, kt�re mog� by� u�yte tylko
// w konkretnych funkcjach.
//---------------------------------------------------------------------
// (C) 2016 PKua
//---------------------------------------------------------------------

#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <limits>

#include "pkua_utils.h"
#include "config.h"

// Klasa silnika zmiennych - singleton
//----------------------------------------------------------
class VarEngine
{
public:
	 // Wyliczenie rodzaj�w zmiennych
	enum VAR_TYPE
	{
		CONST_VAR,
		NONCONST_VAR,
		FLAG_VAR
	};


private:
	VarEngine() {};

	 // Struktura danych o zmiennej
	struct Struct
	{
	private:
		std::string		szName;				// Nazwa zmiennej
		double			dValue;				// Warto�� zmiennej
		std::string		szDesc;				// Opis zmiennej
		VAR_TYPE		eType;				// Typ zmiennej (CONST_VAR - sta�a; NONCONST_VAR - zmienna; FLAG_VAR - flaga funkcji)
		std::vector<std::string> szLinkedVector;

	public:
		Struct() = delete;
		Struct(const std::string & szName, double dValue, const std::string & szDesc, VAR_TYPE eType);

		/*Struct(const Struct & o);	
		Struct operator=(const Struct & o) = delete;
		~Struct();*/

		const std::string	&name() const { return szName; }
		const double	&value() const { return dValue; }
		const std::string	&desc() const { return szDesc; }
		const VAR_TYPE		&type() const { return eType; }
		bool			isLinkedTo(const std::string & szFunctName) const;
		void			changeValue(double dNewValue);

		friend class VarEngine;
		//friend class FunctEngine;
	};

	std::vector<Struct>		oVarVector;				// Tablica ze zmiennymi
	size_t					uiNumFlags = 0;			// Liczba zmiennych b�d�cych flagami
	size_t					uiNumNonFlags = 0;		// Liczba zmiennych nieb�d�cych flagami
	size_t					uiNumConsts = 0;		// Liczba zmiennych sta�ych
	size_t					uiNumNonConsts = 0;		// Liczba zmiennych nie-sta�ych
	size_t					uiNum = 0;				// Liczba zmiennych og�em


public:
	VarEngine(VarEngine const &) = delete;
	void operator=(VarEngine const &) = delete;

	 // Nieokre�lony indeks
	static constexpr size_t nidx = (std::numeric_limits<size_t>::max)();

	 // Pobieranie (jedynej) instancji singletonu
	static VarEngine & get()
	{
		static VarEngine singleton;
		return singleton;
	}

	size_t			add(const std::string & szName, double dValue, const std::string & szDesc, VAR_TYPE eType = CONST_VAR);
	bool			exists(const std::string & szName) const;
	size_t			getIdx(const std::string & szName) const;
	size_t			numOf() { return oVarVector.size(); };
	const size_t	&numOfFlags() const { return uiNumFlags; };
	const size_t	&numOfNonFlags() const { return uiNumNonFlags; };
	const size_t	&numOfConsts() const { return uiNumConsts; };
	const size_t	&numOfNonConsts() const { return uiNumNonConsts; };

	 // Operatory zakresu
	Struct			&operator[](size_t idx);
	Struct			&operator[](const std::string & szName);
};


 // "Zmienna globalna" singletonu
#define gVarEngine VarEngine::get()