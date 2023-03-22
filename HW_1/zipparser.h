#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

struct ZIP_File {
	FILE* file;
	uint32_t startPosition;
	uint32_t endPosition;
};

struct ZIP_File getZipFile(const char* file_name, int startPosition);

uint64_t getFileSize(const char* file_name);
