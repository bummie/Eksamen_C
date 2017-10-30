#include "nodehandler.h"

int main(void)
{
	// Init root mode
	init();
	loadNodesFromFile(FILEPATH);
	
	NODE* nodeNameTest = findNodeByKey("config.update.server1"); 
	nodeNameTest->Print(nodeNameTest);
	ULONG testVal = 120;
	nodeNameTest->SetValue(nodeNameTest, TYPE_NUMERIC, &testVal);
	nodeNameTest->Print(nodeNameTest);
	Enumerate("config.update.*", callbackPrint);
	sortNodeChildren(findNodeByKey("config.update"));
	Enumerate("config.update.*", callbackPrint);
	Enumerate("strings.no.header", callbackPrint);
	
	printf("Button_cancel: %s\n", GetText("button_ok", "no"));
	
	Delete("config.update");
		
	getchar();
	return 0;
}