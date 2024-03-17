#include <stdio.h>
#include "rarjpegparser.h"

#ifdef _WIN32
//#include <locale.h>       //  здесь "живёт" setlocale(LC_ALL, "rus");
#include <Windows.h>
#endif
#define DEBUG

int main(int argc, char* argv[])
{
#ifdef _WIN32
	//setlocale(LC_ALL, "rus");
	SetConsoleCP(1251); //установка кодовой страницы win-cp 1251 в поток ввода
	SetConsoleOutputCP(1251); //установка кодовой страницы win-cp 1251 в поток вывода
#endif

    if (argc > 1)
	{
		for (int i = 0; i < argc; i++)
		{ printf("Параметр № %d %s\n", i, argv[i]); }
        pasrerarjpeg(argv[1]);
	}

    return 0;
}
