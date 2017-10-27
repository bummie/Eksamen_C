#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
char* findNodeValue(char* pszNodeData);

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
	fclose(fFile);
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
	char* nodeNames = findNodeNames(pszNodeData, iNodeCount);
	for(int i = 0; i < iNodeCount; i++)
	{
		printf("%s\n", &nodeNames[i*256]);
	}
	char* nodeValue = findNodeValue(pszNodeData); //Atoi to determine wehter int or string
	printf("Value: %s\n", nodeValue);
	
	free(nodeNames);
	free(nodeValue);
	return 1;
}

//TODO: Clean up
char* findNodeValue(char* pszNodeData)
{
	char* sNodeValue = malloc(256);
	char cBuffer[256];
	int iBufferIndex = 0;
	int iFoundCharEquals = 0;
	int iFoundStartValue = 0;
	int iFoundStartString = 0;
	for(int i = 0; i < strlen(pszNodeData); i++)
	{
		if(pszNodeData[i] == '=')
		{
			iFoundCharEquals = 1;
			continue;
		}
		
		if(iFoundCharEquals)
		{
			// End of stringvalue
			if(iFoundStartString && pszNodeData[i] == '"'){ break; }
			
			if(!iFoundStartString && pszNodeData[i] == '"'){ iFoundStartString = 1; }
			if(pszNodeData[i] != ' ' && !iFoundStartValue) { iFoundStartValue = 1; }
				
			if(iFoundStartValue && pszNodeData[i] != '"')
			{
				cBuffer[iBufferIndex] = pszNodeData[i];
				iBufferIndex++;
			}
		}
	}
	cBuffer[iBufferIndex] = '\0';
	memcpy(sNodeValue, &cBuffer[0], strlen(cBuffer) + 1);
	
	return sNodeValue;
}

//TODO: Clean up
char* findNodeNames(char* pszNodeData, int iNodeCount)
{
	char* asNodeNames = malloc(iNodeCount * 256);
	char cBuffer[256];
	int iCurrentNode = 0;
	int iBufferIndex = 0;

	for(int i = 0; i < strlen(pszNodeData); i++)
	{
		if(pszNodeData[i] == '.' || pszNodeData[i] == ' ')
		{
			cBuffer[iBufferIndex] = '\0';
			memcpy(&asNodeNames[iCurrentNode*256], &cBuffer[0], strlen(&cBuffer[0]) + 1 );
			//asNodeNames[iCurrentNode] = &cBuffer[0];
			if(pszNodeData[i] == ' ')
				break;
			
			iBufferIndex = 0;
			iCurrentNode++;
		}else
		{
			cBuffer[iBufferIndex] = pszNodeData[i];
			iBufferIndex++;
		}
	}
	return asNodeNames;
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