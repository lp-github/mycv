#include "Otsu.h"



Otsu::Otsu()
{
}


Otsu::~Otsu()
{
}

CImg<unsigned char> Otsu::GaussFilt(CImg<unsigned char>& src,int scale,double sigma)
{
	assert(src.spectrum() == 1);
	double PI = 3.141592653589;
	sigma = 1.5;
	double constant = 2 * sigma*sigma;
	//filter[x + row / 2][y + column / 2] = (exp(-(coordSum) / constant)) / (M_PI * constant);
	double filter[3][3];
	double sum = 0;
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			filter[i + 1][j + 1] = exp(-(i*i + j * j) / constant) / (PI * constant);
			sum += filter[i+1][j + 1];
		}
	}
	assert(sum != 0);
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			filter[i + 1][j + 1] /= sum;
			
		}
	}
	CImg<unsigned char> res(src.width(), src.height(), 1, 1);
	cimg_forXY(src, x, y) {
		if (x == 0 || y == 0 || x == src.width() - 1 || y == src.height() - 1) {
			res(x, y) = src(x, y);
			continue;
		}
		double pix = 0;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				int xa = x + i - 1;
				int ya = y + j - 1;
				pix += src(xa, ya)*filter[i][j];
			}
		}
		res(x, y) = pix;
	}
	return res;
}

CImg<unsigned char> Otsu::ImageSegmentationProcessing(CImg<unsigned char> src)
{
	//convert to gray image
	_source = src;
	auto gray = ConvertToGray(src);
	assert(gray.spectrum() == 1);

	//gauss filt
	gray = GaussFilt(gray);
	//calculate propability of intensity histogram and num of pixels of intensity at i.
	double p[256];
	int n[256];
	for (int i = 0; i < 256; i++) {
		n[i] = 0;
		//p[i] = 0;
	}
	int height = gray.height();
	int width = gray.width();
	int totalNum = height * width;

	cimg_forXY(gray, x, y) {
		int i = gray(x, y);
		assert(0 <= i && i <= 255);
		n[i]++;
	}
	for (int i = 0; i < 256; i++) {
		//n[i] = 0;
		p[i] = n[i] * 1.0 / totalNum;
	}
	double maxTheta = 0;//max theta
	int resT = 0;//result threshold
	
	//loop for threshold t from 0 to 255
		//divided into two class of pixels, one less than threshold,another larger than threshold
			//calculate P1,P2,mean1,mean2,mg using
			//P1 = p0+1p1+....pk
			//P2 = pk+1+.....p255
			//m1=1/P1*(0*p0+1*p1+.....+k*pk)
			//m2=1/P2*(k+1*pk+1+.......+255*p255)
			//mg = m1*P1+m2*P2   //calculate out of the loop*
		//calculate thetaG2=(0-mg)^2*p0+ (1-mg)^2*p1+......+(255-mg)^2*p255 ///calculate out of the loop*
		//calculate thetaB2=p1p2(m1-m2)^2
		//choose the most thetaB2
	for (int i = 0; i < 256; i++) {
		int tempT = i;
		double m1 = 0, m2 = 0, p1 = 0, p2 = 0;
		for (int j = 0; j <= tempT; j++) {
			p1 += p[j];
			m1 += j * p[j];
		}
		if (p1 == 0) {
			m1 = 0;
		}
		else {
			m1 = m1 / p1;
		}
		for (int j = tempT+1; j < 256; j++) {
			p2 += p[j];
			m2 += j * p[j];
		}
		if (p2 == 0) {
			m2 = 0;
		}
		else {
			m2 = m2 / p2;
		}

		double theta = p1 * p2*(m1 - m2)*(m1 - m2);
		if (theta > maxTheta) {
			maxTheta = theta;
			resT = tempT;
		}
	}

	double threshold = resT;
	assert(threshold != 0);

	cimg_forXY(gray, x, y) {
		if (gray(x, y) > threshold) {
			gray(x, y) = 255;
		}
		else {
			gray(x, y) = 0;
		}
	}
	_result = gray;
	return gray;
}

CImg<unsigned char> Otsu::ConvertToGray(CImg<unsigned char>& src)
{
	if (src.spectrum() == 1)
		return src;
	assert(src.spectrum() == 3);
	int height = src.height();
	int width = src.width();

	CImg<unsigned char> res(width, height, 1, 1);
	cimg_forXY(src, x, y) {
		double r = src(x, y, 0);
		double g = src(x, y, 1);
		double b = src(x, y, 2);
		res(x, y) = r * 0.2126 + g * 0.7152 + b * 0.0722;
	}
	return res;
}
