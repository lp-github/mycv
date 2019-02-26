#include "test.h"



test::test()
{
}


test::~test()
{
}

void test::testTrainData()
{
	svm->predict(tr_dataMat, tr_results);
	auto row = tr_lablesMat.rows;
	int success = 0;
	for (int i = 0; i < row; i++) {
		//compare the labels
		int preLabel = tr_results.at<float>(i, 0);
		int realLabel = tr_lablesMat.at<int>(i, 0);
		if (preLabel == realLabel) {
			//cout << preLabel << endl;
			success++;
		}
	}
	cout << "train label size:" << tr_lablesMat.rows << endl;
	cout << "train data size:" << tr_dataMat.rows << endl;
	cout << "succsses times:\t" << success << endl;
	cout << "train data test   success rate:" << success * 1.0 / row;
}

void test::testTestData()
{
	svm->predict(test_dataMat, test_results);
	assert(test_results.rows == test_lablesMat.rows);
	assert(test_results.rows > 0);
	auto row = test_lablesMat.rows;
	int success = 0;
	for (int i = 0; i < row; i++) {
		//compare the labels
		int preLabel = test_results.at<float>(i, 0);
		int realLabel = test_lablesMat.at<int>(i, 0);
		if (preLabel == realLabel) {
			success++;
		}
	}
	cout << "test data test   success rate:" << success *1.0/ row;
}

