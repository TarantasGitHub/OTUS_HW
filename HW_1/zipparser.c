#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "zipparser.h"
#include "binaryitem.h"

// end of central dir signature
#define EOCD_Signature 0x06054b50
// central file header signature
#define CDFH_Signature 0x02014b50
// local file header signature
#define LFH_Signature 0x04034b50

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
}
#ifdef __unix__
__attribute__((packed))
#endif
;

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
}
#ifdef __unix__
__attribute__((packed))
#endif
;

// 
struct LocalFileHeader
{
	uint32_t signature;				// Подпись
	uint16_t versionToExtract;		// Версия
	uint16_t generalPurposeBitFlag;	// Флаги
	uint16_t compressionMethod;		// Метод сжатия
	uint16_t modificationTime;		// Время модификации файла 
	uint16_t modificationDate;		// Дата изменения файла
	uint32_t crc32;					// Контрольная сумма Crc-32
	uint32_t compressedSize;		// Сжатый размер
	uint32_t uncompressedSize;		// Несжатый размер
	uint16_t filenameLength;		// Длина имени файла
	uint16_t extraFieldLength;		// Длина дополнительного поля
	//char* filename;				// Имя файла
	//								// Дополнительное поле
}
#ifdef __unix__
__attribute__((packed))
#endif
;

void initIntArray(uint8_t* arr, int length);

long int indexOf(FILE* file, uint32_t lfh_signature_for_search, long int startPosition);

struct ZIP_File getZipFile(const char* file_name, long int startPosition) {

	FILE* file = fopen(file_name, "rb");

