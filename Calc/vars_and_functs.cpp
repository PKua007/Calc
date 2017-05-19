// vars_and_functs.cpp
//---------------------------------------------------------------------
// Plik zr�d�owy ze zmiennymi i funkcjami w kalkulatorze
//---------------------------------------------------------------------

#include "stdafx.h"
#include "vars_and_functs.h"
#include "config.h"


// Wyliczenie jednostek k�ta do funkcji trygonometrycznych
enum TRIG_ANGlE_UNIT
{
	UNIT_RAD,
	UNIT_DEG,
	UNIT_GRAD
};

// Wyliczenie rodzaju b��du standardowego
enum STANDARD_ERROR_TYPE
{
	STD_ERR_MEAN,
	STD_ERR_SAMPLE
};


// Deklaracje funkcji
//---------------------------------------------------------------------
double CalcSinus(std::vector<double> dParamVector);
double CalcCosinus(std::vector<double> dParamVector);
double CalcTangens(std::vector<double> dParamVector);
double CalcCosecans(std::vector<double> dParamVector);
double CalcSecans(std::vector<double> dParamVector);
double CalcCotangens(std::vector<double> dParamVector);

double CalcArcSinus(std::vector<double> dParamVector);
double CalcArcCosinus(std::vector<double> dParamVector);
double CalcArcTangens(std::vector<double> dParamVector);
double CalcArcTangens2(std::vector<double> dParamVector);
double CalcArcCosecans(std::vector<double> dParamVector);
double CalcArcSecans(std::vector<double> dParamVector);
double CalcArcCotangens(std::vector<double> dParamVector);

double CalcMax(std::vector<double> dParamVector);
double CalcMin(std::vector<double> dParamVector);

double CalcToDeg(std::vector<double> dParamVector);
double CalcToRad(std::vector<double> dParamVector);
double CalcToGrad(std::vector<double> dParamVector);

double CalcPow(std::vector<double> dParamVector);
double CalcSqrt(std::vector<double> dParamVector);
double CalcRoot(std::vector<double> dParamVector);
double CalcLn(std::vector<double> dParamVector);
double CalcLog2(std::vector<double> dParamVector);
double CalcLog10(std::vector<double> dParamVector);
double CalcLog(std::vector<double> dParamVector);
double CalcExp(std::vector<double> dParamVector);
double CalcAbs(std::vector<double> dParamVector);

double CalcGamma(std::vector<double> dParamVector);
double CalcDoubleFactorial(std::vector<double> dParamVector);
double CalcNewton(std::vector<double> dParamVector);
double CalcStdErr(std::vector<double> dParamVector);

double CalcArithmeticMean(std::vector<double> dParamVector);
double CalcGeometricMean(std::vector<double> dParamVector);
double CalcHarmonicMean(std::vector<double> dParamVector);
double CalcQuadraticMean(std::vector<double> dParamVector);
double CalcGeneralizedMean(std::vector<double> dParamVector);
double CalcMedian(std::vector<double> dParamVector);

double CalcRound(std::vector<double> dParamVector);
double CalcCeiling(std::vector<double> dParamVector);
double CalcFloor(std::vector<double> dParamVector);
double CalcTrunc(std::vector<double> dParamVector);
double CalcRoundSignif(std::vector<double> dParamVector);


