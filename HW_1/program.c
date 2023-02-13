#include <stdio.h>

int main(int argc, char *argv[])
{
	if(argc > 1)
	{
		for(int i = 0; i < argc; i++)
		{
			printf("Параметр № %d %s\n", i, argv[i]);
		}

		FILE *file;

		if((file = fopen(argv[1], "r")) != NULL)
		{
			printf("Файл \"%s\" найден.\n", argv[1]);			
			fclose(file);
		}
		else
		{
			printf("Файл  \"%s\" не найден.\n", argv[1]);
		}
	}
	printf("hello, world\n");
	return 0;
}