	if (file == NULL) {
	}
	else
	{
		int c = 0, counter = 0;
		//uint32_t lfh_signature = 0;
		uint32_t lfh_signature_for_search = LFH_Signature;

		startPosition = indexOf(file, lfh_signature_for_search, startPosition);

		//startPosition -= sizeof(lfh_signature);
		//fseek(file, startPosition, SEEK_SET);
		//printf("Начали поиск начала ZIP файла, (%ld)\n", startPosition);

		//while (c != EOF && (c = getc(file)) != EOF) {
		//	++startPosition;
		//	if (c == ((char*)&lfh_signature_for_search)[0]) {
		//		int find = 1;
		//		for (int i = 1; i < (int)sizeof(lfh_signature_for_search); ++i) {
		//			++startPosition;
		//			if ((c = getc(file)) == EOF || ((char*)&lfh_signature_for_search)[i] != c) {
		//				find = 0;
		//				break;
		//			}
		//		}

		//		if (find) {
		//			// Возвращаемся в начало ZIP
		//			fseek(file, (-1) * sizeof(lfh_signature_for_search), SEEK_CUR);
		//			startPosition -= sizeof(lfh_signature_for_search);
		//			printf("Нашли начало ZIP файла, (%ld)\n", startPosition);
		//			break;
		//		}
		//	}

		//	//for (int i = 1; i < (int)sizeof(lfh_signature); ++i) {
		//	//	((char*)&lfh_signature)[i - 1] = ((char*)&lfh_signature)[i];
		//	//}
		//	//((char*)&lfh_signature)[3] = c;
		//	//++startPosition;
		//	//if (counter++ < 100) {
		//	//	printf("%ld --+-- LFH_Signature:0x%.8X --- 0x%.8X\n", startPosition, LFH_Signature, lfh_signature);
		//	//}

		//	//if (LFH_Signature == lfh_signature) {
		//	//	// Возвращаемся в начало ZIP
		//	//	fseek(file, (-1) * sizeof(lfh_signature), SEEK_CUR);
		//	//	startPosition -= sizeof(lfh_signature);
		//	//	printf("Нашли начало ZIP файла, (%ld)\n", startPosition);
		//	//	break;
		//	//}
		//}
		struct TempReader tr = getTRReader(sizeof(uint32_t));

		uint32_t lfh_signature = LFH_Signature;
		long int lfh_position = indexOf(file, lfh_signature, startPosition);
		
		if (lfh_position > 0) {
			struct LocalFileHeader* lfh_struct = (struct LocalFileHeader*)malloc(sizeof(struct LocalFileHeader));
			int fileIndex = 1;
			while (lfh_position > 0) {
				printf("Нашли %dй LFH заголовок с помощью indexOf, (%u)\n", fileIndex++, lfh_position);
				fseek(file, lfh_position, SEEK_SET);
				fread((char*)lfh_struct, sizeof(struct LocalFileHeader), 1, file);
				char* filename = (char*)malloc(lfh_struct->filenameLength * sizeof(char));
				fread(filename, lfh_struct->filenameLength, 1, file);
				printf("Длина имени файла: %u, имя: \"%s\"\n", lfh_struct->filenameLength, filename);
				lfh_position = indexOf(file, lfh_signature, lfh_position + sizeof(struct LocalFileHeader));
			}
		}

		uint32_t cdfh_signature = CDFH_Signature;
		long int cdfh_position = indexOf(file, cdfh_signature, startPosition);
		printf("Нашли первый CDFH заголовок с помощью indexOf, (%u)\n", cdfh_position);
		uint32_t eocd_signature = EOCD_Signature;
		long int eocd_position = indexOf(file, eocd_signature, startPosition);
		printf("Нашли первый EOCD заголовок с помощью indexOf, (%u)\n", eocd_position);
		
		fseek(file, 0L, SEEK_SET);
		int _c = 0, lfh_notfound = 1, cdfh_notfound = 1;
		uint32_t result = 0;
		uint32_t lfh_offset = 0, cdfh_offset = 0;
		while ((_c = getc(file)) != EOF) {
			addTRItem(&tr, _c);
			getTRValue(&tr, (char*)&result);
			if (lfh_notfound) {
				++lfh_offset;
			}
			if (cdfh_notfound) {
				++cdfh_offset;
			}
			if (lfh_notfound && lfh_offset > 30400 && lfh_offset < 30600) {
				printf("%u -- LFH_Signature:0x%.8X --- 0x%.8X\n", lfh_offset, LFH_Signature, result);
			}


			if (lfh_notfound && LFH_Signature == result) {
				lfh_offset -= sizeof(result);
				lfh_notfound = 0;
				printf("Нашли начало ZIP файла с помощью структуры, (%u)\n", lfh_offset);
			}
			else	if (cdfh_notfound && CDFH_Signature == result) {
				cdfh_offset -= sizeof(result);
				cdfh_notfound = 0;
				printf("Нашли первый CDFH заголовок с помощью структуры, (%u)\n", cdfh_offset);
			}
			if (!lfh_notfound && !cdfh_notfound) {
				break;
			}

		}

		/*
		char * buffer = (char*)malloc(sizeof(char));

		if (buffer == NULL){
			printf("Error malloc");
		}

		fseek(file, -sizeof(buffer), SEEK_END);

		for (int i = 0; i < 1000; i++) {
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
		fseek(file, (-1) * sizeof(struct EOCD), SEEK_END);

		for (uint64_t offset = file_size - sizeof(signature); offset != 0; --offset) {
			// Считываем четыре байта сигнатуры
			fread((char*)&signature, sizeof(signature), 1, file);

			if (EOCD_Signature == signature) {
				// Есть контакт!
				printf("Нашли сигнатуру EOCD.\n");
				struct EOCD eocd;

				// Возвращение позиции курсора на точку считывания 
				// структуры EOCD
				fseek(file, -1 * (sizeof(signature)), SEEK_CUR);

				// Считываем структуру EOCD
				fread((char*)&eocd, sizeof(eocd), 1, file);

				if (EOCD_Signature == eocd.signature) {
					printf("EOCD считался правильно.\n");
				}
				else
				{
					printf("EOCD считался не правильно.\n");
				}

				printf("eocd.commentLength: %d\n", eocd.commentLength);

				if (eocd.commentLength) {
					// Массив для комментария
					uint8_t* comment = malloc((eocd.commentLength + 1) * sizeof(uint8_t));
					if (comment != NULL) {
						// Инициализация массива
						initIntArray(comment, eocd.commentLength + 1);

						fread((char*)comment, eocd.commentLength, 1, file);
						comment[eocd.commentLength] = 0;
						printf("%s, это комментарии\n", (char*)comment);
					}
				}

				struct CentralDirectoryFileHeader cdfh;

				if (eocd.centralDirectoryOffset) {

					int seek[3] = { SEEK_CUR, SEEK_SET, SEEK_END };

					for (int i = 1; i < 2; ++i) {
						// Смещение курсора на позицию первой записи
						long int offset = (seek[i] == SEEK_END ? (-1) : 1) * (long int)eocd.centralDirectoryOffset;
						offset += startPosition;
						offset -= 111;
						printf("SEEK: %d, offset: %ld\n", seek[i], offset);
						fseek(file, cdfh_offset /*-offset*/, seek[i]);