// Funkcja tworzy zmienne i funkcje u�ywane w kalkulatorze
//---------------------------------------------------------------------
void CreateVarsAndFuncts()
{
	// Ostatni wynik
	gVarEngine.add	("ans",				0.0,						"ostatni wynik", VarEngine::NONCONST_VAR);

	// Sta�e matematyczne
	gVarEngine.add	("pi",				M_PI,						"sta�a pi");
	gVarEngine.add	("e",				M_E,						"liczba Eulera (podstawa logarytmu naturalnego)");
	gVarEngine.add	("phi_gold",		1.61803398874989484820,		"z�oty stosunek");
	gVarEngine.add	("gamma_e",			0.577215664901532860606,	"sta�a Eulera-Mascheroniego");

	// Sta�e fizyczne
	gVarEngine.add	("c",				299792458.,					"pr�dko�� �wiat�a");
	gVarEngine.add	("mu_0",			1.25663706143591728851e-06,	"przenikalno�� magnetyczna pr�ni");
	gVarEngine.add	("epsilon_0",		8.85418781762039077775e-12,	"przenikalno�� elektryczna pr�ni");
	gVarEngine.add	("Z_0",				376.730313461,				"impedancja w�a�ciwa pr�ni");
	gVarEngine.add	("G",				6.67428e-11,				"sta�a grawitacji");
	gVarEngine.add	("h",				6.62606896e-34,				"sta�a Plancka");
	gVarEngine.add	("h_bar",			1.054571628e-34,			"zredukowana sta�a Plancka (sta�a Diraca)");
	gVarEngine.add	("planck_m",		2.17644e-8,					"masa Plancka");
	gVarEngine.add	("planck_l",		1.616252e-35,				"d�ugo�� Plancka");
	gVarEngine.add	("planck_t",		5.39124e-44,				"czas Plancka");
	gVarEngine.add	("e_chrg",			1.602176487e-19,			"�adunek elementarny");
	gVarEngine.add	("mu_B",			927.400915e-26,				"Magneton Bohra");
	gVarEngine.add	("mu_N",			5.05078324e-27,				"Magneton j�drowy");
	gVarEngine.add	("E_h",				4.35974394e-18,				"energia Hartree'go");
	gVarEngine.add	("u",				1.660538782e-27,			"jednostka masy atomowej (unit)");
	gVarEngine.add	("a_0",				0.52917720859e-10,			"promie� atomu Bohra");
	gVarEngine.add	("alpha_fs",		7.2973525376e-3,			"sta�a struktury subtelnej");
	gVarEngine.add	("alpha_fs_inv",	137.035999679,				"odwrotno�� sta�ej struktury subtelnej");
	gVarEngine.add	("g_e",				-2.0023193043622,			"czynnik Landego dla elektronu");
	gVarEngine.add	("m_e",				9.10938215e-31,				"masa spoczynkowa elektronu");
	gVarEngine.add	("mu_e",			5.4857990943e-4,			"moment magnetyczny elektronu");
	gVarEngine.add	("m_p",				1.672621637e-27,			"masa spoczynkowa protonu");
	gVarEngine.add	("mu_p",			1.410606662e-26,			"moment magnetyczny protonu");
	gVarEngine.add	("gamma_p",			2.675222005e8,				"stosunek �yromagnetyczny protonu");
	gVarEngine.add	("m_n",				1.674927211e-27,			"masa spoczynkowa neutronu");
	gVarEngine.add	("N_A",				6.02214179e23,				"sta�a Avogadra");
	gVarEngine.add	("k",				1.3806504e-23,				"sta�a Boltzmanna");
	gVarEngine.add	("F",				96485.3399,					"sta�a Faradaya");
	gVarEngine.add	("R",				8.314472,					"sta�a gazowa");
	gVarEngine.add	("v_1mol",			0.0224413996,				"obj�to�� (w m) 1 mola gazu doskona�ego (w. normalne)");
	gVarEngine.add	("n_0",				2.6867774e25,				"sta�a Loschmidta (N_A / v_1mol)");
	gVarEngine.add	("b",				2.8977685e-3,				"sta�a przesuni�c Wiena");
	gVarEngine.add	("R_inf",			10973731.568527,			"sta�a Rydberga");
	gVarEngine.add	("sigma",			5.670400e-8,				"sta�a Stefana-Boltzmanna");
	gVarEngine.add	("g",				9.8,						"przyspieszenie na Ziemi");
	gVarEngine.add	("g_pl",			9.81,						"przyspieszenie na Ziemi (Polska)");
	gVarEngine.add	("range_min",		1.1125369292536e-308,		"minimum zakresu kalkulatora");
	gVarEngine.add	("range_max",		std::numeric_limits<double>::max(), "maksimum zakresu kalkulatora");

	// Sta�e astronomiczne

	gVarEngine.add("srebrowa", 0, "takie tam nic");
	gVarEngine.add("skolmowska", -1, "wi�ksze nic");


	 // Flagi funkcji trygonometrycznych
	gVarEngine.add("rad", UNIT_RAD, "radiany", VarEngine::FLAG_VAR);
	gVarEngine.add("deg", UNIT_DEG, "stopnie", VarEngine::FLAG_VAR);
	gVarEngine.add("grad", UNIT_GRAD, "gradiusy", VarEngine::FLAG_VAR);

	
	// Flagi b��du standardowego
	gVarEngine.add("mean", STD_ERR_MEAN, "b��d �redniej", VarEngine::FLAG_VAR);
	gVarEngine.add("sample", STD_ERR_SAMPLE, "b��d pr�bki", VarEngine::FLAG_VAR);


	size_t idx;

	 // Funkcje trygonometryczne
	idx = gFunctEngine.add("sin", CalcSinus, "sinus k�ta", "sinus");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "alpha", "rozwarto�� k�ta");
	gFunctEngine[idx].addParam(FunctEngine::Param::Flag("rad;deg;grad", "rad"), "jednostka", "jednostka podanego k�ta");

	idx = gFunctEngine.add("cos", CalcCosinus, "cosinus k�ta", "cosinus");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "alpha", "rozwarto�� k�ta");
	gFunctEngine[idx].addParam(FunctEngine::Param::Flag("rad;deg;grad", "rad"), "jednostka", "jednostka podanego k�ta");

	idx = gFunctEngine.add("tan", CalcTangens, "tangens k�ta", "tangens;tg");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "alpha", "rozwarto�� k�ta");
	gFunctEngine[idx].addParam(FunctEngine::Param::Flag("rad;deg;grad", "rad"), "jednostka", "jednostka podanego k�ta");

	idx = gFunctEngine.add("csc", CalcCosecans, "cosecans k�ta", "cosecans;cosec");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "alpha", "rozwarto�� k�ta");
	gFunctEngine[idx].addParam(FunctEngine::Param::Flag("rad;deg;grad", "rad"), "jednostka", "jednostka podanego k�ta");

	idx = gFunctEngine.add("sec", CalcSecans, "secans k�ta", "secans");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "alpha", "rozwarto�� k�ta");
	gFunctEngine[idx].addParam(FunctEngine::Param::Flag("rad;deg;grad", "rad"), "jednostka", "jednostka podanego k�ta");

	idx = gFunctEngine.add("cot", CalcCotangens, "cotangens k�ta", "cotangens;ctg;cotg;cotan");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "alpha", "rozwarto�� k�ta");
	gFunctEngine[idx].addParam(FunctEngine::Param::Flag("rad;deg;grad", "rad"), "jednostka", "jednostka podanego k�ta");


	// Funkcje cyklometryczne
	idx = gFunctEngine.add("asin", CalcArcSinus, "arcus sinus", "asinus");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "parametr funkcji");
	gFunctEngine[idx].addParam(FunctEngine::Param::Flag("rad;deg;grad", "rad"), "jednostka", "jednostka wynikowego k�ta");

	idx = gFunctEngine.add("acos", CalcArcCosinus, "arcus cosinus", "acosinus");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "parametr funkcji");
	gFunctEngine[idx].addParam(FunctEngine::Param::Flag("rad;deg;grad", "rad"), "jednostka", "jednostka wynikowego k�ta");

	idx = gFunctEngine.add("atan", CalcArcTangens, "arcus tangens", "atangens;atg");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "parametr funkcji");
	gFunctEngine[idx].addParam(FunctEngine::Param::Flag("rad;deg;grad", "rad"), "jednostka", "jednostka wynikowego k�ta");

	idx = gFunctEngine.add("atan2", CalcArcTangens2, "arcus tangens uwzgl�dniaj�cy �wiatk�", "atangens2;atg2");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "y", "wsp�rz�dna y punktu na ramieniu k�ta");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "wsp�rz�dna x punktu na ramieniu k�ta");
	gFunctEngine[idx].addParam(FunctEngine::Param::Flag("rad;deg;grad", "rad"), "jednostka", "jednostka wynikowego k�ta");

	idx = gFunctEngine.add("acsc", CalcArcCosecans, "arcus cosecans k�ta", "acosecans;acosec");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "parametr funkcji");
	gFunctEngine[idx].addParam(FunctEngine::Param::Flag("rad;deg;grad", "rad"), "jednostka", "jednostka wynikowego k�ta");

	idx = gFunctEngine.add("asec", CalcArcSecans, "arcus secans k�ta", "asecans");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "parametr funkcji");
	gFunctEngine[idx].addParam(FunctEngine::Param::Flag("rad;deg;grad", "rad"), "jednostka", "jednostka wynikowego k�ta");

	idx = gFunctEngine.add("acot", CalcArcCotangens, "arcus cotangens k�ta", "acotangens;actg;acotg;acotan");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "parametr funkcji");
	gFunctEngine[idx].addParam(FunctEngine::Param::Flag("rad;deg;grad", "rad"), "jednostka", "jednostka wynikowego k�ta");


	// Funkcje por�wnuj�ce
	idx = gFunctEngine.add("max", CalcMax, "maksymalna warto�� spo�r�d podanych", "maks;maksimum;maximum;maximal;maksymalny");
	gFunctEngine[idx].addParam(FunctEngine::Param::VaList(2), "...", "warto�ci do por�wnania");

	idx = gFunctEngine.add("min", CalcMin, "minimalna warto�� spo�r�d podanych", "minimum;minimal;minimalny");
	gFunctEngine[idx].addParam(FunctEngine::Param::VaList(2), "...", "warto�ci do por�wnania");


	// Funkcje konwertuj�ce jednostki k�ta
	idx = gFunctEngine.add("deg", CalcToDeg, "konwersja na stopnie", "todeg;to_deg");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "alpha", "k�t do konwersji");
	gFunctEngine[idx].addParam(FunctEngine::Param::Flag("rad;grad", "rad"), "jednostka", "jednostka konwertowanego k�ta");

	idx = gFunctEngine.add("rad", CalcToRad, "konwersja na radiany", "torad;to_rad");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "alpha", "k�t do konwersji");
	gFunctEngine[idx].addParam(FunctEngine::Param::Flag("deg;grad", "deg"), "jednostka", "jednostka konwertowanego k�ta");

	idx = gFunctEngine.add("grad", CalcToGrad, "konwersja na gradiusy", "tograd;to_grad");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "alpha", "k�t do konwersji");
	gFunctEngine[idx].addParam(FunctEngine::Param::Flag("deg;rad", "deg"), "jednostka", "jednostka konwertowanego k�ta");


	// Pot�gi i pierwiastki
	idx = gFunctEngine.add("pow", CalcPow, "pot�ga", "power");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "podstawa pot�gi");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "a", "wyk�adnik pot�gi");

	idx = gFunctEngine.add("sqrt", CalcSqrt, "pierwiastek kwadratowy", "square_root;sqr");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "liczba pod pierwiastkiem");
	
	idx = gFunctEngine.add("root", CalcRoot, "pierwiastek n-tego stopnia");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "n", "stopie� pierwiastka");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "liczba pod pierwiastkiem");


	// Logarytmy i eksponenta
	idx = gFunctEngine.add("ln", CalcLn, "logarytm naturalny", "log_e;loge;logarithm_e");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "parametr logarytmu");

	idx = gFunctEngine.add("log2", CalcLog2, "logarytm binarny", "log_2;log_bin;logarithm_2;logarithm_bin");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "parametr logarytmu");
	
	idx = gFunctEngine.add("log10", CalcLog10, "logarytm dziesi�tny", "lg;log_10;log_decimal;logarithm_10;logarithm_decimal");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "parametr logarytmu");

	idx = gFunctEngine.add("log", CalcLog, "logarytm", "logarithm;loga;log_a;logarithm_a;logb;log_b;logarithm_b");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "a", "podstawa logarytmu");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "parametr logarytmu");
	
	idx = gFunctEngine.add("exp", CalcExp, "funkcja wyk�adnicza (eksponenta)", "exponent");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "wyk�adnik eksponenty");


	// Warto�� bezwgl�dna
	idx = gFunctEngine.add("abs", CalcAbs, "warto�� bezwzgl�dna", "absolute");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "prametr warto�ci bezwgl�dnej");

	
	// Funkcja gamma i silnie
	idx = gFunctEngine.add("gamma", CalcGamma, "funkcja gamma (uog�lnienie silnii)");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "parametr funkcji gamma");

	idx = gFunctEngine.add("fact2", CalcDoubleFactorial, "podw�jna silnia", "double_fact;double_factorial;factorial_2;factorial2");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "parametr (liczba rzeczywista)");


	// Dwumian Newtona
	idx = gFunctEngine.add("newton", CalcNewton, "symbol Newtona", "binomial;binomial_coeff");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "n", "moc zbioru");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "k", "liczba element�w do wybrania");


	// B��d standardowy
	idx = gFunctEngine.add("std_err", CalcStdErr, "b��d standardowy", "std_error;standard_error;sigma;std_dev;std_deviation;");
	gFunctEngine[idx].addParam(FunctEngine::Param::Flag("mean;sample"), "rodzaj", "rodzaj b��du standardowego");
	gFunctEngine[idx].addParam(FunctEngine::Param::VaList(2), "...", "kolejne pomiary");
	

	// �rednie
	idx = gFunctEngine.add("mean_ar", CalcArithmeticMean, "�rednia arytmetyczna", "mean;mean_arithmetic;average;avg;ar_mean;arithmetic_mean");
	gFunctEngine[idx].addParam(FunctEngine::Param::VaList(2), "...", "liczby do u�rednienia");

	idx = gFunctEngine.add("mean_geo", CalcGeometricMean, "�rednia geometryczna", "mean_geometric;geo_mean;geometric_mean");
	gFunctEngine[idx].addParam(FunctEngine::Param::VaList(2), "...", "liczby do u�rednienia");

	idx = gFunctEngine.add("mean_harm", CalcHarmonicMean, "�rednia harmoniczna", "mean_harmonic;harm_mean;harmonic_mean");
	gFunctEngine[idx].addParam(FunctEngine::Param::VaList(2), "...", "liczby do u�rednienia");

	idx = gFunctEngine.add("mean_quard", CalcQuadraticMean, "�rednia kwadratowa", "rms;mean_quadratic;quadr_mean;quadratic_mean");
	gFunctEngine[idx].addParam(FunctEngine::Param::VaList(2), "...", "liczby do u�rednienia");
	
	idx = gFunctEngine.add("mean_general", CalcGeneralizedMean, "�rednia pot�gowa", "mean_generalized;general_mean;generalized_mean;mean_gen;gen_mean;power_mean;mean_power;pow_mean;mean_pow");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "k", "rz�d �redniej");
	gFunctEngine[idx].addParam(FunctEngine::Param::VaList(2), "...", "liczby do u�rednienia");

	idx = gFunctEngine.add("med", CalcMedian, "mediana", "median");
	gFunctEngine[idx].addParam(FunctEngine::Param::VaList(2), "...", "liczby do obliczenia mediany");


	// Funkcje zaokr�glaj�ce
	idx = gFunctEngine.add("round", CalcRound, "zaokr�glenie");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "liczba do zaokr�glenia");

	idx = gFunctEngine.add("ceil", CalcCeiling, "zaokr�glenie do g�ry", "ceiling");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "liczba do zaokr�glenia");

	idx = gFunctEngine.add("floor", CalcFloor, "zaokr�glenie w d�");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "liczba do zaokr�glenia");
	
	idx = gFunctEngine.add("trunc", CalcTrunc, "zaokr�glenie w kierunku 0", "truncate");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "liczba do zaokr�glenia");

	idx = gFunctEngine.add("round_n", CalcRoundSignif, "zaokr�glenie do n cyfr znacz�cych", "round_signif;round_significant");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(), "x", "liczba do zaokr�glenia");
	gFunctEngine[idx].addParam(FunctEngine::Param::Value(6), "n", "liczba cyfr znacz�cych");


	// Funkcje dodatkowe
	//gFunctEngine.add("unix_time", nullptr, "czas uniksowy", "unix;unix_timestamp");
}


