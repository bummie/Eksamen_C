// 	Sebastian S. Berge - Eksamen: "C programmering i Linux"
//	31.10.2017

#include "nodehandler.h"

// Global variables
NODE* rootNode;

void init()
{
	rootNode = newNode("root");
}

// Node constructor
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
	node->DecreaseChildArray = NODE_DecreaseChildArraySize;
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

// Destroys node and frees memory
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
	
	addNodeSortedPosition(nodeDestination, node);
	nodeDestination->iNodes++;
	return 1;
}

// Deletes node and nodes children
void Delete(char* nodeKey)
{
	if(nodeKey == NULL) { return; };
	
	NODE* nodeDelete = findNodeByKey(nodeKey);
	if(nodeDelete == NULL) { return; }
	
	// TODO: Make a new function for this shizz
	char* sParentKey = getParentByNodeKey(nodeKey);
	NODE* parentNode = findNodeByKey(sParentKey);
	
	int iIndex = getNodeIndexFromParent(parentNode, nodeDelete);
	if(iIndex == -1) { return; }
	
	// Delete babies first
	if(nodeDelete->iNodes > 0)
	{
		for(int i = 0; i < nodeDelete->iNodes; i++)
		{
			DeleteByNode(nodeDelete->pnNodes[i]);
		}
	}
	printf("Deleting_NODE: %s\n", nodeDelete->pszName);
	destructNode(nodeDelete);
	
	// Remove one from parentnodecount
	if(parentNode != NULL)
	{
		if(parentNode->iNodes > 0)
		{
			parentNode->iNodes--;
		}
	}
	
	free(sParentKey);
	shiftArray(LEFT, parentNode, iIndex);
	
	// If nodes childrens array is full, increase
	if(parentNode->iNodes <= (parentNode->iArraySizeChildNodes - NODE_CHILD_SIZE_INCREMENTER))
	{
		printf("'%s' is wasting space, decreasing size\n", parentNode->pszName);
		parentNode->DecreaseChildArray(parentNode);
	}
}

// Recursivly deletes childnodes
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
	
	printf("Deleting_NODE_CHILD: %s\n", nodeDelete->pszName);
	destructNode(nodeDelete);
}

// Returns the string for given code and language
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
	if(nodeLang == NULL){ return NULL; }
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

