/* rarjpegparser.c -- парсер rarjpeg файла */
#include "rarjpegparser.h"
#include "zipparser.h"
#define SOI_FB 0XFF     // первый байт заголовка jpeg файла
#define SOI_SB 0XD8     // второй байт заголовка jpeg файла
#define EOI_FB 0XFF     // первый байт хвостовика jpeg файла
#define EOI_SB 0XD9     // второй байт хвостовика jpeg файла
void pasrerarjpeg(const char *filepath)
{
    FILE * file;

    if ((file = fopen(filepath, "rb")) != NULL)
    {
#ifdef _WIN32
        printf("Файл \"%s\" найден. Размер %llu \n", filepath, getFileSize(filepath));
#else
        printf("Файл \"%s\" найден. Размер %lu \n", filepath, getFileSize(filepath));
#endif
        int currentchar;
        int index = 0;
        int prevChar = 0;
        int jpgIsEnd = 0;

        while (!feof(file))
        {
            if (jpgIsEnd)
            {
                printf("Файл содержит что-то большее, чем один jpg\n");
                index = ftell(file);

                //struct ZIP_File zip_file =
                    getZipFile(filepath, index);
                break;
            }
            else
            { currentchar = getc(file); }

            if (prevChar == EOI_FB && currentchar == EOI_SB)
            {
                printf("Конец данных JPG файла, строка %d\n", index);
                jpgIsEnd = 1;
            }
            else if (prevChar == SOI_FB && currentchar == SOI_SB)
            { printf("Файл начинается как JPG\n"); }

            prevChar = currentchar;
        }
        fclose(file);
    }
	else
    { printf("Файл \"%s\" не найден.\n", filepath); }
}
