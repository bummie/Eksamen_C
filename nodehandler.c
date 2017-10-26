#include <stdio.h>

// Node structure
typedef unsigned long ULONG;
typedef struct _NODE 
{
	char *pszName; // Navnet på denne noden.
	ULONG ulIntVal; // Hvis numerisk så er dette verdien.
	char *pszString; // Peker til streng eller NULL hvis numerisk.
	int iNodes; // Antall noder under denne. 0 for ingen.
	struct _NODE *pnNodes[]; // Pekere til under-noder.
} NODE;

// Headers
FILE* loadFile(char* filePath);
NODE* loadNodesFromFile(char* filePath[]);

int main(int argc, char **argv)
{
	loadNodesFromFile("../Data/data.txt");
	getchar();
	return 0;
}

NODE* loadNodesFromFile(char* filePath[])
{
	FILE* fFile = loadFile(filePath);
	if(fFile == NULL){return NULL;}
	
	
}

FILE* loadFile(char* filePath)
{
	if(filePath == NULL)
	{
		printf("Filplassering kan ikke være NULL\n");
		return NULL;
	}
	
	FILE* fFile = fopen(filePath, "r");
	
	if(fFile == NULL)
	{
		printf("Filplassering var ikke gyldig\n");
		return NULL;
	}
	
	return fFile;	
}

