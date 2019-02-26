#pragma once
#include"CImg.h"
#include<assert.h>
using namespace cimg_library;

class Otsu
{
public:
	Otsu();
	~Otsu();
	CImg<unsigned char> ImageSegmentationProcessing(CImg<unsigned char> src);
	CImg<unsigned char> ConvertToGray(CImg<unsigned char>& src);
	CImg<unsigned char> GaussFilt(CImg<unsigned char> &src,int scale = 3,double sigma = 0.5);
private:
	CImg<unsigned char> _source;
	CImg<unsigned char> _result;
	
};

