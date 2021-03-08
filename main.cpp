#include <Windows.h>
#include <iostream>
#include "zipHelper.h"
using namespace std;
 
int main(int argc, char** argv) {
	cPackFile packTool;
	cUnpackFile unpackTool;
	 //printf("Compress the example folder and its subfiles in the forexample folder to the example.zip file in the forexample folder\n");
	packTool.CreateZipFromDir("C:\\Users\\admin\\Desktop\\test\\setup2\\Debug\\forexample\\example", "C:\\Users\\admin\\Desktop\\test\\setup2\\Debug\\forexample\\example.zip");
	 //printf("Unzip the example.zip file under the forexample folder to the example2 folder under the forexample folder\n");
	unpackTool.CreateDirFromZip("forexample\\example2", "forexample\\example.zip");
	 //printf("Compress the forexample\\example\\11.txt file into the forexample\\11.zip file\n");
	packTool.CreateZipFromDir("forexample\\example\\11.txt", "forexample\\11.zip");
	 //printf("Unzip the forexample\\11.zip file to the forexample\\111.txt file\n");
	unpackTool.CreateFileFromZip("forexample\\111.txt", "forexample\\11.zip");
	return 0;
}
