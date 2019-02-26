#pragma once
#include"commonfile.h"
#include "Otsu.h"
#include "fileOpenHelper.h"
#include "houghTrans.h"
#include <opencv2/ml/ml.hpp>
#include <opencv2/core/core.hpp>
#include<opencv2/opencv.hpp>
#include<assert.h>
#include<fstream>
using namespace std;
using namespace cv;
using namespace ml;
class test
{

public:
	test();
	~test();
	void testTrainData();
	void testTestData();
	vector<string> testSingleDigitOnA4(string imgPath);
	void train();
	void processA4(CImg<unsigned char> &gray,CImg<unsigned char> & colorful, vector<string> &rowMessages);
	void loadSVM();
	void processFromDir(string dir);
private:
	cv::Ptr<cv::ml::SVM> svm;
	Mat tr_dataMat;
	Mat tr_lablesMat;
	Mat test_dataMat;
	Mat test_lablesMat;
	Mat tr_results;
	Mat test_results;
	void resize(CImg<unsigned char> &gray);
};