vector<string> test::testSingleDigitOnA4(string filename)
{
	vector<string> rowMessages;
	int index = filename.find_last_of('\\')+1;
	string resfilename = filename.substr(index, filename.length() - index);
	rowMessages.push_back(resfilename);
	//read sample image
			//auto filename = "testSet\\ImageData\\IMG_20181217_135840.jpg";
	auto img = imread(filename);
	cv::resize(img, img, Size(600, 800), 0.0, 0.0, INTER_AREA);
	//rowMessages.push_back()
	//convert to cimg
	CImg<unsigned char> src = convertFromMatToCImg(img);
	
	stringstream ss;
	
	//segmentation;
	Otsu otsu;
	cout << "start segmentation" << endl;
	auto seg = otsu.ImageSegmentationProcessing(src);
	cout << "segmentation finished" << endl;
	//seg.display();
	/*
	cout << "remove small area start..." << endl;
	//throwAwaySmallArea(seg);						//this procedure cost too much time and is not neccessary in part one
	cout << "remove small area end..." << endl;
	*/
	//fushi A4
	CImg<unsigned char> temp(seg.width(), seg.height(), 1, 1);
	cimg_forXY(seg, x, y) {
		int depressed = 0;
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j++) {
				int xa = x + i;
				int ya = y + j;
				if (xa < 0 || xa >= seg.width() || ya < 0 || ya >= seg.height()) {
					depressed++;
					continue;
				}
				if (seg(xa, ya) == 255) {
					depressed++;
				}
			}
		}
		temp(x, y) = depressed == 9 ? 0 : seg(x, y);
	}
	//temp.display();
	//hough trans to detect edge
	cout << "hough trans start..." << endl;
	houghTrans ht;
	auto htimg = ht.hough_trans(temp, 200);
	auto htdeimg = ht.draw_edge();
	//htimg.display();
	ht.paint_point();
	cout << "hough trans finished, you have got " << ht.pointx.size() << " point(s)" << endl;
	//reorder the point 
	/*
	0 1
	2 3
	*/
	auto pointx = ht.pointx;
	auto pointy = ht.pointy;
	vector<int> orderx, ordery;
	orderx.reserve(4);
	ordery.reserve(4);
	if (pointx.size() != 4) {
		rowMessages.push_back("hough trans failed to find 4 point exactly");
		return rowMessages;
	}
	//assert(pointx.size() == 4 && pointy.size() == 4);
	vector<int> sum;
	cout << "start process A4 paper correction..." << endl;
	for (int i = 0; i < 4; i++) {
		int temp = pointx[i] + pointy[i];
		sum.push_back(temp);
	}
	//reorder by pop-queue small to large
	for (int i = 3; i > 0; i--) {
		for (int j = 0; j < i; j++) {
			if (sum[j] > sum[j + 1]) {
				//exchange
				sum[j] = sum[j] + sum[j + 1];
				sum[j + 1] = sum[j] - sum[j + 1];
				sum[j] = sum[j] - sum[j + 1];

				pointx[j] = pointx[j] + pointx[j + 1];
				pointx[j + 1] = pointx[j] - pointx[j + 1];
				pointx[j] = pointx[j] - pointx[j + 1];

				pointy[j] = pointy[j] + pointy[j + 1];
				pointy[j + 1] = pointy[j] - pointy[j + 1];
				pointy[j] = pointy[j] - pointy[j + 1];
			}
		}
	}
	if (pointy[1] > pointy[2]) {//reorder has reached mistake
		pointy[1] = pointy[1] + pointy[2];
		pointy[2] = pointy[1] - pointy[2];
		pointy[1] = pointy[1] - pointy[2];

		pointx[1] = pointx[1] + pointx[2];
		pointx[2] = pointx[1] - pointx[2];
		pointx[1] = pointx[1] - pointx[2];
	}
	//calculate paras
	int xa, xb, xc, xd, ya, yb, yc, yd;
	xa = pointx[0];
	xb = pointx[1];
	xc = pointx[2];
	xd = pointx[3];
	ya = pointy[0];
	yb = pointy[1];
	yc = pointy[2];
	yd = pointy[3];
	/*cout << xa << "," << ya << "\t" << xb << "," << yb << endl;
	cout << xc << "," << yc << "\t" << xd << "," << yd << endl;*/
	ss.clear();
	string tempstring;
	ss << "\"" << xa << "," << ya<<"\"";
	ss >> tempstring;
	tempstring.insert((size_t)(tempstring.find_last_of(','))+1, 1,' ');
	rowMessages.push_back(tempstring);
	ss.clear();
	ss<< "\"" << xb << "," << yb << "\"";
	ss >> tempstring;
	tempstring.insert((size_t)(tempstring.find_last_of(','))+1, 1, ' ');
	rowMessages.push_back(tempstring);
	ss.clear();
	ss << "\"" << xc << ","  << yc << "\"";
	ss >> tempstring;
	tempstring.insert((size_t)(tempstring.find_last_of(','))+1, 1, ' ');
	rowMessages.push_back(tempstring);
	ss.clear();
	ss << "\"" << xd << "," << yd << "\"";
	ss >> tempstring;
	tempstring.insert((size_t)(tempstring.find_last_of(','))+1, 1, ' ');
	rowMessages.push_back(tempstring);
	//rowMessages.push_back()
	//calculate trans matrix
	//while (1);
	double a, b, c, d, e, f, g, h;
	CImg<unsigned char> result(600,800,1,3);
	int x1 = 0, x3 = 0, y1 = 0, y2 = 0, x2 = result.width() - 1, x4 = result.width() - 1, y3 = result.height() - 1, y4 = result.height() - 1;

	CImg<float> A(4, 4, 1, 1);
	A(0, 0) = x1;
	A(0, 1) = x2;
	A(0, 2) = x3;
	A(0, 3) = x4;

	A(1, 0) = y1;
	A(1, 1) = y2;
	A(1, 2) = y3;
	A(1, 3) = y4;

	A(2, 0) = x1 * y1;
	A(2, 1) = x2 * y2;
	A(2, 2) = x3 * y3;
	A(2, 3) = x4 * y4;

	A(3, 0) = 1;
	A(3, 1) = 1;
	A(3, 2) = 1;
	A(3, 3) = 1;

	CImg<float> bv(1, 4, 1, 1), cv(1, 4, 1, 1);
	bv(0, 0) = xa;
	bv(0, 1) = xb;
	bv(0, 2) = xc;
	bv(0, 3) = xd;

	cv(0, 0) = ya;
	cv(0, 1) = yb;
	cv(0, 2) = yc;
	cv(0, 3) = yd;

	auto xParas = bv.get_solve(A);
	auto yParas = cv.get_solve(A);
	a = xParas(0, 0);
	b = xParas(0, 1);
	c = xParas(0, 2);
	d = xParas(0, 3);

	e = yParas(0, 0);
	f = yParas(0, 1);
	g = yParas(0, 2);
	h = yParas(0, 3);
	////trans from source to result
	cimg_forXY(result, x, y) {
		double xsrc = a * x + b * y + c * x*y + d;
		double ysrc = e * x + f * y + g * x*y + h;
		//pa pb
		//pc pd
		
		//result(x, y) = newsrc(xsrc,ysrc);

		result(x, y, 0) = src(xsrc,ysrc,0);
		result(x, y, 1) = src(xsrc, ysrc, 1);
		result(x, y, 2) = src(xsrc, ysrc, 2);
	}
	auto tempfilename = filename.substr(filename.find_last_of('\\')+1, filename.length() - filename.find_last_of('\\'));
	tempfilename = tempfilename.substr(0, tempfilename.length()-4);
	result.save(("A4_correction\\" + tempfilename + ".bmp").c_str());
	cout << "A4 pager correction finished" << endl;
	//result.display();
	CImg<unsigned char> colorful(result);
	result._spectrum = 1;
	//result.display();
	cout << "start processing remove effect of light... " << endl;
	removeShadow(result);
	//result.display();
	//reduce the side black
	cimg_forXY(result, x, y) {
		if (x <= 10 || y <= 10 || x >= result.width() - 10 || y >= result.height() - 10) {
			result(x, y) = 255;
		}
	}
	result.save_bmp(("threshold\\" + tempfilename + ".bmp").c_str());
	cout << "light effect removing finished" << endl;
	//result.display();
	
	cout << "start processing split and predict..." << endl;
	processA4(result,colorful, rowMessages);
	cout << "finished split and predict" << endl;
	return rowMessages;
}

