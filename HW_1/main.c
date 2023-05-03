#include <stdio.h>
#include "zipparser.h"

#ifdef _WIN32
	//#include <locale.h>       //  ����� "����" setlocale(LC_ALL, "rus");
	#include <Windows.h>
#endif

int main(int argc, char *argv[])
{
#ifdef _WIN32
	//setlocale(LC_ALL, "rus");
	SetConsoleCP(1251); //��������� ������� �������� win-cp 1251 � ����� �����
	SetConsoleOutputCP(1251); //��������� ������� �������� win-cp 1251 � ����� ������
#endif

	if(argc > 1)
	{
		for(int i = 0; i < argc; i++)
		{
			printf("�������� � %d %s\n", i, argv[i]);
		}

		FILE *file;

		if((file = fopen(argv[1], "rb")) != NULL)
		{
#ifdef _WIN32
			printf("���� \"%s\" ������. ������ %llu \n", argv[1], getFileSize(argv[1]));
#else
			printf("���� \"%s\" ������. ������ %lu \n", argv[1], getFileSize(argv[1]));
#endif
			
			int c;
			int index = 0;
			int prevChar = 0;
			int jpgIsEnd = 0;

			while((c = getc(file)) != EOF && (++index - 1) < (4 * 4 * 100000))
			{
				if(jpgIsEnd)
				{
					printf("���� �������� ���-�� �������, ��� ���� jpg\n");
					struct ZIP_File zip_file = getZipFile(argv[1], index);
					break;
				}

				/*
				printf("%2X ", c);
				if(index % 16 == 0)
				{
					printf("\n");
				}
				else if(index % 4 == 0)
				{
					printf("  ");
				}
				*/

				if(prevChar == 0XFF && c == 0XD9)
				{
					printf("\n����� ������ JPG �����, ������ %d \n", index);
					jpgIsEnd = 1;
					//break;
				}
				else if(prevChar == 0XFF && c == 0XD8)
				{
					printf("���� ���������� ��� JPG\n");
				}
				
				prevChar = c;
			}
			fclose(file);
		}
		else
		{
			printf("����  \"%s\" �� ������.\n", argv[1]);
		}
	}

	return 0;
}
