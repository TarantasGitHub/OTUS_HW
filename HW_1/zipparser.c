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
}
#ifdef __unix__
	__attribute__((packed))
#endif
		;

struct ZIP_File getZipFile(const char* file_name, long int startPosition) {

	FILE* file = fopen(file_name, "rb");

	if (file == NULL) {
	}
	else
	{
		int c = 0, counter = 0;
		uint32_t lfh_signature = 0;
		startPosition -= sizeof(lfh_signature);
		fseek(file, startPosition, SEEK_SET);
		printf("������ ����� ������ ZIP �����, (%ld)\n", startPosition);

		while ((c = getc(file)) != EOF) {
			for (int i = 1; i < (int)sizeof(lfh_signature); ++i) {
				((char*)&lfh_signature)[i - 1] = ((char*)&lfh_signature)[i];
			}
			((char*)&lfh_signature)[3] = c;
			++startPosition;
			if (counter++ < 100) {
				printf("%ld --+-- LFH_Signature:0x%.8X --- 0x%.8X\n", startPosition, LFH_Signature, lfh_signature);
			}

			if (LFH_Signature == lfh_signature) {
				// ������������ � ������ ZIP
				fseek(file, (-1) * sizeof(lfh_signature), SEEK_CUR);
				startPosition -= sizeof(lfh_signature);
				printf("����� ������ ZIP �����, (%ld)\n", startPosition);
				break;
			}
		}
		struct TempReader tr = getTRReader(sizeof(uint32_t));

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
				printf("����� ������ ZIP ����� � ������� ���������, (%u)\n", lfh_offset);
			}
			else	if (cdfh_notfound && CDFH_Signature == result) {
				cdfh_offset -= sizeof(result);
				cdfh_notfound = 0;
				printf("����� ������ CDFH ��������� � ������� ���������, (%u)\n", cdfh_offset);
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

		// ���� ��������� EOCD
		// ������� ������ �������� � ����� �����,
		// �������� �� ����� �� ������ signature
		fseek(file, (-1) * sizeof(struct EOCD), SEEK_END);

		for (uint64_t offset = file_size - sizeof(signature); offset != 0; --offset) {
			// ��������� ������ ����� ���������
			fread((char*)&signature, sizeof(signature), 1, file);

			if (EOCD_Signature == signature) {
				// ���� �������!
				printf("����� ��������� EOCD.\n");
				struct EOCD eocd;

				// ����������� ������� ������� �� ����� ���������� 
				// ��������� EOCD
				fseek(file, -1 * (sizeof(signature)), SEEK_CUR);

				// ��������� ��������� EOCD
				fread((char*)&eocd, sizeof(eocd), 1, file);

				if (EOCD_Signature == eocd.signature) {
					printf("EOCD �������� ���������.\n");
				}
				else
				{
					printf("EOCD �������� �� ���������.\n");
				}

				printf("eocd.commentLength: %d\n", eocd.commentLength);

				if (eocd.commentLength) {
					// ������ ��� �����������
					uint8_t* comment = malloc((eocd.commentLength + 1) * sizeof(uint8_t));
					if (comment != NULL) {
						// ������������� �������
						initIntArray(comment, eocd.commentLength + 1);

						fread((char*)comment, eocd.commentLength, 1, file);
						comment[eocd.commentLength] = 0;
						printf("%s, ��� �����������\n", (char*)comment);
					}
				}

				struct CentralDirectoryFileHeader cdfh;

				if (eocd.centralDirectoryOffset) {

					int seek[3] = { SEEK_CUR, SEEK_SET, SEEK_END };

					for (int i = 1; i < 2; ++i) {
						// �������� ������� �� ������� ������ ������
						long int offset = (seek[i] == SEEK_END ? (-1) : 1) * (long int)eocd.centralDirectoryOffset;
						offset += startPosition;
						offset -= 111;
						printf("SEEK: %d, offset: %ld\n", seek[i], offset);
						fseek(file, cdfh_offset /*-offset*/, seek[i]);

						// ������ ��������� CentralDirectoryFileHeader
						fread((char*)&cdfh, sizeof(cdfh), 1, file);

						if (CDFH_Signature != cdfh.signature) {
							// ������
							printf("������ ����������� cdfh.signature (��������: 0x%.8X ������: 0x%.8X)\n",
								cdfh.signature, CDFH_Signature);
							// break;
						}
						else {
							printf("���������� cdfh.signature (��������: 0x%.8X ������: 0x%.8X)\n",
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
					printf("��������� �� %u ��������\n", index);

					// ��������� ��� ����� / �����
					if (cdfh.filenameLength) {
						uint8_t* filename = malloc((cdfh.filenameLength + 1) * sizeof(uint8_t));
						if (filename != NULL) {
							// ������������� �������
							initIntArray(filename, cdfh.filenameLength + 1);

							// ������� ������� �������� �����
							fread((char*)filename, cdfh.filenameLength, 1, file);

							// ���������� ���������� �������� �������
							filename[cdfh.filenameLength] = 0;

							printf("%s,", (char*)filename);
						}
					}
				}
				break;
			}
			else
			{
				// ���� ������� �� ������� ��������� �� ���� ����
				// �� ���������� ������� � ������� ������ �����
				fseek(file, -1 * (sizeof(signature) + 1), SEEK_CUR);
			}
		}

		fclose(file);
	}

	struct ZIP_File result = { NULL, 0, 0 };
	return result;
}

// ����������� ������� �����
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

// ������������� �������
void initIntArray(uint8_t* arr, int length) {
	for (int i = 0; i < length; ++i) {
		arr[i] = 0;
	}
}
