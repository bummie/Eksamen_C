// 	Sebastian S. Berge - Eksamen: "C programmering i Linux"
//	31.10.2017

#include "nodehandler.h"
#define FILEPATH "../Data/data.txt"

int main(void)
{
	// Part 1
	printf("Part 1\n");
	init();
	printf("Initialized root node\n");
	
	// Creates the nodes in sorted order from file
	// Using SetValue to give the nodes value
	if(!loadNodesFromFile(FILEPATH)) { printf("Could not load file\n"); getchar(); return 1;}
	printf("Loaded nodes from file: %s\n", FILEPATH);
	
	//Setters and getters
	printf("Fetching node with name server1\n");
	NODE* nodeTest = findNodeByKey("config.update.server1"); 
	nodeTest->Print(nodeTest);
	
	nodeTest->SetInt(nodeTest, 120);
	printf("\nUsing SetInt to set value of node to unsigned long value\n");
	nodeTest->Print(nodeTest);
	
	nodeTest->SetString(nodeTest, "This is a string!");
	printf("\nUsing SetString to set value of the node to a strng\n");
	nodeTest->Print(nodeTest);
	
	printf("\nThe nodechildren are sorted on insert\n");
	Enumerate("strings.en.*", callbackPrint);

	printf("\n\nPart 2\n");
	// Part 2
	
	// GetType of node
	printf("Get the type of the node\n");
	if(nodeTest->GetType(nodeTest) == TYPE_FOLDER) { printf("Node is a folder.\n");}
	if(nodeTest->GetType(nodeTest) == TYPE_NUMERIC) { printf("Node has a numeric value.\n");}
	if(nodeTest->GetType(nodeTest) == TYPE_STRING) { printf("Node has a string value.\n");}
	
	ULONG iValue = 123;
	nodeTest->SetValue(nodeTest, TYPE_NUMERIC, &iValue);
	printf("\nUsing SetValue to set value of node to unsigned long value\n");
	nodeTest->Print(nodeTest);
	
	nodeTest->SetValue(nodeTest, TYPE_STRING, "This is a string!");
	printf("\nUsing SetValue to set value of the node to a strng\n");
	nodeTest->Print(nodeTest);

	printf("\n\nPart 3\n");

	// Enumerator
	printf("\nEnumerator string.en.*:\n");
	Enumerate("strings.en.*", callbackPrint);
	printf("\nEnumerator string.no.header:\n");
	Enumerate("strings.no.header", callbackPrint);
	
	printf("\n\nPart 4\n\n");
	// Delete
	NODE* demoResizeNode = findNodeByKey("strings.en");
	printf("The children array is increasing and decrasing dynamically.\n");
	printf("EN: Arraysize: %d/%d\n", demoResizeNode->iNodes, demoResizeNode->iArraySizeChildNodes);
	Enumerate("strings.en.*", callbackPrint);
	printf("\nDemo deleting nodes with automatic resize of array\n");
	Delete("strings.en.katt");
	Delete("strings.en.ape");
	Delete("strings.en.test");
	Enumerate("strings.en.*", callbackPrint);
	printf("EN: Arraysize: %d/%d\n", demoResizeNode->iNodes, demoResizeNode->iArraySizeChildNodes);
	
	printf("\nPart 5\n\n");
	printf("Try to get text of button_ok in norwegian, this does not exist so it tries to get english version\n");
	printf("NO:Button_ok: %s\n", GetText("button_ok", "no"));
	printf("NO:Header: %s\n", GetText("header", "no"));
	printf("Test if unknown string returns NULL\n");
	if(GetText("button_unknown", "no") == NULL) { printf("Unknown string\n"); }
	
	printf("\n\nEXTRA: Prints all nodes in tree\n");
	PrintNodeWithChildren(GetRootNode());
	
	printf("\n\nPress any key to exit..");
	getchar();
	return 0;
}