void test::train()
{
	char tr_imagedb[] = "train-images.idx3-ubyte";
	char tr_labledb[] = "train-labels.idx1-ubyte";
	char test_imagedb[] = "t10k-images.idx3-ubyte";
	char test_labledb[] = "t10k-labels.idx1-ubyte";
	//imread()
	//Mat a(Size(0, 0), 0, 0);
	vector < vector<float> > tr_data(n_trimg, vector<float>(n_features));
	vector <int> tr_lables(n_trimg);
	vector < vector<float> > test_data(n_testimg, vector<float>(n_features));
	vector <int> test_lables(n_testimg);

	// Reading Databases
	cout << "Reading Databases...\n";
	tr_data = read_images_extract_features(tr_imagedb, n_trimg);
	cout << "fine traing data...\n";
	tr_lables = read_lables(tr_labledb, n_trimg);
	cout << "fine traing labels...\n";
	test_data = read_images_extract_features(test_imagedb, n_testimg);
	cout << "fine testing data...\n";
	test_lables = read_lables(test_labledb, n_testimg);
	cout << "fine testing labels...\n";

	/**------------------------------------------------------------------------------SVM---------------------------------------------------------------------------------------**/

	cout << "Running SVM...\n";
	//Putting all data to Matrix of opencv
	 tr_dataMat=Mat(n_trimg, n_features, CV_32FC1);
	 tr_lablesMat=Mat(n_trimg, 1, CV_32SC1);
	 test_dataMat=Mat(n_testimg, n_features, CV_32FC1);
	 test_lablesMat=Mat(n_testimg, 1, CV_32SC1);
	 tr_results= Mat(n_trimg, 1, CV_32SC1);
	 test_results= Mat(n_testimg, 1, CV_32SC1);
	//gettype
	//auto dddd = tr_results.data;
	//uchar* aaaa;

	//auto a = tr_results.at()
	for (int i = 0; i<n_trimg; ++i)
	{
		for (int j = 0; j<n_features; ++j)
		{
			tr_dataMat.at<float>(i, j) = tr_data[i][j];
		}
	}

	for (int i = 0; i<n_trimg; ++i)
	{
		tr_lablesMat.at<int>(i, 0) = tr_lables[i];
	}

	for (int i = 0; i<n_testimg; ++i)
	{
		for (int j = 0; j<n_features; ++j)
		{
			test_dataMat.at<float>(i, j) = test_data[i][j];
		}
	}
	for (int i = 0; i<n_testimg; ++i)
	{
		test_lablesMat.at<int>(i, 0) = test_lables[i];
	}

	//cout<<tr_dataMat<<"\n\n\n";
	//cout<<tr_lablesMat<<"\n\n\n";
	//cout<<test_dataMat<<"\n\n\n";
	//cout<<test_lablesMat<<"\n\n\n";

	// parameters for SVM
	/* to be depressed
	SVM::ParamTypes params;
	params = SVM::ParamTypes::
	params.svm_type    =ml:: CvSVM::NU_SVC;
	params.kernel_type = CvSVM::POLY;
	params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);

	params.degree        = CvSVM::POLY;
	params.gamma         = CvSVM::POLY;
	params.coef0         = CvSVM::POLY;
	params.nu            = 0.1;
	params.p             = CvSVM::EPS_SVR;
	*/
	svm = SVM::create();
	//cv::Ptr<cv::ml::SVM>
	/*svm->setType(ml::SVM::NU_SVC);
	svm->setKernel(ml::SVM::POLY);
	svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 1e-6));
	svm->setGamma(SVM::POLY);
	svm->setDegree(SVM::POLY);
	svm->setCoef0(SVM::POLY);
	svm->setP(SVM::EPS_SVR);
	svm->setNu(0.1);
	*/
	/*
	svm->setType(ml::SVM::C_SVC);
	svm->setKernel(ml::SVM::RBF);
	svm->setDegree(10.0);
	svm->setGamma(8.0);
	svm->setCoef0(1.0);
	svm->setC(10.0);
	svm->setNu(0.5);
	svm->setP(0.1);
	*/
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::LINEAR);
	svm->setC(2.67);
	svm->setGamma(5.383);
	
	svm->train(tr_dataMat, cv::ml::SampleTypes::ROW_SAMPLE, tr_lablesMat);
	//svm->train(tr_dataMat,)
	svm->save("testSet\\ImageData\\svmmnist.xml");
}