// Prints out the given node
void callbackPrint(char* nodeName, void* nodeValue)
{
	if(nodeName == NULL || nodeValue == NULL) { return; }
	NODE* callbackNode = findNodeByKey(nodeName);
	if(callbackNode == NULL) { return; }
	
	callbackNode->Print(callbackNode);
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

// 
//	Function pointers
//

NODE* NODE_GetChildWithKey(NODE* self, char* sKey)
{
	NODE* nodeChild = NULL;
	for(int i = 0; i < self->iNodes; i++)
	{
		NODE* tempNode = self->pnNodes[i];
		
		// Since array is sorted, stop when we have reached the last possibly position
		if(strcasecmp(tempNode->pszName, sKey) > 0) { break; }
		if(strcasecmp(tempNode->pszName, sKey) == 0) 
		{
			nodeChild = tempNode;
			break;
		}
	}
	
	return nodeChild;
}

void NODE_IncreaseChildArraySize(NODE* self)
{
	if(self == NULL) { return; }
	int iNewSize = (self->iArraySizeChildNodes + NODE_CHILD_SIZE_INCREMENTER);
	NODE* nodeReallocated = realloc(self->pnNodes, iNewSize * sizeof(NODE*));
	
	if(nodeReallocated != NULL)
	{
		self->pnNodes = nodeReallocated;
		self->iArraySizeChildNodes = iNewSize;
	}else
	{
		printf("Could not increase childrenArray");
	}
}

void NODE_DecreaseChildArraySize(NODE* self)
{
	if(self == NULL) { return; }
	int iNewSize = (self->iArraySizeChildNodes - NODE_CHILD_SIZE_INCREMENTER);
	NODE* nodeReallocated = realloc(self->pnNodes, iNewSize * sizeof(NODE*));
	
	if(nodeReallocated != NULL)
	{
		self->pnNodes = nodeReallocated;
		self->iArraySizeChildNodes = iNewSize;
	}else
	{
		printf("Could not decrease childrenArray");
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
	char* tempString = strdup(pszText);
	free(self->pszString);
	self->pszString = tempString;
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
		self->SetInt(self, *((ULONG*) nodeValue));
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

// Prints out information about node
void NODE_Print(NODE* self)
{
	if(self == NULL) { printf("Print failed: Node is null\n"); return NULL; }
	
	if(self->GetType(self) == TYPE_FOLDER)
	{
		printf("[%s]: Type: Folder, Children: %d\n", self->pszName, self->iNodes);
	}
	else if(self->GetType(self) == TYPE_NUMERIC)
	{
		printf("[%s]: Type: Numeric, Children: %d, Value: %d\n", self->pszName, self->iNodes, self->GetValue(self));
	}
	else if(self->GetType(self) == TYPE_STRING)
	{
		printf("[%s]: Type: Text, Children: %d, Value: \"%s\"\n", self->pszName, self->iNodes, self->GetValue(self));
	}
}

// Filehandling
int loadNodesFromFile(char* filePath)
{
	FILE* fFile = loadFile(filePath);
	if(fFile == NULL){return 0;}
	
	char* sBuffer = malloc(1024);
	size_t iLineLength;
	char cReadStatus;
	
	while(cReadStatus = getline(&sBuffer, &iLineLength, fFile) != -1)
	{
		// Dont read comments
		if(sBuffer[0] == '/' && sBuffer[1] == '/') { continue; }
		parseNodeData(sBuffer);
	}
	
	free(sBuffer);
	sBuffer = NULL;
	fclose(fFile);
	return 1;
}

// Tries to load a file, returns NULL if it could not load file
FILE* loadFile(char* filePath)
{
	if(filePath == NULL)
	{
		printf("Filepath can not be NULL\n");
		return NULL;
	}
	
	FILE* fFile = fopen(filePath, "r");
	
	if(fFile == NULL)
	{
		printf("Filepath is not legit\n");
		return NULL;
	}
	
	return fFile;	
}

// Creates nodes from inputted line
int parseNodeData(char* pszNodeData)
{
	if(pszNodeData == NULL) { return 0; }
	
	int iNodeCount = findNodeCountInString(pszNodeData);
	char* nodeNames = findNodeNames(pszNodeData, iNodeCount);
	NODE* tempNode = rootNode;
	
	for(int i = 0; i < iNodeCount; i++)
	{
		addNode(tempNode, newNode(&nodeNames[i*NODE_NAME_BUFFER_SIZE]));
		tempNode = tempNode->GetChildWithKey(tempNode, &nodeNames[i*NODE_NAME_BUFFER_SIZE]);
		if(tempNode == NULL) { break; }
	}
	
	// Set value of node
	char* nodeValue = findNodeValue(pszNodeData);
	if(tempNode != NULL)
	{
		char* sExtraChars;
		ULONG ulValue = strtol(nodeValue, &sExtraChars, 10);
		
		if(*sExtraChars == '\0' || *sExtraChars == '\n')
		{
			tempNode->SetValue(tempNode, TYPE_NUMERIC, &ulValue);
		}else
		{
			stripStringQuotes(nodeValue);
			tempNode->SetValue(tempNode, TYPE_STRING, nodeValue);
		}
	}
	
	// Free memory
	free(nodeNames);
	nodeNames = NULL;
	free(nodeValue);
	nodeValue = NULL;
	return 1;
}

// Finds the value to append a given node from fileline 
char* findNodeValue(char* pszNodeData)
{
	char* sNodeValue = malloc(NODE_NAME_BUFFER_SIZE);
	char cBuffer[NODE_NAME_BUFFER_SIZE];
	int iBufferIndex = 0;
	int iFoundCharEquals = FALSE;
	int iFoundStartValue = FALSE;
	int iFoundStartString = FALSE;
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
			
			if(!iFoundStartString && pszNodeData[i] == '"'){ iFoundStartString = TRUE; }
			if(pszNodeData[i] != ' ' && !iFoundStartValue) { iFoundStartValue = TRUE; }
				
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

// Finds the names from fileline returns names in an array
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

// Finding the count of nodenames by amount of '.'
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

// Adds the node into its sorted position
void addNodeSortedPosition(NODE* nodeParent, NODE* newNode)
{
	if(nodeParent == NULL || newNode == NULL) { return; }
	if(nodeParent->iNodes <= 0) { nodeParent->pnNodes[0] = newNode; return;};
	
	for(int i = 0; i <= nodeParent->iNodes; i++)
	{		
		if(nodeParent->pnNodes[i] == NULL)
		{
			nodeParent->pnNodes[i] = newNode;
			return;
		}	
		else if(strcasecmp(newNode->pszName, nodeParent->pnNodes[i]->pszName) < 0)
		{
			shiftArray(RIGHT, nodeParent, i);
			nodeParent->pnNodes[i] = newNode;
			return;
		}
	}
}

// Moves the nodes children either right og left from given index
void shiftArray(int iDirection, NODE* nodeParent, int iIndex)
{
	if(nodeParent == NULL) { return; }
	
	if(iDirection == LEFT)
	{
		for(int i = iIndex; i < nodeParent->iNodes; i++)
		{
			nodeParent->pnNodes[i] = nodeParent->pnNodes[i+1];
		}
	}
	else // Righ shift
	{
		for(int i = nodeParent->iNodes; i > iIndex; i--)
		{
			if(nodeParent->pnNodes[i-1] != NULL && (i-1) >= 0)
			{
				nodeParent->pnNodes[i] = nodeParent->pnNodes[i-1];
			}
		}
	}
}

// Takes a node key "strings.en.text" and removes the last node
char* getParentByNodeKey(char* nodeKey)
{
	if(nodeKey == NULL && strcasecmp(nodeKey, "root") != 0) { return NULL; }
	
	char* sKeyDupe = strdup(nodeKey);
	
	for(int i = strlen(sKeyDupe); i > 0; i-- )
	{
		if(sKeyDupe[i] == '.')
		{
			sKeyDupe[i] = '\0';
			break;
		}
	}
	return sKeyDupe;
}

// Gives the index where the childNode is located in parentNode
int getNodeIndexFromParent(NODE* parentNode, NODE* childNode)
{
	if(parentNode ==  NULL || childNode == NULL) { return NULL; }
	
	int iIndex = -1;
	
	for(int i = 0; i < parentNode->iNodes; i++)
	{
		if(strcasecmp(parentNode->pnNodes[i]->pszName, childNode->pszName) == 0)
		{
			iIndex = i;
			break;
		}
	}
	
	return iIndex;
}

// Removes quotation marks from string
void stripStringQuotes(char* pszString)
{
	if(pszString == NULL) { printf("String is null"); return; }
	memmove(pszString, pszString+1, strlen(pszString));
	pszString[strlen(pszString)-1] = '\0';
}

// Prints node with children
void PrintNodeWithChildren(NODE* node)
{
	if(node == NULL) { return; }
	node->Print(node);
	for(int i = 0; i < node->iNodes; i++)
	{
		PrintNodeWithChildren(node->pnNodes[i]);
	}
}

// Returns the rootnode
NODE* GetRootNode()
{
	return rootNode;
}