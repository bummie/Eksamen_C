// 	Sebastian S. Berge - Eksamen: "C programmering i Linux"
//	31.10.2017
#ifndef _NODEHANDLER_H_
#define _NODEHANDLER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NODE_NAME_BUFFER_SIZE 256
#define NODE_CHILD_SIZE_INCREMENTER 5
#define FALSE 0
#define TRUE !FALSE
#define LEFT 0
#define RIGHT !LEFT

// Node types
#define TYPE_NUMERIC 0
#define TYPE_STRING 1
#define TYPE_FOLDER 2

// Node structure
typedef unsigned long ULONG;
typedef struct _NODE 
{
	char* pszName; 
	ULONG ulIntVal; 
	char* pszString;
	int iNodes; 
	int iArraySizeChildNodes;
	struct _NODE** pnNodes;
	
	//Function pointers
	struct _NODE* (*GetChildWithKey)(struct _NODE* self, char* key);
	void (*IncreaseChildArray)(struct _NODE* self);
	void (*DecreaseChildArray)(struct _NODE* self);
	void (*SetInt)(struct _NODE* self, ULONG ulValue);
	void (*SetString)(struct _NODE* self, char* pszText);
	ULONG (*GetInt)(struct _NODE* self);
	char* (*GetString)(struct _NODE* self);
	int (*GetType)(struct _NODE* self);
	void (*Print)(struct _NODE* self);
	void* (*GetValue)(struct _NODE* self);
	void (*SetValue)(struct _NODE* self, int iValueType, void* nodeValue);
	
} NODE;

// Headers
FILE* loadFile(char* filePath);
int loadNodesFromFile(char* filePath);
int findNodeCountInString(char* pszNodeData);
int parseNodeData(char* pszNodeData);
char* findNodeNames(char* pszNodeData, int iNodeCount);
char* findNodeValue(char* pszNodeData);
NODE* newNode(char* pszName);
NODE* findNodeByKey(char* nodeKey);
void Enumerate(char* nodeKey, void (*Callback)(char* nodeName, void* nodeValue));
void callbackPrint(char* nodeName, void* nodeValue);
void Delete(char* nodeKey);
void DeleteByNode(NODE* nodeDelete);
char* GetText(char* nodeName, char* nodeLangCode);
void sortNodeChildren(NODE* nodeParent);
void swap(NODE* a, NODE* b);
void shiftArray(int iDirection, NODE* nodeParent, int iIndex);
void addNodeSortedPosition(NODE* nodeParent, NODE* newNode);
char* getParentByNodeKey(char* nodeKey);
int getNodeIndexFromParent(NODE* parentNode, NODE* childNode);
void stripStringQuotes(char* pszString);
void PrintNodeWithChildren(NODE* node);
NODE* GetRootNode();

// Pointer Functions
NODE* NODE_GetChildWithKey(NODE* self, char* sKey);
void NODE_IncreaseChildArraySize(NODE* self);
void NODE_DecreaseChildArraySize(NODE* self);
void NODE_SetInt(NODE* self, ULONG ulValue);
void NODE_SetString(NODE* self, char* pszText);
ULONG NODE_GetInt(NODE* self);
char* NODE_GetString(NODE* self);
int NODE_GetType(NODE* self);
void NODE_Print(NODE* self);
void* NODE_GetValue(NODE* self);
void NODE_SetValue(NODE* self, int iValueType, void* nodeValue);
#endif