// 	Sebastian S. Berge - Eksamen: "C programmering i Linux"
//	31.10.2017

#include "nodehandler.h"
#define FILEPATH "../Data/data.txt"

int main(void)
{
	// Part 1
	init();
	loadNodesFromFile(FILEPATH);
	
	NODE* nodeNameTest = findNodeByKey("config.update.server1"); 
	nodeNameTest->Print(nodeNameTest);
	ULONG testVal = 120;
	nodeNameTest->SetValue(nodeNameTest, TYPE_NUMERIC, &testVal);
	nodeNameTest->Print(nodeNameTest);
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