// Implementacje funkcji
//---------------------------------------------------------------------

// Funkcja pomocnicza, licz�ca okre�lon� funkcj� trygonometryczn�
//---------------------------------------------------------------------
inline double CalcTrigFunct(const std::vector<double> & dParamVector, double(*pTrig)(double))
{
	double dResult;
	switch ((int)(dParamVector[1]))
	{
	case UNIT_RAD:
		dResult = pTrig(dParamVector[0]);
		break;
	case UNIT_DEG:
		dResult = pTrig(dParamVector[0] * M_PI / 180.0);
		break;
	case UNIT_GRAD:
		dResult = pTrig(dParamVector[0] * M_PI / 200.0);
		break;
	default:
		dResult = 0.0;
		break;
	}

	// Zbyt ma�e wyniki traktuj jako 0
	if (abs(dResult) < 1.e-14)
		dResult = 0.0;

	return dResult;
}

// Sinus k�ta (k�t, flaga: jednostka)
//---------------------------------------------------------------------
double CalcSinus(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 2)
		throw std::runtime_error("Implementacja CalcSinus: przekazano z�� liczb� parametr�w");

	return CalcTrigFunct(dParamVector, sin);
}

// Cosinus k�ta (k�t, flaga: jednostka)
//---------------------------------------------------------------------
double CalcCosinus(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 2)
		throw std::runtime_error("Implementacja CalcCosinus: przekazano z�� liczb� parametr�w");

	return CalcTrigFunct(dParamVector, cos);
}

