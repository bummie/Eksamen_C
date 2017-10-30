#include "nodehandler.h"

// Global variables
NODE* rootNode;

void init()
{
	rootNode = newNode("root");
}

// Node
NODE* newNode(char* pszName)
{
	NODE* node = calloc(1, sizeof(NODE));
	node->pszName = malloc(strlen(pszName) + 1);
	memcpy(node->pszName, pszName, strlen(pszName) + 1);
	node->pszString = NULL;
	node->ulIntVal = NULL;
	node->iNodes = 0;
	node->iArraySizeChildNodes = NODE_CHILD_SIZE_INCREMENTER;
	node->pnNodes = calloc(NODE_CHILD_SIZE_INCREMENTER, sizeof(NODE*));
	
	// Functions
	node->GetChildWithKey = NODE_GetChildWithKey;
	node->IncreaseChildArray = NODE_IncreaseChildArraySize;
	node->SetInt = NODE_SetInt;
	node->SetString = NODE_SetString;
	node->GetInt = NODE_GetInt;
	node->GetString = NODE_GetString;
	node->GetType = NODE_GetType;
	node->Print = NODE_Print;
	node->SetValue = NODE_SetValue;
	node->GetValue = NODE_GetValue;
	
	return node;
}

void destructNode(NODE* nNode)
{
	free(nNode->pnNodes);
	nNode->pnNodes = NULL;
	free(nNode->pszName);
	nNode->pszName = NULL;
	free(nNode);
	nNode = NULL;
}

// Adds as a child of another node
int addNode(NODE* nodeDestination, NODE* node)
{
	if(nodeDestination == NULL || node == NULL) { return 0;};
	if(nodeDestination->GetChildWithKey(nodeDestination, node->pszName) != NULL)
	{ 
		destructNode(node);
		return 0;
	}
	
	// If nodes childrens array is full, increase
	if(nodeDestination->iNodes >= nodeDestination->iArraySizeChildNodes)
	{
		printf("'%s' is full, increasing size\n", nodeDestination->pszName);
		nodeDestination->IncreaseChildArray(nodeDestination);
	}
		
	nodeDestination->pnNodes[nodeDestination->iNodes] = node;
	nodeDestination->iNodes++;
	
	return 1;
}

void Delete(char* nodeKey)
{
	if(nodeKey == NULL) { return; };
	
	NODE* nodeDelete = findNodeByKey(nodeKey);
	if(nodeDelete == NULL) { return; }
	
	// Delete babies first
	if(nodeDelete->iNodes > 0)
	{
		for(int i = 0; i < nodeDelete->iNodes; i++)
		{
			DeleteByNode(nodeDelete->pnNodes[i]);
		}
	}
	printf("Sletter_NODE: %s\n", nodeDelete->pszName);
	destructNode(nodeDelete);
	
	//TODO: Oppdatere forelder om at barnet er dødt
	//TODO: Sortere barnene til forelder, så ingen hull
	//TODO: Decrase size om antallerer mindre
}

void DeleteByNode(NODE* nodeDelete)
{
	if(nodeDelete == NULL) { return; };

	// Delete babies first
	if(nodeDelete->iNodes > 0)
	{
		for(int i = 0; i < nodeDelete->iNodes; i++)
		{
			Delete(nodeDelete->pnNodes[i]);
		}
	}
	
	printf("Sletter_barn: %s\n", nodeDelete->pszName);
	destructNode(nodeDelete);
}

char* GetText(char* nodeName, char* nodeLangCode)
{
	if(nodeName == NULL || nodeLangCode == NULL) { return; }
	
	char nodeKey[512];
	snprintf(nodeKey, (sizeof(nodeKey)-1), "%s.%s.%s", "strings", nodeLangCode, nodeName);
	
	NODE* nodeLang = findNodeByKey(nodeKey);
	if(nodeLang == NULL)
	{
		// Try english version
		if(strcasecmp("en", nodeLangCode) == 0) { return NULL; }
		
		snprintf(nodeKey, (sizeof(nodeKey)-1), "%s.%s.%s", "strings", "en", nodeName);
		nodeLang = findNodeByKey(nodeKey);
	}
	return nodeLang->GetValue(nodeLang);
}

