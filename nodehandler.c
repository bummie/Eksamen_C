#include <stdio.h>
#include <stdlib.h>

// Node structure
typedef unsigned long ULONG;
typedef struct _NODE 
{
	char* pszName; // Navnet på denne noden.
	ULONG ulIntVal; // Hvis numerisk så er dette verdien.
	char *pszString; // Peker til streng eller NULL hvis numerisk.
	int iNodes; // Antall noder under denne. 0 for ingen.
	struct _NODE *pnNodes[]; // Pekere til under-noder.
} NODE;

// Constants
const char* FILEPATH = "../Data/data.txt";

// Headers
FILE* loadFile(char* filePath);
NODE* loadNodesFromFile(char* filePath);
int findNodeCountInString(char* pszNodeData);
int parseNodeData(char* pszNodeData);
char* findNodeNames(char* pszNodeData, int iNodeCount);

int main(int argc, char **argv)
{
	loadNodesFromFile(FILEPATH);
	getchar();
	return 0;
}

NODE* loadNodesFromFile(char* filePath)
{
	FILE* fFile = loadFile(filePath);
	if(fFile == NULL){return NULL;}
	
	char* sBuffer = malloc(sizeof(char) * 1024);
	size_t iLineLength;
	char cReadStatus;
	
	while(cReadStatus = getline(&sBuffer, &iLineLength, fFile) != -1)
	{
		printf("Line: %s", sBuffer);
		parseNodeData(sBuffer);
	}
	
	free(sBuffer);
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

// TODO: node constructor
NODE* NODE_New(char* pszName)
{
	NODE* node = malloc(sizeof(NODE));
	node->pszName = pszName;
	node->pszString = NULL;
	node->ulIntVal = NULL;
	node->iNodes = 0;
	
	return node;
}

void NODE_Destructor(NODE* nNode)
{
	free(nNode);
}

int parseNodeData(char* pszNodeData)
{
	if(pszNodeData == NULL)
		return 0;
	
	int iNodeCount = findNodeCountInString(pszNodeData);
	printf("NodeCount: %d\n", iNodeCount);
	findNodeNames(pszNodeData, iNodeCount);
	return 1;
}

char* findNodeNames(char* pszNodeData, int iNodeCount)
{
	char* asNodeNames[iNodeCount];
	char cBuffer[256];
	int iCurrentNode = 0;
	int iBufferIndex = 0;

	for(int i = 0; i < strlen(pszNodeData); i++)
	{
		if(pszNodeData[i] == ' ')
			break;
		if(pszNodeData[i] == '.')
		{
			cBuffer[iBufferIndex] = '\0';
			asNodeNames[iCurrentNode] = &cBuffer[0];
			iBufferIndex = 0;
			iCurrentNode++;
		}else
		{
			cBuffer[iBufferIndex] = pszNodeData[i];
			iBufferIndex++;
			printf("%c", pszNodeData[i]);
		}
	}
	
	for(int i = 0; i < iNodeCount; i++)
	{
		printf("\n%s", asNodeNames[i]);
	}
	
	printf("\n");
	
	return NULL;
}

int findNodeCountInString(char* pszNodeData)
{
	int iNodeCount = 1;
	for(int i = 0; i < strlen(pszNodeData); i++)
	{
		if(pszNodeData[i] == '.')
			iNodeCount++;
		if(pszNodeData[i] == ' ')
			break;
	}
		
	return iNodeCount;
}