// Tangens k�ta (k�t, flaga: jednostka)
//---------------------------------------------------------------------
double CalcTangens(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 2)
		throw std::runtime_error("Implementacja CalcTangens: przekazano z�� liczb� parametr�w");

	double dResult = CalcTrigFunct(dParamVector, tan);

	// Zapobiegnij zbyt du�emu wynikowi �wiadcz�cemu o parametrze spoza dziedziny
	if (abs(dResult) > 1.e14)
		throw CalcError(CalcError::OUT_OF_DOMAIN, "parametr poza dziedzin� tangensa");

	return dResult;
}

// Cosecans k�ta (k�t, flaga: jednostka)
//---------------------------------------------------------------------
double CalcCosecans(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 2)
		throw std::runtime_error("Implementacja CalcCosecans: przekazano z�� liczb� parametr�w");

	double dResult = CalcTrigFunct(dParamVector, sin);

	// Zapobiegnij zbyt ma�emu wynikowi �wiadcz�cemu o parametrze spoza dziedziny
	if (abs(dResult) < 1.e-14)
		throw CalcError(CalcError::OUT_OF_DOMAIN, "parametr poza dziedzin� cosecansa");

	return 1.0 / dResult;
}

// Secans k�ta (k�t, flaga: jednostka)
//---------------------------------------------------------------------
double CalcSecans(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 2)
		throw std::runtime_error("Implementacja CalcSecans: przekazano z�� liczb� parametr�w");

	double dResult = CalcTrigFunct(dParamVector, cos);

	// Zapobiegnij zbyt ma�emu wynikowi �wiadcz�cemu o parametrze spoza dziedziny
	if (abs(dResult) < 1.e-14)
		throw CalcError(CalcError::OUT_OF_DOMAIN, "parametr poza dziedzin� secansa");

	return 1.0 / dResult;
}