						// Чтение структуры CentralDirectoryFileHeader
						fread((char*)&cdfh, sizeof(cdfh), 1, file);

						if (CDFH_Signature != cdfh.signature) {
							// Ошибка
							printf("Ошибка определения cdfh.signature (получили: 0x%.8X вместо: 0x%.8X)\n",
								cdfh.signature, CDFH_Signature);
							// break;
						}
						else {
							printf("Определили cdfh.signature (получили: 0x%.8X вместо: 0x%.8X)\n",
								cdfh.signature, CDFH_Signature);
						}

					}

					uint32_t sign = 0;
					uint32_t sign_op = 0;

					int c;
					uint32_t index = 0;

					fseek(file, 0L, SEEK_CUR);
					// rewind(file);

					while ((c = getc(file)) != EOF) {
						for (int i = 1; i < 4; ++i) {
							((char*)&sign)[i - 1] = ((char*)&sign)[i];
							((char*)&sign_op)[4 - i] = ((char*)&sign_op)[4 - (i + 1)];

						}
						((char*)&sign)[3] = c;
						((char*)&sign_op)[0] = c;

						if (CDFH_Signature == sign || CDFH_Signature == sign_op) {
							printf("%d, CDFH_Signature is 0x%.8x, sign is 0x%.8X, sign_op is 0x%.8X \n", index, CDFH_Signature, sign, sign_op);
						}
						else if (EOCD_Signature == sign || EOCD_Signature == sign_op) {
							printf("EOCD_Signature in %d\n", index);
						}
						else
						{
							if (index < 40) {
								printf("0x%.8X --- 0x%.8X\n", sign, sign_op);
							}
						}
						++index;
					}
					printf("Закончили на %u итерации\n", index);

					// Считываем имя файла / папки
					if (cdfh.filenameLength) {
						uint8_t* filename = malloc((cdfh.filenameLength + 1) * sizeof(uint8_t));
						if (filename != NULL) {
							// Инициализация массива
							initIntArray(filename, cdfh.filenameLength + 1);

							// Попытка считать название файла
							fread((char*)filename, cdfh.filenameLength, 1, file);

							// Заполнение последнего элемента массива
							filename[cdfh.filenameLength] = 0;

							printf("%s,", (char*)filename);
						}
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

	struct ZIP_File result = { NULL, 0, 0 };
	return result;
}

// Определение размера файла
uint64_t getFileSize(const char* file_name) {
	uint64_t file_size = 0;

	FILE* file = fopen(file_name, "rb");

	if (file == NULL) {
		file_size = -1;
	}
	else
	{
		while (getc(file) != EOF) {
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

// Поиск вхождения
long int indexOf(FILE* file, uint32_t lfh_signature_for_search, long int startPosition) {
	char c = 0;
	// Запомнили где был курсор
	long int oldPosition = ftell(file);
	fseek(file, startPosition, SEEK_SET);
	// printf("Начали поиск начала вхождения маски, (%ld)\n", startPosition);
	startPosition = -1;

	while (!feof(file)) {
		
		c = getc(file);

		if (c == ((char*)&lfh_signature_for_search)[0]) {
			int find = 1;
			for (int i = 1; i < (int)sizeof(lfh_signature_for_search); ++i) {

				if (feof(file) || ((c = getc(file)) != ((char*)&lfh_signature_for_search)[i])) {
					find = 0;
					fseek(file, (-1) * (i - 1), SEEK_CUR);
					break;
				}
			}

			if (find) {
				// Возвращаемся в начало ZIP
				fseek(file, (-1) * sizeof(lfh_signature_for_search), SEEK_CUR);
				startPosition = ftell(file);
				// printf("Нашли начало вхождения маски, (%ld)\n", startPosition);
				break;
			}
		}
	}
	// Вернули курсор на то место, где он был изначально
	fseek(file, oldPosition, SEEK_SET);
	return startPosition;
}