void test::loadSVM()
{
	string svmpath = "testSet\\ImageData\\svmmnist.xml";
	svm = Algorithm::load<ml::SVM>(svmpath);
}

void test::processFromDir(string dir)
{
	//train();
	if (svm == nullptr) {
		svm = Algorithm::load<ml::SVM>("testSet\\ImageData\\svmmnist.xml");
	}
	if (svm == nullptr) {
		cout << "cannot find xml file of svm,may be you need train the svm first?" << endl;
		while (1);
	}
	

	fileOpenHelper fh;
	vector<string> result;
	fh.getFilePath(dir, result, true, "*.jpg");
	fstream fout;
	fout.open("data.csv", ios::out);
	/*fstream fout;
	fout.open("data.csv", ios::out);*/
	fout << "文件名" <<','<< "角点1" <<','<< "角点2"<<',' << "角点3" <<','<< "角点4" <<','<< "学号" <<','<< "手机号" <<','<< "身份证号" << endl;
	for (int i = 0; i < result.size(); i++) {
		//cout << "path retrieve loop start" << endl;
		auto path = result[i];
		cout << path << endl;
		auto mess = testSingleDigitOnA4(path);
		
		for (int j = 0; j < mess.size(); j++) {
			if (j == 8)break;
			cout << mess[j] << endl;
			fout << mess[j] << "\t,";
		}
		fout << endl;
		cout << endl << endl;
	}
	fout.close();
	cout << "process all image under this path" << endl;
}

CImg<unsigned char> convertFromMatToCImg(Mat mat)
{
	int row = mat.rows;
	int col = mat.cols;

	CImg<unsigned char> res(col,row,1,mat.channels());

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (mat.channels() == 3) {
				auto b = mat.at<Vec3b>(i, j)[0];
				auto g = mat.at<Vec3b>(i, j)[1];
				auto r = mat.at<Vec3b>(i, j)[2];
				res(j, i, 0) = r;
				res(j, i, 1) = g;
				res(j, i, 2) = b;
			}
			else if (mat.channels() == 1) {
				res(j, i) = mat.at<uchar>(i, j);
			}
			else {
				assert(false);
			}
		}
	}
	return res;
}