// Cotangens k�ta (k�t, flaga: jednostka)
//---------------------------------------------------------------------
double CalcCotangens(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 2)
		throw std::runtime_error("Implementacja CalcCotangens: przekazano z�� liczb� parametr�w");

	double dResult = CalcTrigFunct(dParamVector, tan);

	// Zapobiegnij zbyt ma�emu wynikowi �wiadcz�cemu o parametrze spoza dziedziny
	if (abs(dResult) < 1.e-14)
		throw CalcError(CalcError::OUT_OF_DOMAIN, "parametr poza dziedzin� cotangensa");
	// Bardzo du�y wynik - cotangens zmierza do 0
	else if (abs(dResult) > 1.e14)
		return 0.0;

	return 1.0 / dResult;
}

// Funkcja pomocnicza, licz�ca okre�lon� funkcj� cyklometryczn�
//---------------------------------------------------------------------
inline double CalcArcFunct(const std::vector<double> & dParamVector, double(*pArc)(double))
{
	switch ((int)(dParamVector[1]))
	{
	case UNIT_RAD:
		return pArc(dParamVector[0]);
		break;
	case UNIT_DEG:
		return pArc(dParamVector[0]) / M_PI * 180.0;
		break;
	case UNIT_GRAD:
		return pArc(dParamVector[0]) / M_PI * 200.0;
		break;
	default:
		return 0.0;
		break;
	}
}

// Arcus sinus k�ta (liczba, flaga: jednostka zwracanego k�ta)
//---------------------------------------------------------------------
double CalcArcSinus(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 2)
		throw std::runtime_error("Implementacja CalcArcSinus: przekazano z�� liczb� parametr�w");

	CalcError::clearFenv();
	double dResult = CalcArcFunct(dParamVector, asin);
	CalcError::checkFenv(FENV_DOMAIN);

	return dResult;
}

// Arcus cosinus k�ta (liczba, flaga: jednostka zwracanego k�ta)
//---------------------------------------------------------------------
double CalcArcCosinus(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 2)
		throw std::runtime_error("Implementacja CalcArcCosinus: przekazano z�� liczb� parametr�w");

	CalcError::clearFenv();
	double dResult = CalcArcFunct(dParamVector, acos);
	CalcError::checkFenv(FENV_DOMAIN);

	return dResult;
}

// Arcus tangens k�ta (liczba, flaga: jednostka zwracanego k�ta)
//---------------------------------------------------------------------
double CalcArcTangens(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 2)
		throw std::runtime_error("Implementacja CalcArcTangens: przekazano z�� liczb� parametr�w");

	return CalcArcFunct(dParamVector, atan);
}

// Arcus tangens k�ta pobieraj�ce wsp�rz�dne punktu na ramieniu k�ta (liczba, flaga: jednostka zwracanego k�ta)
//---------------------------------------------------------------------
double CalcArcTangens2(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 3)
		throw std::runtime_error("Implementacja CalcArcTangens2: przekazano z�� liczb� parametr�w");

	if (dParamVector[0] == 0.0 && dParamVector[1] == 0.0)
		throw CalcError(CalcError::OUT_OF_DOMAIN, "jedna ze wsp�rz�dnych musi by� niezerowa");

	switch ((int)(dParamVector[2]))
	{
	case UNIT_RAD:
		return atan2(dParamVector[0], dParamVector[1]);
		break;
	case UNIT_DEG:
		return atan2(dParamVector[0], dParamVector[1]) / M_PI * 180.0;
		break;
	case UNIT_GRAD:
		return atan2(dParamVector[0], dParamVector[1]) / M_PI * 200.0;
		break;
	default:
		return 0.0;
		break;
	}
}

// Arcus cosecans k�ta (liczba, flaga: jednostka zwracanego k�ta)
//---------------------------------------------------------------------
double CalcArcCosecans(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 2)
		throw std::runtime_error("Implementacja CalcArcCosecans: przekazano z�� liczb� parametr�w");

	CalcError::clearFenv();
	dParamVector[0] = 1 / dParamVector[0];
	double dResult = CalcArcFunct(dParamVector, asin);
	CalcError::checkFenv(FENV_DOMAIN);

	return dResult;
}

// Arcus secans k�ta (liczba, flaga: jednostka zwracanego k�ta)
//---------------------------------------------------------------------
double CalcArcSecans(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 2)
		throw std::runtime_error("Implementacja CalcArcSecans: przekazano z�� liczb� parametr�w");

	CalcError::clearFenv();
	dParamVector[0] = 1 / dParamVector[0];
	double dResult = CalcArcFunct(dParamVector, acos);
	CalcError::checkFenv(FENV_DOMAIN);

	return dResult;
}

// Arcus cotangens k�ta (liczba, flaga: jednostka zwracanego k�ta)
//---------------------------------------------------------------------
double CalcArcCotangens(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 2)
		throw std::runtime_error("Implementacja CalcArcCotangens: przekazano z�� liczb� parametr�w");

	dParamVector[0] = 1 / dParamVector[0];
	return CalcArcFunct(dParamVector, atan);
}

// Maksimum (liczba1, liczba2, ...)
//---------------------------------------------------------------------
double CalcMax(std::vector<double> dParamVector)
{
	if (dParamVector.size() < 2)
		throw std::runtime_error("Implementacja CalcMax: przekazano z�� liczb� parametr�w");

	return *std::max_element(dParamVector.begin(), dParamVector.end());
}

// Minimum (liczba1, liczba2, ...)
//---------------------------------------------------------------------
double CalcMin(std::vector<double> dParamVector)
{
	if (dParamVector.size() < 2)
		throw std::runtime_error("Implementacja CalcMax: przekazano z�� liczb� parametr�w");

	return *std::min_element(dParamVector.begin(), dParamVector.end());
}

// Konwersja na stopnie (k�t, jednostka konwertowanego)
//---------------------------------------------------------------------
double CalcToDeg(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 2)
		throw std::runtime_error("Implementacja CalcToDeg: przekazano z�� liczb� parametr�w");

	switch (int(dParamVector[1]))
	{
	case UNIT_RAD:
		return dParamVector[0] * 180.0 / M_PI;
		break;

	case UNIT_GRAD:
		return dParamVector[0] * 0.9;
		break;

	default:
		return 0.0;
		break;
	}
}