// Takes inputkey "strings.no.*" and calls function pointer with nodekey and value
void Enumerate(char* nodeKey, void (*Callback)(char* nodeName, void* nodeValue))
{
	if(nodeKey == NULL || Callback == NULL) { return; }
	
	// If user inputted a star at the end
	if(nodeKey[strlen(nodeKey) - 1] == '*')
	{
		char* pszKeyDuplicate = strdup(nodeKey);
		char nodeChildKey[NODE_NAME_BUFFER_SIZE];
		pszKeyDuplicate[strlen(pszKeyDuplicate) - 2] = '\0';
		
		NODE* parentNode = findNodeByKey(pszKeyDuplicate);
		
		if(parentNode != NULL)
		{
			for(int i = 0; i < parentNode->iNodes; i++)
			{
				NODE* childNode = parentNode->pnNodes[i];
				if(childNode->GetType(childNode) != TYPE_FOLDER)
				{
					// Creates nodekey for child
					snprintf(nodeChildKey, (sizeof(nodeChildKey)-1), "%s.%s", pszKeyDuplicate, childNode->pszName);
					//childNode->Print(childNode);
					Callback(nodeChildKey, childNode->GetValue(childNode));
				}
			}
		}
		free(pszKeyDuplicate);
	}
	else
	{
		// Did not find star *
		NODE* parentNode = findNodeByKey(nodeKey);
		if(parentNode != NULL)
		{
			Callback(nodeKey, parentNode->GetValue(parentNode));
		}
	}
}

void callbackPrint(char* nodeName, void* nodeValue)
{
	if(nodeName == NULL || nodeValue == NULL) { return; }
	NODE* callbackNode = findNodeByKey(nodeName);
	if(callbackNode == NULL) { return; }

	if(callbackNode->GetType(callbackNode) == TYPE_NUMERIC)
	{
		printf("Callback_Node: %s, Value: %d\n", nodeName, nodeValue);
	}
	else if(callbackNode->GetType(callbackNode) == TYPE_STRING)
	{
		printf("Callback_Node: %s, Value: %s\n", nodeName, nodeValue);
	}
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
		if(strcasecmp(tempNode->pszName, sKey) == 0)// TODO: Binary search 
		{
			nodeChild = tempNode;
			break;
		}
	}
	
	return nodeChild;
}

// Function pointers
void NODE_IncreaseChildArraySize(NODE* self)
{
	//TODO: Fikse increase størrelsen
	if(self == NULL) { return; }
	int iNewSize = (self->iArraySizeChildNodes + NODE_CHILD_SIZE_INCREMENTER);
	NODE* nodeReallocated = realloc(self->pnNodes, iNewSize * sizeof(NODE*));
	
	if(nodeReallocated != NULL)
	{
		self->pnNodes = nodeReallocated;
	}else
	{
		printf("Could not increase childrenArray");
	}
}

void NODE_SetInt(NODE* self, ULONG ulValue)
{
	//TODO: Fikse increase størrelsen
	if(self == NULL || ulValue == NULL) { return; }
	self->ulIntVal = ulValue;
	self->pszString = NULL;
}

void NODE_SetString(NODE* self, char* pszText)
{
	//TODO: Fikse increase størrelsen
	if(self == NULL || pszText == NULL) { return; }
	self->pszString = strdup(pszText);//self->pszString = pszText;
}

ULONG NODE_GetInt(NODE* self)
{
	if(self == NULL) { return NULL; }
	return self->ulIntVal;
}

char* NODE_GetString(NODE* self)
{
	if(self == NULL) { return NULL; }
	return self->pszString;
}

void* NODE_GetValue(NODE* self)
{
	if(self == NULL) { return NULL; }
	
	if(self->GetType(self) == TYPE_NUMERIC)
	{
		return (void*) ((int*)(self->GetInt(self)));
	}
	else if(self->GetType(self) == TYPE_STRING)
	{
		return (void*) self->GetString(self);
	}
	
	return NULL;
}

void NODE_SetValue(NODE* self, int iValueType, void* nodeValue)
{
	if(self == NULL) { return NULL; }
	
	if(iValueType == TYPE_NUMERIC)
	{
		self->SetInt(self, *((int*) nodeValue));
	}
	else if(iValueType == TYPE_STRING)
	{
		self->SetString(self, (char*) nodeValue);
	}
}

int NODE_GetType(NODE* self)
{
	if(self == NULL) { return NULL; }
	
	if(self->GetString(self) == NULL && self->GetInt(self) == NULL)
	{
		return TYPE_FOLDER;
	}
	else if(self->GetString(self) == NULL)
	{
		return TYPE_NUMERIC;
	}
	else
	{
		return TYPE_STRING;
	}
}

