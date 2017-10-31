// 	Sebastian S. Berge - Eksamen: "C programmering i Linux"
//	31.10.2017

#include "nodehandler.h"
#define FILEPATH "../Data/data.txt"

int main(void)
{
	// Part 1
	init();
	
	// Creates the nodes in sorted order from file
	// Using SetValue to give the nodes value
	loadNodesFromFile(FILEPATH);
	
	//Setters and getters
	NODE* nodeTest = findNodeByKey("config.update.server1"); 
	nodeTest->Print(nodeTest);
	
	ULONG testVal = 120;
	nodeTest->SetValue(nodeTest, TYPE_NUMERIC, &testVal);
	nodeTest->Print(nodeTest);
	nodeTest->SetValue(nodeTest, TYPE_STRING, "TestNode er noden!");
	nodeTest->Print(nodeTest);
	
	// Part 2
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