#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "zipparser.h"

// end of central dir signature
#define EOCD_Signature 0x06054b50
// central file header signature
#define CDFH_Signature 0x02014b50

// End of central directory record
struct EOCD {
	// end of central dir signature -> 4 bytes  (0x06054b50)
	uint32_t signature;
	// number of this disk -> 2 bytes
	uint16_t diskNumber;
	// number of the disk with the
	// start of the central directory -> 2 bytes
	uint16_t startDiskNumber;
	// total number of entries in the
	// central directory on this disk -> 2 bytes
	uint16_t numberCentralDirectoryRecord;
	// total number of entries in
	// the central directory -> 2 bytes
	uint16_t totalCehtralDirectoryRecord;
	// size of the central directory -> 4 bytes
	uint32_t sizeOfCentralDirectory;
	// offset of start of central
	// directory with respect to
	// the starting disk number -> 4 bytes
	uint32_t centralDirectoryOffset;
	// .ZIP file comment length -> 2 bytes
	uint16_t commentLength;
	// .ZIP file comment -> (variable size)
} __attribute__((packed));

/*
	[central directory header 1]
	.
	.
	.
	[central directory header n]
	[digital signature]

	File header:
*/
struct CentralDirectoryFileHeader
{
	// central file header signature -> 4 bytes  (0x02014b50)
    uint32_t signature;
	// version made by -> 2 bytes	
    uint16_t versionMadeBy;
	// version needed to extract -> 2 bytes	
    uint16_t versionToExtract;
	// general purpose bit flag -> 2 bytes
    uint16_t generalPurposeBitFlag;
	// compression method -> 2 bytes
    uint16_t compressionMethod;
	// last mod file time -> 2 bytes
    uint16_t modificationTime;
	// last mod file date -> 2 bytes
    uint16_t modificationDate;
	// crc - 32 -> 4 bytes
    uint32_t crc32;
	// compressed size -> 4 bytes
    uint32_t compressedSize;
	// uncompressed size -> 4 bytes
    uint32_t uncompressedSize;
	// file name length -> 2 bytes
    uint16_t filenameLength;
	// extra field length -> 2 bytes
    uint16_t extraFieldLength;
	// file comment length -> 2 bytes
    uint16_t fileCommentLength;
	// disk number start -> 2 bytes
    uint16_t diskNumber;
	// internal file attributes -> 2 bytes
    uint16_t internalFileAttributes;
	// external file attributes -> 4 bytes
    uint32_t externalFileAttributes;
	// relative offset of local header -> 4 bytes
    uint32_t localFileHeaderOffset; 
	// file name (variable size)
	// extra field(variable size)
	// file comment(variable size)
} __attribute__((packed));

// 
struct LocalFileHeader
{
    uint32_t signature;
    uint16_t versionToExtract;
    uint16_t generalPurposeBitFlag;
    uint16_t compressionMethod;
    uint16_t modificationTime;
    uint16_t modificationDate;
    uint32_t crc32;
    uint32_t compressedSize;
    uint32_t uncompressedSize;
    uint16_t filenameLength;
    uint16_t extraFieldLength; 
} __attribute__((packed));

struct ZIP_File getZipFile(const char* file_name, int startPosition) {
	FILE* file = fopen(file_name, "rb");
	if(file == NULL) {
	}
	else
	{
		/*
		char * buffer = (char*)malloc(sizeof(char));
		if(buffer == NULL){
			printf("Error malloc");
		}
		fseek(file, -sizeof(buffer), SEEK_END);

		for(int i = 0; i < 1000; i++) {
			fread(buffer, sizeof(buffer), 1, file);
			printf("%s", buffer);
			fseek(file, -2*sizeof(buffer), SEEK_CUR);
		}
		*/

		uint64_t file_size = getFileSize(file_name);
		uint32_t signature = 0; 

		// Ищем сигнатуру EOCD
		// сначала делаем смещение в конец файла,
		// недоходя до конца на размер signature
		fseek(file, -sizeof(struct EOCD), SEEK_END);

		for (uint64_t offset = file_size - sizeof(signature); offset != 0; --offset) {
		    // Считываем четыре байта сигнатуры
			fread((char *)&signature, sizeof(signature), 1, file);
			
			if (EOCD_Signature == signature) {
				// Есть контакт!
				printf("Нашли сигнатуру EOCD.");
				struct EOCD eocd;
				
				// Возвращение позиции курсора на точку считывания 
				// структуры EOCD
				fseek(file, -1 * (sizeof(signature)), SEEK_CUR);
				
				// Считываем структуру EOCD
				fread((char *)&eocd, sizeof(eocd), 1, file);
				
				printf("eocd.commentLength = %d", eocd.commentLength);			
				
				if(eocd.commentLength) {
					// Массив для комментария
					uint8_t comment[eocd.commentLength + 1];

					// Инициализация массива
					initIntArray(comment, eocd.commentLength + 1);

					fread((char *)comment, eocd.commentLength, 1, file);
					comment[eocd.commentLength] = 0; 
					printf("%s, это комментарии\n", (char *)comment);
				}

				if(eocd.centralDirectoryOffset) {
					// Смещение курсора на позицию первой записи
					fseek(file, eocd.centralDirectoryOffset, SEEK_SET);
					struct CentralDirectoryFileHeader cdfh;
					// Чтение структуры CentralDirectoryFileHeader
					fread((char *)&cdfh, sizeof(cdfh), 1, file);

					if(CDFH_Signature != cdfh.signature) {
						// Ошибка
						printf("Ошибка определения cdfh.signature (получили: 0x%.8X вместо: 0x%.8X)\n", cdfh.signature, CDFH_Signature);
						break;
					}

					// Считываем имя файла / папки
					if(cdfh.filenameLength) {
						uint8_t filename[cdfh.filenameLength + 1];
						
						// Инициализация массива
						initIntArray(filename, cdfh.filenameLength + 1);

						// Попытка считать название файла
						fread((char *)filename, cdfh.filenameLength, 1, file);
						
						// Заполнение последнего элемента массива
						filename[cdfh.filenameLength] = 0;
						
						printf("%s,", (char *)filename);
					}
				}
				break;
			}
			else
			{
				// Если подпись не найдена смещаемся на один байт
				// от предыдущей позиции в сторону начала файла
				fseek(file, -1 * (sizeof(signature) + 1), SEEK_CUR);
			}
		}

		fclose(file);
	}

	struct ZIP_File result = {NULL, 0, 0};
	return result;
}

// Определение размера файла
uint64_t getFileSize(const char* file_name) {
	uint64_t file_size = 0;

	FILE* file = fopen(file_name, "rb");
	
	if(file == NULL) {
		file_size = -1;
	}
	else {
		while(getc(file) != EOF) {
			++file_size;
		}
		fclose(file);
	}
	return file_size;
}

// Инициализация массива
void initIntArray(uint8_t* arr, int length) {
	for (int i = 0; i < length; ++i) {
		arr[i] = 0;
	}
}
