#pragma once
#ifndef ZIP_H
#define ZIP_H
#include <iostream>
#include <fstream>
#include <string>
#include <io.h>
#include <direct.h>
#include "zlib\unzip.h"
#include <zlib\zip.h>
using namespace std;
 
class cPackFile
{
public:
	 //Compress a directory or file (dirName) into a zipFileName (.zip) file
	void CreateZipFromDir(string dirName, string zipFileName);
	cPackFile();
	~cPackFile();
private:
	 //Recursively add subdirectories to the zip file  
	void CollectFilesInDirToZip(zipFile zf, string strPath, string parentDir);
 
	void AddFileToZip(zipFile zf, string fileNameInZip, string srcFile);
};
 
static inline bool IsFileExist(string pPath);
 
class cUnpackFile
{
public:
	cUnpackFile();
	~cUnpackFile();
	 void CreateDirFromZip(string dirName, string zipFileName);//Unzip to folder dirName
	 void CreateFileFromZip(string fName, string zipFileName);//Unzip the first file to file fName
};
 
#endif
