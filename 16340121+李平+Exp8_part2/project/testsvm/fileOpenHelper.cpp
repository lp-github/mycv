#include "fileOpenHelper.h"



fileOpenHelper::fileOpenHelper()
{
}


fileOpenHelper::~fileOpenHelper()
{
}

void fileOpenHelper::getFilePath(string mainDir,vector<string>& files, bool recursive, string filter)
{
	//文件句柄  
	//mainDir = "D:\\ftp";
	files.clear();
	const char *dir = mainDir.c_str();
	_chdir(dir);
	intptr_t hFile;
	_finddata_t fileinfo;

	if ((hFile = _findfirst(filter.c_str(), &fileinfo)) != -1)
	{
		do
		{
			if (!(fileinfo.attrib & _A_SUBDIR))//找到文件
			{
				char filename[_MAX_PATH];
				strcpy_s(filename, dir);
				strcat_s(filename, "\\");
				strcat_s(filename, fileinfo.name);
				string temfilename = filename;
				//cout << temfilename << endl;
				files.push_back(temfilename);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		//cout << "while end" << endl;
		_findclose(hFile);
	}
}