// Konwersja na radiany (k�t, jednostka konwertowanego)
//---------------------------------------------------------------------
double CalcToRad(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 2)
		throw std::runtime_error("Implementacja CalcToRad: przekazano z�� liczb� parametr�w");

	switch (int(dParamVector[1]))
	{
	case UNIT_DEG:
		return dParamVector[0] / 180.0 * M_PI;
		break;

	case UNIT_GRAD:
		return dParamVector[0] / 200.0 * M_PI;
		break;

	default:
		return 0.0;
		break;
	}
}

// Konwersja na gradiusy (k�t, jednostka konwertowanego)
//---------------------------------------------------------------------
double CalcToGrad(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 2)
		throw std::runtime_error("Implementacja CalcToGrad: przekazano z�� liczb� parametr�w");

	switch (int(dParamVector[1]))
	{
	case UNIT_DEG:
		return dParamVector[0] * 200.0 / 180.0;
		break;

	case UNIT_RAD:
		return dParamVector[0] * 200.0 / M_PI;
		break;

	default:
		return 0.0;
		break;
	}
}

// Pot�ga (baza, wyk�adnik)
//---------------------------------------------------------------------
double CalcPow(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 2)
		throw std::runtime_error("Implementacja CalcToRad: przekazano z�� liczb� parametr�w");

	if (dParamVector[0] == 0.0 && dParamVector[1] == 0.0)
		throw CalcError(CalcError::INDETERM, "symbol nieoznaczony 0^0");

	CalcError::clearFenv();
	double dResult = pow(dParamVector[0], dParamVector[1]);

	CalcError::checkFenv(FENV_DOMAIN, "nieokre�lone pot�gowanie");
	CalcError::checkFenv(FENV_DIVBYZERO, "dzielenie przez 0 - ujemny wyk�adnik");
	CalcError::checkFenv(FENV_RANGE);

	return dResult;
}

// Pierwiastek kwadratowy (liczba)
//---------------------------------------------------------------------
double CalcSqrt(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 1)
		throw std::runtime_error("Implementacja CalcSqrt: przekazano z�� liczb� parametr�w");

	if (dParamVector[0] < 0.0)
		throw CalcError(CalcError::NEGATIVE_ROOT, "pierwiastek kwadratowy z liczby ujemnej nie istnieje");

	return std::sqrt(dParamVector[0]);
}

// Pierwiastek n-tego stopnia (stopie�, liczba pod pierwiastkiem)
//---------------------------------------------------------------------
double CalcRoot(std::vector<double> dParamVector)
{
	int		iDegree;
	bool	bIsNegative;

	if (dParamVector.size() != 2)
		throw std::runtime_error("Implementacja CalcRoot: przekazano z�� liczb� parametr�w");

	if (!pkutl::intel_is_int(dParamVector[0]) || (iDegree = int(dParamVector[0])) < 1)
		throw CalcError(CalcError::EXPECTED_POSITIVE_INT, "stopie� pierwiastka musi by� liczb� naturaln� dodatni�");

	bIsNegative = dParamVector[1] < 0.0;
	if (iDegree % 2 == 0 && bIsNegative)
		throw CalcError(CalcError::NEGATIVE_ROOT, "pierwiastek arytmetyczny patrzystego stopnia z liczby ujemnej nie istnieje");

	if (bIsNegative)
		return -std::pow(-dParamVector[1], 1.0 / dParamVector[0]);
	else
		return std::pow(dParamVector[1], 1.0 / dParamVector[0]);
}

// Logarytm naturalny (parametr)
//---------------------------------------------------------------------
double CalcLn(std::vector<double> dParamVector)
{
	double dResult;
	if (dParamVector.size() != 1)
		throw std::runtime_error("Implementacja CalcLn: przekazano z�� liczb� parametr�w");

	CalcError::clearFenv();
	dResult = std::log(dParamVector[0]);
	CalcError::checkFenv(FENV_DOMANDZERO);

	return dResult;
}

// Logarytm binarny (parametr)
//---------------------------------------------------------------------
double CalcLog2(std::vector<double> dParamVector)
{
	double dResult;
	if (dParamVector.size() != 1)
		throw std::runtime_error("Implementacja CalcLog2: przekazano z�� liczb� parametr�w");

	CalcError::clearFenv();
	dResult = std::log2(dParamVector[0]);
	CalcError::checkFenv(FENV_DOMANDZERO);

	return dResult;
}

// Logarytm dziesi�tny (parametr)
//---------------------------------------------------------------------
double CalcLog10(std::vector<double> dParamVector)
{
	double dResult;
	if (dParamVector.size() != 1)
		throw std::runtime_error("Implementacja CalcLog10: przekazano z�� liczb� parametr�w");

	CalcError::clearFenv();
	dResult = std::log10(dParamVector[0]);
	CalcError::checkFenv(FENV_DOMANDZERO);

	return dResult;
}

// Logarytm (podstawa, parametr)
//---------------------------------------------------------------------
double CalcLog(std::vector<double> dParamVector)
{
	double dResult;
	if (dParamVector.size() != 2)
		throw std::runtime_error("Implementacja CalcLog: przekazano z�� liczb� parametr�w");

	// Sprawd�, czy baza nale�y do dziedziny
	if (dParamVector[0] <= 0.0 || dParamVector[0] == 1.0)
		throw CalcError(CalcError::BAD_LOG_BASE, "podstawa logarytmu musi nale�e� do przedzia�u (0; 1) u (1; +inf)");

	CalcError::clearFenv();
	dResult = std::log(dParamVector[1]);
	CalcError::checkFenv(FENV_DOMANDZERO);

	return dResult / std::log(dParamVector[0]);
}

// Eksponenta (wyk�adnik)
//---------------------------------------------------------------------
double CalcExp(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 1)
		throw std::runtime_error("Implementacja CalcExp: przekazano z�� liczb� parametr�w");

	CalcError::clearFenv();
	double dResult = std::exp(dParamVector[0]);
	CalcError::checkFenv(FENV_RANGE);

	return dResult;
}

