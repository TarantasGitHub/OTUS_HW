#include <stdio.h>
#include <stdlib.h>
#include "binaryitem.h"

struct TempReader getTRReader(int count) {

	char* c = malloc(count * sizeof(char));

	if (c != NULL) {
		for (int i = 0; i < count; ++i) {
			c[i] = 0;
		}
	}
	else {
		printf("Error malloc in getTRReader.");
	}

	struct TempReader reader = { 0, count, &c[0] };
	return reader;
}

void addTRItem(struct TempReader* reader, int val) {

	if (reader->index < reader->count - 1) {
		reader->index += 1;
	}
	else {
		reader->index = 0;
	}

	*(reader->container + reader->index) = val;
	unsigned int wer = 0;
	getTRValue(reader, (char*)&wer);

	//	printf("Вставка значения 0x%x в позицию %d (%p -- 0x%x -- 0x%X)",
	//		       	val, reader->index, reader->container+reader->index, *(reader->container+reader->index), wer);
}

void getTRValue(struct TempReader* reader, char* result) {

	//	result = (char *)malloc(sizeof(char)*reader->count);
	if (result == NULL) {
		fprintf(stderr, "Error malloc in getTRValue.");
		exit(1);
	}
	int index = reader->index;

	for (int i = (reader->count - 1); i >= 0; --i) {
		*(result + i) = *(reader->container + index);
		index = index > 0 ? index - 1 : reader->count - 1;
	}
}