void setFlag(const CImg<unsigned char>& src, int ** & flagOnPos,int x,int y , int flagNumber)
{
	if (x < 0 || y < 0 || x >= src.width() || y >= src.height()) {//out of range
		return;
	}
	if (src(x, y) == 0) {//black pixels and cannot be set;
		return;
	}
	if (flagOnPos[y][x] != -1)//already set flag
		return;
	//white pixel and not set flag
	//flagOnPos.at
	flagOnPos[y][x] = flagNumber;
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			if (i == 0 && j == 0)continue;
			int xa = x + i;
			int ya = y + j;
			setFlag(src, flagOnPos, xa, ya, flagNumber);
		}
	}
}

void test::processA4(CImg<unsigned char>& result,CImg<unsigned char> & colorful,vector<string>& rowMessages)
{	
	string filename = rowMessages[0];
	auto tempfilename = filename.substr(filename.find_last_of('\\') + 1, filename.length() - filename.find_last_of('\\'));
	tempfilename = tempfilename.substr(0, tempfilename.length() - 4);

	string studentNumber = "", phoneNumber = "", privateIdentityNumber = "";
	stringstream ss;
	int *rowsum = new int[result.height()];
	for (int i = 0; i < result.height(); i++) {
		rowsum[i] = 0;
	}
	//int *colsum = new int[result.width()];
	cimg_forXY(result, x, y) {
		rowsum[y] += result(x, y) == 0 ? 1 : 0;
	}
	vector<int> marginUP;
	vector<int> marginDown;
	for (int i = 0; i < result.height() - 1; i++) {
		if (rowsum[i] == 0 && rowsum[i + 1] > 0) {//up
			marginUP.push_back(i + 1);
		}
		else if (rowsum[i] > 0 && rowsum[i + 1] == 0) {//down
			marginDown.push_back(i);
			if (marginDown.back() - marginUP.back() <= 5) {
				marginUP.pop_back();
				marginDown.pop_back();
			}
		}
	}
	assert(marginUP.size() == marginDown.size());//there are 3 rows of numbers;
	//vector<CImg<unsigned char>> rowColorfuls;
	vector<CImg<unsigned char> > rowImages;
	CImg<unsigned char> saveImg(result);
	for (int i = 0; i < marginUP.size(); i++) {
		CImg<unsigned char> temp(result.width(), marginDown[i] - marginUP[i] + 1, 1, 1);
		
		for (int i = 0; i < saveImg.width(); i++) {
			for (int j = 0; j < marginUP.size(); j++) {
				int rownumber = marginUP[j];
				saveImg(i, rownumber) = 0;
				rownumber = marginDown[j];
				saveImg(i, rownumber) = 0;
			}

		}
		
		cimg_forXY(temp, x, y) {
			temp(x, y) = result(x, y + marginUP[i]);
		}
		//temp.display();
		rowImages.push_back(temp);
	}
	saveImg.save_bmp(("row_div\\" + tempfilename + ".bmp").c_str());//save row divide pics

	delete[] rowsum;
	rowsum = nullptr;
	//col divise
	saveImg = result;
	for (int i = 0; i < rowImages.size(); i++) {
		auto temp = rowImages[i];
		//auto tempcolorful = rowColorfuls[i];
		int * colsum = new int[temp.width()];
		for (int j = 0; j < temp.width(); j++) {
			colsum[j] = 0;
		}
		cimg_forXY(temp, x, y) {
			colsum[x] += temp(x, y) == 0 ? 1 : 0;//black
		}
		vector<int> marginLeft;
		vector<int> marginRight;
		for (int j = 0; j < temp.width() - 1; j++) {
			if (colsum[j] == 0 && colsum[j + 1] > 0) {
				marginLeft.push_back(j + 1);
			}
			else if (colsum[j] > 0 && colsum[j + 1] == 0) {
				marginRight.push_back(j);
				if (1) {//test anyway
					int temppixelnum = 0;
					for (int k = marginLeft.back(); k <= marginRight.back(); k++) {
						temppixelnum += colsum[k];
					}
					if (temppixelnum < 20) {//this area has no more than 5 pixels;
						marginLeft.pop_back();
						marginRight.pop_back();
					}
				}
			}
		}
		assert(marginLeft.size() == marginRight.size());//assert there are 8 number of student numbers
		vector<CImg<unsigned char>> colImages;
		for (int j = 0; j < marginLeft.size(); j++) {
			int uprow = marginUP[i];
			int downrow = marginDown[i];
			int leftcol = marginLeft[j];
			int rightcol = marginRight[j];
			for (int m = leftcol; m <= rightcol; m++) {
				saveImg(m, uprow) = 0;
				saveImg(m, downrow) = 0;
			}
			for (int m = uprow; m <= downrow; m++) {
				saveImg(leftcol, m) = 0;
				saveImg(rightcol, m) = 0;
			}
			//cout << "left" << marginLeft[j] << "\tright:" << marginRight[j];
			CImg<unsigned char> local(marginRight[j] - marginLeft[j] + 1, temp.height(), 1, 1);
			cimg_forXY(local, x, y) {
				local(x, y) = temp(x+marginLeft[j], y);
			}
			local._spectrum = 1;
			cimg_forXY(local, x, y) {
				local(x, y) = 255 - local(x, y);
			}
			//local.display();
			auto f = getFeatureFromSingleImage(local);
			auto fmat = Mat(1, n_features, CV_32FC1);
			for (int k = 0; k < n_features; k++) {
				fmat.at<float>(0, k) = f[k];
			}
			auto l = Mat(1, 1, CV_32SC1);
			//cout << fmat.cols << endl;
			//cout<<svm->getVarCount()<<endl;
			//cout << fmat.type() << endl;
			svm->predict(fmat, l);
			
			auto label = l.at<float>(0, 0);
			ss << (int)label;
			//studentNumber = studentNumber * 10 + (int)label;
			//cout << "label" << label << endl;
			//local.display();
			//local.display();
			colImages.push_back(local);
		}
		ss >> studentNumber;
		rowMessages.push_back(studentNumber);
		ss.clear();
	}
	saveImg.save_bmp(("col_div\\" + tempfilename + ".bmp").c_str());//save row divide pics
	//samples.cols == var_count && samples.type() == CV_32F
}

