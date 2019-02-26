#pragma once
#include<vector>
#include<iostream>
#include<io.h>
#include<string>
#include<direct.h>
using namespace std;
class fileOpenHelper
{
public:
	fileOpenHelper();
	~fileOpenHelper();
	void getFilePath(string filepath,vector<string>&result, bool recursive, string filter);
};

