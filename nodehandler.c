#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NODE_NAME_BUFFER_SIZE 256
#define NODE_CHILD_SIZE_INCREMENTER 5
// Node structure
typedef unsigned long ULONG;
typedef struct _NODE 
{
	char* pszName; 
	ULONG ulIntVal; 
	char* pszString; 
	int iNodes; 
	int iArraySizeChildNodes;
	struct _NODE* pnNodes[NODE_CHILD_SIZE_INCREMENTER];
	
	//Function pointers
	struct _NODE* (*GetChildWithKey)(struct _NODE* self, char* key);
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
NODE* newNode(char* pszName);
NODE* findNodeByKey(char* nodeKey);

// Pointer Functions
NODE* NODE_GetChildWithKey(NODE* self, char* sKey);

// Global variables
NODE* rootNode;

int main(int argc, char **argv)
{
		
	// Init root mode
	rootNode = newNode("root");
	//loadNodesFromFile(FILEPATH);
	addNode(rootNode, newNode("seb"));
	
	NODE* nodeNameTest = findNodeByKey("seb"); 
	if(nodeNameTest != NULL)
		printf("Node: %s", nodeNameTest->pszName);
	
	findNodeByKey("strings");
	
	getchar();
	return 0;
}

// Node
NODE* newNode(char* pszName)
{
	NODE* node = malloc(sizeof(NODE));
	node->pszName = pszName;
	node->pszString = NULL;
	node->ulIntVal = NULL;
	node->iNodes = 0;
	node->iArraySizeChildNodes = NODE_CHILD_SIZE_INCREMENTER;

	node->GetChildWithKey = NODE_GetChildWithKey;
	return node;
}

void destructNode(NODE* nNode)
{
	free(nNode);
}

// Adds as a child of another node
int addNode(NODE* nodeDestination, NODE* node)
{
	if(node == NULL) { return 0;};
	
	if(nodeDestination->iNodes < nodeDestination->iArraySizeChildNodes)
	{
		nodeDestination->pnNodes[nodeDestination->iNodes] = node;
		nodeDestination->iNodes++;
	}else
	{
		//TODO: Increase nodearray and add
	}
	
	return 1;
}

NODE* findNodeByKey(char* nodeKey)
{
	NODE* node = NULL;
	if(nodeKey == NULL || rootNode == NULL){ return node; }
	
	int iNodeCount = findNodeCountInString(nodeKey);
	char* nodeNames = findNodeNames(nodeKey, iNodeCount);
	NODE* tempNode = rootNode;
	
	for(int i = 0; i < iNodeCount; i++)
	{
		node = tempNode->GetChildWithKey(tempNode, &nodeNames[i * NODE_NAME_BUFFER_SIZE]);
		if(node == NULL) { break; };
		
		tempNode = node;
	}
	
	free(nodeNames);
	return node;
}

// Function pointers
NODE* NODE_GetChildWithKey(NODE* self, char* sKey)
{
	NODE* nodeChild = NULL;
	for(int i = 0; i < self->iNodes; i++)
	{
		NODE* tempNode = self->pnNodes[i];
		if(strcasecmp(tempNode->pszName, sKey) == 0)
		{
			nodeChild = tempNode;
			break;
		}
	}
	
	return nodeChild;
}

// Filehandling
NODE* loadNodesFromFile(char* filePath)
{
	FILE* fFile = loadFile(filePath);
	if(fFile == NULL){return NULL;}
	
	char* sBuffer = malloc(1024);
	size_t iLineLength;
	char cReadStatus;
	
	while(cReadStatus = getline(&sBuffer, &iLineLength, fFile) != -1)
	{
		printf("Line: %s", sBuffer);
		parseNodeData(sBuffer);
		printf("\n");
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

int parseNodeData(char* pszNodeData)
{
	if(pszNodeData == NULL)
		return 0;
	
	int iNodeCount = findNodeCountInString(pszNodeData);
	printf("NodeCount: %d\n", iNodeCount);
	char* nodeNames = findNodeNames(pszNodeData, iNodeCount);
	for(int i = 0; i < iNodeCount; i++)
	{
		printf("%s\n", &nodeNames[i*NODE_NAME_BUFFER_SIZE]);
	}
	char* nodeValue = findNodeValue(pszNodeData);
	printf("Value: %s\n", nodeValue);
	
	free(nodeNames);
	free(nodeValue);
	return 1;
}

//TODO: Clean up
char* findNodeValue(char* pszNodeData)
{
	char* sNodeValue = malloc(NODE_NAME_BUFFER_SIZE);
	char cBuffer[NODE_NAME_BUFFER_SIZE];
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
			// End of numeric value
			if(!iFoundStartString && iFoundStartValue && pszNodeData[i] == ' '){ break; }
			
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
	char* asNodeNames = malloc(iNodeCount * NODE_NAME_BUFFER_SIZE);
	char cBuffer[NODE_NAME_BUFFER_SIZE];
	int iCurrentNode = 0;
	int iBufferIndex = 0;
	int iFoundEndNames = 0;
	for(int i = 0; i < strlen(pszNodeData); i++)
	{
		if(pszNodeData[i] == '.' || pszNodeData[i] == ' ')
		{
			iFoundEndNames = 1;
			cBuffer[iBufferIndex] = '\0';
			memcpy(&asNodeNames[iCurrentNode*NODE_NAME_BUFFER_SIZE], &cBuffer[0], strlen(&cBuffer[0]) + 1 );

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
	
	if(!iFoundEndNames)
	{
		cBuffer[iBufferIndex] = '\0';
		memcpy(&asNodeNames[iCurrentNode*NODE_NAME_BUFFER_SIZE], &cBuffer[0], strlen(&cBuffer[0]) + 1 );
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