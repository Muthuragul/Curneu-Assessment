#include "zipHelper.h"
 
//Add the file to the zip file, note that if the source file srcFile is empty, add an empty directory    
 //fileNameInZip: the file name in the zip file, including the relative path    
void cPackFile::AddFileToZip(zipFile zf, string fileNameInZip, string srcFile)
{
	FILE* srcfp = NULL;
 
	 //Initialize file information written to zip
	//zip_fileinfo zi;
	zip_fileinfo   zi = { 0 };
 
	zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
		zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
	zi.dosDate = 0;
	zi.internal_fa = 0;
	zi.external_fa = 0;
 
	 //If srcFile is empty, add an empty directory    
	string new_file_name = "";
	new_file_name += fileNameInZip;
	if (srcFile == "")
	{
		new_file_name += "/";
	}
 
	 //Create a new file in the zip file    
	zipOpenNewFileInZip(zf, new_file_name.c_str(), &zi, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
	
 
	if (srcFile != "")
	{
		 //Open source file    
		fopen_s(&srcfp, srcFile.c_str(), "rb");
		if (srcfp == NULL)
		{
			 printf("Cannot add file: %s !\n", srcFile);
			 zipCloseFileInZip(zf); //Close the zip file  
			return;
		}
 
		 //Read in the source file and write to the zip file    
		char buf[100 * 1024]; //buffer    
		int numBytes = 0;
		while (!feof(srcfp))
		{
			numBytes = fread(buf, 1, sizeof(buf), srcfp);
			zipWriteInFileInZip(zf, buf, numBytes);
			if (ferror(srcfp))
				break;
		}
 
		 //Close the source file    
		fclose(srcfp);
	}
 
	 //Close the zip file    
	zipCloseFileInZip(zf);
}
 
 
 //Recursively add subdirectories to the zip file    
void cPackFile::CollectFilesInDirToZip(zipFile zf, string strPath, string parentDir)
{
	string allfilestr="";
 
	allfilestr += strPath;
	allfilestr += "//*.*";//find all files
 
	long Handle;
	struct _finddata_t FileInfo;
 
	 if ((Handle = _findfirst(allfilestr.c_str(), &FileInfo)) == -1L)//The directory does not exist
		return;
 
	string currentPath;
	string fullPath;
 
	do
	{
		if (strlen(FileInfo.name) == 1 && FileInfo.name[0] == '.'
			|| strlen(FileInfo.name) == 2 && FileInfo.name[0] == '.'&&FileInfo.name[1] == '.')
			 continue;//Current folder upper level directory
 
		if (parentDir.length() != 0)
		{
			fullPath = parentDir;
			fullPath += "/";
			 fullPath += FileInfo.name;//The relative path generated in the zip file
		}
		else
			fullPath += FileInfo.name;
 
		currentPath = strPath;
		currentPath += "//";
		currentPath += FileInfo.name;
 
		if (FileInfo.attrib & _A_SUBDIR)// is a directory
		{
			 AddFileToZip(zf, fullPath, ""); //Generate directory structure in zip file
			 CollectFilesInDirToZip(zf, currentPath, fullPath);//Recursively collect subdirectory files    
			continue;
		}
		 AddFileToZip(zf, fullPath, currentPath); //Add the file to the zip file
	} while (_findnext(Handle, &FileInfo) == 0);//find next file
}
 
 //Compress a directory or file (dirName) into a zipFileName (.zip) file
void cPackFile::CreateZipFromDir(string dirName, string zipFileName)
{
	long Handle;
	struct _finddata_t FileInfo;
 
	 if ((Handle = _findfirst(dirName.c_str(), &FileInfo)) == -1L)//The directory does not exist
		return;
 
	 zipFile newZipFile = zipOpen(zipFileName.c_str(), APPEND_STATUS_CREATE); //Create zip file    
	if (newZipFile == NULL)
	{
		 printf("\nUnable to create zip file!");
		return;
	}
 
	
 
	if (FileInfo.attrib & _A_SUBDIR)// is a directory
	{
		AddFileToZip(newZipFile, FileInfo.name, "");
		CollectFilesInDirToZip(newZipFile, dirName, FileInfo.name);
	}
	else// is a file
		AddFileToZip(newZipFile, FileInfo.name, dirName);
 
	 zipClose(newZipFile, NULL); //Close the zip file    
}
 
cPackFile::cPackFile()
{
 
}
 
cPackFile::~cPackFile()
{
 
}
 
 
 
static inline bool IsFileExist(string pPath)
{
	//return GetFileAttributesA(pPath.c_str()) != INVALID_FILE_ATTRIBUTES;
	std::ifstream ifile(pPath);
	return (bool)ifile;
}
 
void cUnpackFile::CreateDirFromZip(string dirName, string zipFileName)
{
	bool slashFlag = true;
	unzFile unZipDir = unzOpen(zipFileName.c_str());
 
	if (unZipDir == NULL)
	{
		 printf("Unable to decompress the zip file!");
		return;
	}
 
	if (!IsFileExist(dirName))
		_mkdir(dirName.c_str());
 
	int nResult = unzGoToFirstFile(unZipDir);
	unsigned char * buf;
 
	while (nResult == UNZ_OK)
	{
		char szCurrentFile[260];
		unz_file_info unZipFileInfo;
		unzGetCurrentFileInfo(unZipDir, &unZipFileInfo, szCurrentFile, sizeof(szCurrentFile), NULL, 0, NULL, 0);
 
		std::string filePath = std::string(szCurrentFile);
		std::string fileName;
		string fileRoot;
		unsigned last_slash_pos = filePath.find_last_of('\\');
		if (last_slash_pos == std::string::npos) {
			last_slash_pos = filePath.find_last_of('/');
			slashFlag = false;
		}
		if (last_slash_pos != std::string::npos)
		{
			fileRoot = dirName;
			if (slashFlag)
				fileRoot += "\\";
			else
				fileRoot += "/";
			std::string filetemp = filePath.substr(0, last_slash_pos);
			fileRoot += filetemp;
 
			if (!IsFileExist(fileRoot))
				_mkdir(fileRoot.c_str());
			if (last_slash_pos == filePath.length() - 1)
			{
				nResult = unzGoToNextFile(unZipDir);
				 continue;//Folder item
			}
		}
 
		int size = unZipFileInfo.uncompressed_size;
		buf = new unsigned char[size];
		if (!buf)
		{
			printf("No enough Memory!\n");
			return;
		}
		memset(buf, 0, size*sizeof(char));
		int nReadBytes;
		if (UNZ_OK == unzOpenCurrentFilePassword(unZipDir, NULL))
		{
			nReadBytes = unzReadCurrentFile(unZipDir, buf, size);
			unzCloseCurrentFile(unZipDir);
		}
		FILE * pFile;
		string filePos;
		filePos = dirName;
		if (slashFlag)
			filePos += "\\";
		else
			filePos += "/";
 
		filePos += filePath;
 
		fopen_s(&pFile, filePos.c_str(), "wb");
		if (pFile)
			fwrite(buf, nReadBytes, 1, pFile);
		else
			 printf("Cannot open output file %s \n", filePos);
 
		fclose(pFile);
		delete buf;
 
		nResult = unzGoToNextFile(unZipDir);
	}
	unzClose(unZipDir);
}
 
void cUnpackFile::CreateFileFromZip(string fName, string zipFileName)
{
	bool slashFlag = true;
	unzFile unZipDir = unzOpen(zipFileName.c_str());
 
	if (unZipDir == NULL)
	{
		 printf("Unable to decompress the zip file!");
		return;
	}
 
	int nResult = unzGoToFirstFile(unZipDir);
	unsigned char * buf;
 
	if (nResult == UNZ_OK)
	{
		char szCurrentFile[260];
		unz_file_info unZipFileInfo;
		unzGetCurrentFileInfo(unZipDir, &unZipFileInfo, szCurrentFile, sizeof(szCurrentFile), NULL, 0, NULL, 0);
 
		int size = unZipFileInfo.uncompressed_size;
		buf = new unsigned char[size];
		if (!buf)
		{
			printf("No enough Memory!\n");
			return;
		}
		memset(buf, 0, size*sizeof(char));
		int nReadBytes;
		if (UNZ_OK == unzOpenCurrentFilePassword(unZipDir, NULL))
		{
			nReadBytes = unzReadCurrentFile(unZipDir, buf, size);
			unzCloseCurrentFile(unZipDir);
		}
		FILE * pFile;
		fopen_s(&pFile, fName.c_str(), "wb");
		if (pFile)
			fwrite(buf, nReadBytes, 1, pFile);
		else
			 printf("Cannot open output file %s \n", fName);
 
		fclose(pFile);
		delete buf;
	}
	unzClose(unZipDir);
}
 
cUnpackFile::cUnpackFile()
{
 
}
 
cUnpackFile::~cUnpackFile()
{
 
}
