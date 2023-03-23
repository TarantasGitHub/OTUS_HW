struct TempReader {
	int index;
	int count;
	char *container;
};

struct TempReader getTRReader(int count);

void addTRItem (struct TempReader *reader, int val); 

void getTRValue (struct TempReader *reader, char *result); 
