#include "stdafx.h"

#include "newconio.h"
#include "main_func.h"
#include "vars_and_functs.h"

using namespace std;

int main()
{
	{
		// Inicjacja konsoli - tryb niepe³noekranowy, zablokowana zmiana rozmiaru okna
		_cinit(false, true);
		SetConsoleTitleA("Kalkulator");

		// Utwórz zmienne i funkcje
		CreateVarsAndFuncts();

		// Wyœwietl info startowe
		ShowStartInfo();

		// Obs³u¿ program
		ProceedMain();

		// Zresetuj kolor i wyczyœæ ekran. Mo¿e siê przydaæ przydaæ, jeœli wywo³ano np. z cmd
		ResetColor();
		Clrscr();
	}
    return 0;
}