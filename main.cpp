#include <Windows.h>
#include <iostream>
#include "zipHelper.h"
using namespace std;
 
int main(int argc, char** argv) {
	cPackFile packTool;
	cUnpackFile unpackTool;
	 
	packTool.CreateZipFromDir("C:\\Users\\Ragul\\Desktop\\test\\setup2\\Debug\\forexample\\example", "C:\\Users\\Ragul\\Desktop\\test\\setup2\\Debug\\forexample\\example.zip");
	 
	unpackTool.CreateDirFromZip("forexample\\example2", "forexample\\example.zip");
	
	packTool.CreateZipFromDir("forexample\\example\\11.txt", "forexample\\11.zip");
	
	unpackTool.CreateFileFromZip("forexample\\111.txt", "forexample\\11.zip");
	return 0;
}