// Warto�� bezwzgl�dna (parametr)
//---------------------------------------------------------------------
double CalcAbs(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 1)
		throw std::runtime_error("Implementacja CalcAbs: przekazano z�� liczb� parametr�w");

	return std::abs(dParamVector[0]);
}

// Funkcja gamma (parametr)
//---------------------------------------------------------------------
double CalcGamma(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 1)
		throw std::runtime_error("Implementacja CalcGamma: przekazano z�� liczb� parametr�w");

	CalcError::clearFenv();
	double dResult = tgamma(dParamVector[0]);
	CalcError::checkFenv(FENV_DOMANDZERO);
	CalcError::checkFenv(FENV_RANGE);

	return dResult;
}

// Podw�jna silnia
//--------------------------------------------------------------------
double CalcDoubleFactorial(std::vector<double> dParamVector)
{
	double dResult;
	if (dParamVector.size() != 1)
		throw std::runtime_error("Implementacja CalcDoubleFactorial: przekazano z�� liczb� parametr�w");

	// Oblicz i sprawd�, czy nie wyst�pi�y wyj�tki
	CalcError::clearFenv();
	dResult = std::tgamma(dParamVector[0] / 2.0 + 1) * std::exp2(dParamVector[0] / 2.0) * std::pow(M_PI / 2, 0.25 * (std::cos(dParamVector[0] * M_PI) - 1.0));
	CalcError::checkFenv(FENV_DOMANDZERO, "funkcja gamma - parametr poza dziedzin�");
	CalcError::checkFenv(FENV_RANGE);

	return dResult;
}

// Symbol Newtona (moc zbioru, l. el. do wybrania)
//---------------------------------------------------------------------
double CalcNewton(std::vector<double> dParamVector)
{
	double dFactorials[3];
	dFactorials[0] = dParamVector[0];
	dFactorials[1] = dParamVector[0] - dParamVector[1];
	dFactorials[2] = dParamVector[1];

	if (dParamVector.size() != 2)
		throw std::runtime_error("Implementacja CalcNewton: przekazano z�� liczb� parametr�w");

	// Policz odpowienie silnie i testuj b��dy
	for (int i = 0; i < 3; i++)
	{
		CalcError::clearFenv();
		dFactorials[i] = tgamma(dFactorials[i] + 1.0);
		CalcError::checkFenv(FENV_DOMANDZERO, "obliczanie silni - parametr poza dziedzin�");
		CalcError::checkFenv(FENV_RANGE, "obliczanie silni - przekroczono zakres");
	}

	return dFactorials[0] / dFactorials[1] / dFactorials[2];
}

// Odchylenie standardowe (liczby ...)
//---------------------------------------------------------------------
double CalcStdErr(std::vector<double> dParamVector)
{
	double dMean;
	double dResult = 0.0;
	if (dParamVector.size() < 3)
		throw std::runtime_error("Implementacja CalcStdErr: przekazano z�� liczb� parametr�w");

	// Sprawd�, czy wszystkie pomiary s� dodatnie
	std::for_each(dParamVector.begin() + 1, dParamVector.end(), 
		[](double val) { 
			if (val < 0.0)
				throw CalcError(CalcError::EXPECTED_POSITIVE, "wszystkie warto�ci pomiar�w musz� by� dodatnie");
		});

	// Oblicz �redni�
	CalcError::clearFenv();
	dMean = std::accumulate(dParamVector.begin() + 1, dParamVector.end(), 0.0) / (dParamVector.size() - 1);

	// Dodaj kwadraty wszystkich pomiar�w
	for (auto i = dParamVector.begin() + 1; i != dParamVector.end(); i++)
		dResult += (*i - dMean) * (*i - dMean);

	// Sprawd�, czy nie wyst�pi�y b��dy zakresu
	CalcError::checkFenv(FENV_RANGE);

	// Zwr�� odpowiedni rodzaj b��du standardowego
	switch ((int)dParamVector[0])
	{
	case STD_ERR_MEAN:
		return std::sqrt(dResult / (dParamVector.size() - 1) / (dParamVector.size() - 2));
		break;
	case STD_ERR_SAMPLE:
		return std::sqrt(dResult / (dParamVector.size() - 2));
		break;
	default:
		return 0.0;
		break;
	}
}

// Funkcja pomocnicza licz�ca �redni� pot�gow�
//---------------------------------------------------------------------
double CalcGeneralizedMeanHelper(double k, std::vector<double> dParamVector)
{
	double	dMean;
	bool	bSignChanged = false;

	// �rednia geometryczna - wymaga osobnej implementacji
	if (k == 0.0)
	{
		// Oblicz iloczyn pod pierwiastkiem
		CalcError::clearFenv();
		dMean = std::accumulate(dParamVector.begin(), dParamVector.end(), 1.0, std::multiplies<>());
		CalcError::checkFenv(FENV_RANGE, "obliczanie �redniej - przekroczono zakres");

		// Je�li iloczyn ujemny i pierwiastek parzystego stopnia, wyrzu� wyj�tek; je�li nieparzystego, tymczasowo zmie� znak
		if (dMean < 0.0)
		{
			if (!(dParamVector.size() % 2))
				throw CalcError(CalcError::NEGATIVE_ROOT, "obliczanie �redniej - nieokre�lona warto��");
			else
			{
				dMean = -dMean;
				bSignChanged = true;
			}
		}

		// Oblicz �redni� i, je�li trzeba, przywr�� znak
		CalcError::clearFenv();
		dMean = std::pow(dMean, 1.0 / dParamVector.size());
		CalcError::checkFenv(FENV_RANGE, "obliczanie �redniej - przekroczono zakres");
		if (bSignChanged)
			dMean = -dMean;

		return dMean;
	}
	// Pozosta�e �rednie
	else
	{
		// Oblicz sum� pot�g podzielon� przez liczb� element�w
		CalcError::clearFenv();
		dMean = std::accumulate(dParamVector.begin(), dParamVector.end(), 0.0,
			[k](double sum, double val) {
				return sum + std::pow(val, k);
			}) / dParamVector.size();
		CalcError::checkFenv(FENV_RANGE, "obliczanie �redniej - przekroczono zakres");
		CalcError::checkFenv(FENV_DOMAIN, "obliczanie �redniej - nieokre�lona warto��");
		CalcError::checkFenv(FENV_DIVBYZERO, "obliczanie �redniej - dzielenie przez 0");

		// Je�li stopie� jest nieparzysty (obliczamy pierwiastek nieparzystego stopnia lub jego odwrotno��) i suma ujemna,
		// tymczasowo zmie� znak
		if (pkutl::intel_is_int(k) && int(k) % 2 && dMean < 0.0)
		{
			dMean = -dMean;
			bSignChanged = true;
		}

		// Oblicz �redni� i ewentualnie przywr�� znak
		CalcError::clearFenv();
		dMean = std::pow(dMean, 1.0 / k);
		CalcError::checkFenv(FENV_RANGE, "obliczanie �redniej - przekroczono zakres");
		if (bSignChanged)
			dMean = -dMean;

		return dMean;
	}
}

