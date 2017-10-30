#include "nodehandler.h"

int main(void)
{
	// Init root mode
	init();
	loadNodesFromFile(FILEPATH);
	
	NODE* nodeNameTest = findNodeByKey("config.update.server1"); 
	nodeNameTest->Print(nodeNameTest);
	nodeNameTest->SetValue(nodeNameTest, TYPE_STRING, "Hello");
	nodeNameTest->Print(nodeNameTest);
	Enumerate("config.update.*", callbackPrint);
	Enumerate("strings.no.header", callbackPrint);
	
	Delete("config.update");
		
	getchar();
	return 0;
}