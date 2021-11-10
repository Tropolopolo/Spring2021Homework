#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct city_relations{
	char Origin[1024];
	char Dest[1024];
	int cost;
} CityPair;

char** readfile(char * filename)
{
	FILE * fp;
	fp = fopen(filename, "r");
	char buffer[1024];
	
	char** lines = (char**) malloc(1024);
	
	fgets(buffer, 1024, fp);
	char end[] = "END OF INPUT";
	while(strncmp(buffer, end, sizeof(end) != 0)
	{
		
	}
	fclose(fp);
}

int main(int args, char * argv[])
{
	//printf("%d\n",args);
	if(args == 1)
	{
		printf("Please insert input file.\n");
	}
	else{
		//command line arguments format
		//find_route input_filename origin_city destination_city heuristic_filename
		
		readfile(argv[1]);
	}
}

//compilation:
/*
	gcc find_route.c -o find_route
*/