void removeShadow(CImg<unsigned char>& gray)
{
	//calculate background
	assert(gray.spectrum() == 1);
	CImg<unsigned char> background(gray.width(), gray.height(), 1, 1);
	cimg_forXY(gray, x, y) {
		vector<int> pixels;
		//list<int> li;
		

		int max = 0;
		int second = 0;
		for (int i = -2; i <= 2; i++) {
			for (int j = -2; j <= 2; j++) {
				int xa = x + i;
				int ya = y + j;
				if (xa < 0 || ya < 0 || xa >= gray.width() || ya >= gray.height())continue;
				int pix = gray(xa, ya);
				//pixels.push_back(pix);
				if (pix > max) {
					second = max;
					max = pix;
				}
				else if (pix > second) {
					second = pix;
				}
			}
		}
		
		
		
		//for(int i = 0)
		background(x, y) = (second+max) / 2;
	}
	//background.display();
	
	cimg_forXY(gray, x, y) {
		int delta = abs(background(x, y) - gray(x, y));
		if (delta < 10) {//background
			gray(x, y) = 255;
		}
		else {
			gray(x, y) = 0;
		}
	}
	//gray.display();
}

void splitSegmentation(CImg<unsigned char>& gray, int rowdiv, int coldiv)
{
	Otsu otsu;
	vector < vector<CImg<unsigned char>>> dividedImages;
	int row = gray.height() / rowdiv;
	int col = gray.width() / coldiv;
	for (int i = 0; i < rowdiv; i++) {
		for (int j = 0; j < coldiv; j++) {//totally rowdiv*coldiv pics
			CImg<unsigned char>  temp(col,row,1,1);
			cimg_forXY(temp, x, y) {
				temp(x, y) = gray(x + j * col, y + i * row);
			}
			temp = otsu.ImageSegmentationProcessing(temp);
		}
	}
	for (int i = 0; i < rowdiv; i++) {
		for (int j = 0; j < coldiv; j++) {

		}
	}
}

