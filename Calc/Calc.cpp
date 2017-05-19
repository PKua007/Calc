#include "stdafx.h"

#include "newconio.h"
#include "main_func.h"
#include "vars_and_functs.h"

using namespace std;

int main()
{
	{
		// Inicjacja konsoli - tryb niepe�noekranowy, zablokowana zmiana rozmiaru okna
		_cinit(false, true);
		SetConsoleTitleA("Kalkulator");

		// Utw�rz zmienne i funkcje
		CreateVarsAndFuncts();

		// Wy�wietl info startowe
		ShowStartInfo();

		// Obs�u� program
		ProceedMain();

		// Zresetuj kolor i wyczy�� ekran. Mo�e si� przyda� przyda�, je�li wywo�ano np. z cmd
		ResetColor();
		Clrscr();
	}
    return 0;
}