#pragma once
#include"CImg.h"
#include"ImageAssistance.h"
#include"myRansac.h"
using namespace cimg_library;
class myStitch
{
public:
	myStitch();
	~myStitch();
	CImg<unsigned char> stitch(CImg<unsigned char> &pic1, CImg<unsigned char> &pic2);
private:

};