void throwAwaySmallArea(CImg<unsigned char>& src)
{
	//after min-max
	assert(src.spectrum() == 1);
	int flagnumber = 0;
	int** flagOnPos = new int*[src.height()];
	
	//flagOnPos.
	for (int i = 0; i < src.height(); i++) {
		flagOnPos[i] = new int[src.width()];
		for (int j = 0; j < src.width(); j++) {
			flagOnPos[i][j] = -1;
		}
	}

	vector<int> sumOnFlag;
	int loopcycle = 0;
	cimg_forXY(src, x, y) {
		//setFlag(src, flagOnPos, x, y, flagnumber);
		if (src(x, y) == 0 || flagOnPos[y][x] != -1)continue;
		loopcycle++;
		vector<int> stackx, stacky;
		stackx.push_back(x);
		stacky.push_back(y);
		int stackloop = 0;
		while (stackx.size() != 0) {
			stackloop++;
			int xpos = stackx.back();
			int ypos = stacky.back();
			stackx.pop_back();
			stacky.pop_back();
			flagOnPos[ypos][xpos] = flagnumber;
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					if (i == 0 && j == 0)continue;
					if (xpos + i < 0 || ypos+j < 0 || xpos+i >= src.width() || ypos+j >= src.height())continue;
					if (src(xpos + i, ypos + j) == 0)continue;
					if (flagOnPos[ypos + j][xpos + i] != -1)continue;
					stackx.push_back(xpos + i);
					stacky.push_back(ypos + j);
				}
			}
		}
		//cout << "stackloop" << stackloop << endl;
		flagnumber++;
	}
	//cout << "loop cycle" << loopcycle << endl;
	//cout << "flagnumber:" << flagnumber << endl;
	sumOnFlag.resize(flagnumber);
	cimg_forXY(src, x, y) {
		int flag = flagOnPos[y][x];
		if (flag == -1)continue;
		while (flag > sumOnFlag.size() - 1) {
			sumOnFlag.push_back(0);
		}
		
		sumOnFlag[flag]++;
		
	}
	
	int maxIndex = -1;
	int maxSum = -1;
	for (int i = 0; i < sumOnFlag.size(); i++) {
		//cout << i << ":\t" << sumOnFlag[i] << endl;
		if (sumOnFlag[i] > maxSum) {
			maxSum = sumOnFlag[i];
			maxIndex = i;
		}
	}
	//cout << "maxindex:\t" << maxIndex << endl;
	//cout << "maxsum:\t" << maxSum << endl;
	int flag = maxIndex;

	cimg_forXY(src, x, y) {
		if (flagOnPos[y][x] == flag) {
			continue;
		}
		else {
			src(x, y) = 0;
		}
	}
	//cout << "end" << endl;
	//memory deallocate
	for (int i = 0; i < src.height(); i++) {
		delete []flagOnPos[i];
		flagOnPos[i] = nullptr;
	}
	delete[]flagOnPos;
	flagOnPos = nullptr;
	//while (1);
	//end;
}

void expansion(CImg<unsigned char>& src)
{
	assert(src.spectrum() == 1);
	CImg<unsigned char> temp(src.width(), src.height(), 1, 1);
	cimg_forXY(src, x, y) {
		assert(src(x, y) == 0 || src(x, y) == 255);
		bool haswhite = false;
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j++) {
				int xa = x + i;
				int ya = y + j;
				if (xa < 0 || ya < 0 || xa >= src.width() || ya >= src.height()) {
					haswhite = true;
					i = 10;
					break;
				}
				if (src(xa, ya) == 255) {
					haswhite= true;
					i = 10;
					break;
				}
			}
		}
		if (haswhite) {
			temp(x, y) = 255;
		}
		else {
			temp(x,y) = src(x, y);
		}
	}
	cimg_forXY(src, x, y) {
		src(x, y) = temp(x, y);
	}
}

void erosion(CImg<unsigned char>& src)
{
	assert(src.spectrum() == 1);
	CImg<unsigned char> temp(src.width(), src.height(), 1, 1);
	cimg_forXY(src, x, y) {
		assert(src(x, y) == 0 || src(x, y) == 255);
		bool hasblack = false;
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j++) {
				int xa = x + i;
				int ya = y + j;
				if (xa < 0 || ya < 0 || xa >= src.width() || ya >= src.height()) {
					hasblack = true;
					i = 10;
					break;
				}
				if (src(xa, ya) == 0) {
					hasblack = true;
					i = 10;
					break;
				}
			}
		}
		if (hasblack) {
			temp(x, y) = 0;
		}
		else {
			temp(x, y) = src(x, y);
		}
	}
	cimg_forXY(src, x, y) {
		src(x, y) = temp(x, y);
	}
}