// �rednia arytmetyczna (liczby ...)
//---------------------------------------------------------------------
double CalcArithmeticMean(std::vector<double> dParamVector)
{
	if (dParamVector.size() < 2)
		throw std::runtime_error("Implementacja CalcArithmeticMean: przekazano z�� liczb� parametr�w");

	return CalcGeneralizedMeanHelper(1, dParamVector);
}

// �rednia geometryczna (liczby ...)
//---------------------------------------------------------------------
double CalcGeometricMean(std::vector<double> dParamVector)
{
	if (dParamVector.size() < 2)
		throw std::runtime_error("Implementacja CalcGeometricMean: przekazano z�� liczb� parametr�w");

	return CalcGeneralizedMeanHelper(0, dParamVector);
}

// �rednia harmoniczna (liczby ...)
//---------------------------------------------------------------------
double CalcHarmonicMean(std::vector<double> dParamVector)
{
	if (dParamVector.size() < 2)
		throw std::runtime_error("Implementacja CalcHarmomicMean: przekazano z�� liczb� parametr�w");

	return CalcGeneralizedMeanHelper(-1, dParamVector);
}

// �rednia kwadratowa (liczby ...)
//---------------------------------------------------------------------
double CalcQuadraticMean(std::vector<double> dParamVector)
{
	if (dParamVector.size() < 2)
		throw std::runtime_error("Implementacja CalcGeomatricMean: przekazano z�� liczb� parametr�w");

	return CalcGeneralizedMeanHelper(2, dParamVector);
}

// �redna pot�gowa (stopie�, liczby ...)
//---------------------------------------------------------------------
double CalcGeneralizedMean(std::vector<double> dParamVector)
{
	if (dParamVector.size() < 3)
		throw std::runtime_error("Implementacja CalcGeneralizedMean: przekazano z�� liczb� parametr�w");

	return CalcGeneralizedMeanHelper(dParamVector[0], std::vector<double>{ dParamVector.begin() + 1, dParamVector.end() });
}

// Mediana (liczby ...)
//---------------------------------------------------------------------
double CalcMedian(std::vector<double> dParamVector)
{
	if (dParamVector.size() < 2)
		throw std::runtime_error("Implementacja CalcMedian: przekazano z�� liczb� parametr�w");

	std::sort(dParamVector.begin(), dParamVector.end());
	if (dParamVector.size() % 2)
		return dParamVector[(dParamVector.size() - 1) / 2];
	else
		return (dParamVector[dParamVector.size() / 2 - 1] + dParamVector[dParamVector.size() / 2]) / 2.0;
}

// Zaokr�glenie (liczba)
//---------------------------------------------------------------------
double CalcRound(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 1)
		throw std::runtime_error("Implementacja CalcRound: przekazano z�� liczb� parametr�w");

	return std::round(dParamVector[0]);
}

// Zaokr�glenie w g�r� (liczba)
//---------------------------------------------------------------------
double CalcCeiling(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 1)
		throw std::runtime_error("Implementacja CalcCeiling: przekazano z�� liczb� parametr�w");

	return std::ceil(dParamVector[0]);
}

// Zaokr�glenie w d� (liczba)
//---------------------------------------------------------------------
double CalcFloor(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 1)
		throw std::runtime_error("Implementacja CalcFloor: przekazano z�� liczb� parametr�w");

	return std::floor(dParamVector[0]);
}

// Zaokr�glenie w stron� zera (liczba)
//---------------------------------------------------------------------
double CalcTrunc(std::vector<double> dParamVector)
{
	if (dParamVector.size() != 1)
		throw std::runtime_error("Implementacja CalcTrunc: przekazano z�� liczb� parametr�w");

	return std::trunc(dParamVector[0]);
}

// Zaokr�glenie do n cyfr znacz�cych (liczba, n)
//---------------------------------------------------------------------
double CalcRoundSignif(std::vector<double> dParamVector)
{
	unsigned short	usSignif;
	double			dReturn;

	if (dParamVector.size() != 2)
		throw std::runtime_error("Implementacja CalcRoundSignif: przekazano z�� liczb� parametr�w");

	if (dParamVector[1] <= 0.0 || !pkutl::intel_is_int(dParamVector[1]))
		throw CalcError(CalcError::EXPECTED_POSITIVE_INT, "liczba cyfr znacz�cych musi by� liczb� ca�kowit� dodatni�");

	// Zabezpiecz przed zbyt du�� liczb� cyfr
	usSignif = (unsigned short)dParamVector[1];
	if (usSignif > _MAX_PREC)
		usSignif = _MAX_PREC;

	CalcError::clearFenv();
	dReturn = pkutl::round_signif(dParamVector[0], usSignif);
	CalcError::checkFenv(FENV_RANGE, "podczas zaokr�glania przekroczono zakres");
	return dReturn;
}