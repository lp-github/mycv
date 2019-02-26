#pragma once
#include "mySift.h"
#include"CImg.h"
#include"ImageAssistance.h"
#include<vector>
using namespace std;
using namespace cimg_library;
class myRansac
{
public:
	myRansac();
	~myRansac();
	struct myMat33 {
		double paras[8];
	};
	void trans(struct myMat33& transMat, mySift::KeyPointDescriptor & from, int &tox, int &toy);
	void ransacProcess(vector<ImageAssistance::MatchPair> & matchPairs,myMat33& transMat);
	//void getParas(mySift::KeyPointDescriptor&from, mySift::KeyPointDescriptor&to, myMat33 &mat);
};