void NODE_Print(NODE* self)
{
	if(self == NULL) { printf("Print failed: Node is null\n"); return NULL; }
	
	if(self->GetType(self) == TYPE_FOLDER)
	{
		printf("[%s]: Type: Folder, Children: %d\n", self->pszName, self->iNodes);
	}
	else if(self->GetType(self) == TYPE_NUMERIC)
	{
		printf("[%s]: Type: Numeric, Children: %d, Value: %d\n", self->pszName, self->iNodes, self->GetInt(self));
	}
	else if(self->GetType(self) == TYPE_STRING)
	{
		printf("[%s]: Type: Text, Children: %d, Value: %s\n", self->pszName, self->iNodes, self->GetString(self));
	}
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
		parseNodeData(sBuffer);
		printf("\n");
	}
	
	free(sBuffer);
	sBuffer = NULL;
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
	//printf("NodeCount: %d\n", iNodeCount);
	char* nodeNames = findNodeNames(pszNodeData, iNodeCount);
	
	NODE* tempNode = rootNode;
	for(int i = 0; i < iNodeCount; i++)
	{
		printf("%s\n", &nodeNames[i*NODE_NAME_BUFFER_SIZE]);
		printf("AddNodeState: %d\n", addNode(tempNode, newNode(&nodeNames[i*NODE_NAME_BUFFER_SIZE])));
		tempNode = tempNode->GetChildWithKey(tempNode, &nodeNames[i*NODE_NAME_BUFFER_SIZE]);
		if(tempNode == NULL) { break; }
		
		printf("StringNameFromNode: %s\n", tempNode->pszName);
	}
	
	// Set value of node
	char* nodeValue = findNodeValue(pszNodeData);
	if(tempNode != NULL)
	{
		char* sExtraChars;
		ULONG ulValue = strtol(nodeValue, &sExtraChars, 10);
		
		if(*sExtraChars == '\0' || *sExtraChars == '\n')
		{
			tempNode->SetInt(tempNode, ulValue);
		}else
		{
			tempNode->SetString(tempNode, nodeValue);
		}
	}
	//printf("Value: %s\n", nodeValue);
	
	// Fre memory
	free(nodeNames);
	nodeNames = NULL;
	free(nodeValue);
	nodeValue = NULL;
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
			if(iFoundStartString && pszNodeData[i] == '"')
			{ 
				cBuffer[iBufferIndex] = '"';
				iBufferIndex++;
				break;
			}
			// End of numeric value
			if(!iFoundStartString && iFoundStartValue && pszNodeData[i] == ' '){ break; }
			
			if(!iFoundStartString && pszNodeData[i] == '"'){ iFoundStartString = 1; }
			if(pszNodeData[i] != ' ' && !iFoundStartValue) { iFoundStartValue = 1; }
				
			if(iFoundStartValue)// && pszNodeData[i] != '"')
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
	
	for(int i = 0; i < strlen(pszNodeData); i++)
	{
		if(pszNodeData[i] == '.' || pszNodeData[i] == ' ' || pszNodeData[i] == '=')
		{
			cBuffer[iBufferIndex] = '\0';
			memcpy(&asNodeNames[iCurrentNode*NODE_NAME_BUFFER_SIZE], &cBuffer[0], strlen(&cBuffer[0]) + 1 );

			if(pszNodeData[i] == ' ' || pszNodeData[i] == '=')
				break;
			
			iBufferIndex = 0;
			iCurrentNode++;
		}else
		{
			cBuffer[iBufferIndex] = pszNodeData[i];
			iBufferIndex++;
		}
	}
	
	if((iNodeCount - iCurrentNode) > 0 )
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

void sortNodeChildren(NODE* nodeParent)
{
	if(nodeParent == NULL) { return; }
	
	int iSortedIndex = 0;
	int iSelectedIndex = 0;
	
	while(nodeParent->iNodes > iSortedIndex)
	{
		iSelectedIndex = iSortedIndex;
		for(int i = iSortedIndex; i < nodeParent->iNodes; i++)
		{
			if(strcasecmp(nodeParent->pnNodes[i]->pszName, nodeParent->pnNodes[iSelectedIndex]->pszName) > 0)
			{
				iSelectedIndex = i;
			}
				
		}
		
		printf("Swap: %s med %s\n", nodeParent->pnNodes[iSortedIndex]->pszName, nodeParent->pnNodes[iSelectedIndex]->pszName);
		swap(nodeParent->pnNodes[iSortedIndex], nodeParent->pnNodes[iSelectedIndex]);
		iSortedIndex++;
	}
}

void swap(NODE* a, NODE* b)
{
	NODE* swap = a;
	a = b;
	b = swap;
}
//TODO: Make a stripqoutes function