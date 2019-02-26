#pragma once
#include"CImg.h"
#include "houghTrans.h"
#include "Otsu.h"
#include<iostream>
using namespace std;
class A4Refactor
{
public:
	A4Refactor();
	~A4Refactor();
	CImg<unsigned char> RefactProcessing(CImg<unsigned char>& source);
private:
	CImg<unsigned char> _source;
	CImg<unsigned char> _res;
	void resort(vector<int>&posx, vector<int>&y);
};

