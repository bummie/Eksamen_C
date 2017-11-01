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
	loadNodesFromFile(FILEPATH);
	printf("Loaded nodes from file: %s\n", FILEPATH);
	
	//Setters and getters
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

	printf("\n\nPart 2");
	// Part 2
	printf("\nUsing SetValue to set value of node to unsigned long value\n");
	nodeTest->Print(nodeTest);
	
	nodeTest->SetValue(nodeTest, TYPE_STRING, "This is a string!");
	printf("\nUsing SetValue to set value of the node to a strng\n");
	nodeTest->Print(nodeTest);

	
	// Enumerator
	Enumerate("strings.en.*", callbackPrint);
	Enumerate("strings.no.header", callbackPrint);
	
	printf("Button_cancel: %s\n", GetText("button_ok", "no"));
	printf("EN: Arraysize: %d\n", findNodeByKey("strings.en")->iArraySizeChildNodes);
	Delete("strings.en.katt");
	Delete("strings.en.ape");
	Delete("strings.en.test");
	Enumerate("strings.en.*", callbackPrint);
	printf("EN: Arraysize: %d\n", findNodeByKey("strings.en")->iArraySizeChildNodes);
	
	getchar();
	return 0;
}