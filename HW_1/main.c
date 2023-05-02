#include <stdio.h>
#include <locale.h>       //  здесь "живёт" setlocale(LC_ALL, "rus");
#include "zipparser.h"

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "rus");

	if(argc > 1)
	{
		for(int i = 0; i < argc; i++)
		{
			printf("Параметр № %d %s\n", i, argv[i]);
		}

		FILE *file;

		if((file = fopen(argv[1], "rb")) != NULL)
		{
#ifdef _WIN32
			printf("Файл \"%s\" найден. Размер %llu \n", argv[1], getFileSize(argv[1]));
#else
			printf("Файл \"%s\" найден. Размер %lu \n", argv[1], getFileSize(argv[1]));
#endif
			
			int c;
			int index = 0;
			int prevChar = 0;
			int jpgIsEnd = 0;

			while((c = getc(file)) != EOF && (++index - 1) < (4 * 4 * 100000))
			{
				if(jpgIsEnd)
				{
					printf("Файл содержит что-то большее, чем один jpg\n");
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
					printf("\nКонец данных JPG файла, строка %d \n", index);
					jpgIsEnd = 1;
					//break;
				}
				else if(prevChar == 0XFF && c == 0XD8)
				{
					printf("Файл начинается как JPG\n");
				}
				
				prevChar = c;
			}
			fclose(file);
		}
		else
		{
			printf("Файл  \"%s\" не найден.\n", argv[1]);
		}
	}

	return 0;
}
