#include <tchar.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include"CImg.h"
#include<iostream>
#include"mySift.h"
#include"ImageAssistance.h"
#include"myStitch.h"
extern "C" {
#include <vl/generic.h>
#include<vl/sift.h>
#include<vl/stringop.h>
#include<vl/pgm.h>
#include<vl/getopt_long.h>
}
using namespace cimg_library;
using namespace std;

int main(int argc, const char * argv[]) {
	
	
	VL_PRINT("Hello world!\n");
	/*const char * leftpath = "8.bmp";
	const char* rightpath = "9.bmp";
	CImg<unsigned char> leftSrc,rightSrc;
	leftSrc.load_bmp(leftpath);
	rightSrc.load_bmp(rightpath);

	CImg<unsigned char> leftdst(leftSrc.width(), leftSrc.height(), 1, 3);
	CImg<unsigned char> rightdst(rightSrc.width(), rightSrc.height(), 1, 3);

	ImageAssistance::ConvertToSphericalCoordinate(leftSrc, leftdst);
	ImageAssistance::ConvertToSphericalCoordinate(rightSrc, rightdst);

	//ciSrc = CImg<unsigned char>(dst);

	int noctaves = 4, nlevels = 2, o_min = 0;
	
	mySift *ms = new mySift(leftdst, noctaves, nlevels, o_min);
	auto descriptor1 = ms->sift_detect();

	//delete ms;
	ms = new mySift(rightdst, noctaves, nlevels, o_min);
	auto descriptor2 = ms->sift_detect();
	for (int i = 0; i < descriptor1.size(); i++) {
		int x0 = descriptor1[i].keypoint.x;
		int y0 = descriptor1[i].keypoint.y;
		int radius = 3;
		unsigned int color[3] = { 255,0,0 };
		leftdst.draw_circle(x0, y0, radius, color, 3);
	}
	for (int i = 0; i < descriptor2.size(); i++) {
		int x0 = descriptor2[i].keypoint.x;
		int y0 = descriptor2[i].keypoint.y;
		int radius = 3;
		unsigned int color[3] = { 255,0,0 };
		rightdst.draw_circle(x0, y0, radius, color, 3);
	}
	//rightdst.display();
	//ms->src.display();
	//return 0;
	auto matchPair = ImageAssistance::FeatureMatching(descriptor1, descriptor2);

	CImg<unsigned char> comb(leftdst.width()+rightdst.width(), leftdst.height(), 1, 3);
	cimg_forXY(leftdst, x, y) {
		comb(x, y, 0) = leftdst(x, y, 0);
		comb(x, y, 1) = leftdst(x, y, 1);
		comb(x, y, 2) = leftdst(x, y, 2);
	}
	cimg_forXY(rightdst, x, y) {
		comb(x+leftdst.width(),y,0) = rightdst(x, y, 0);
		comb(x + leftdst.width(), y, 1) = rightdst(x, y, 1);
		comb(x + leftdst.width(), y, 2) = rightdst(x, y, 2);
	}
	for (int i = 0; i < matchPair.size(); i++) {
		auto mp = matchPair[i];
		auto xl = mp.pointLeft.keypoint.x;
		auto yl = mp.pointLeft.keypoint.y;
		auto xr = mp.pointRight.keypoint.x;
		auto yr = mp.pointRight.keypoint.y;
		xr = xr + leftdst.width();
		yr = yr;
		int red = rand();
		red = abs(red);
		red = red % 256;
		int green = rand();
		green = abs(green);
		green = green % 256;
		int blue = rand();
		blue = abs(blue);
		blue = blue % 256;
		unsigned int color[3] = { red,green,blue };
		int opacity = 3;
		comb.draw_line(xl, yl, xr, yr, color, opacity);
	}
	comb.display();
	//comb.display();
	
	return 0;*/
	const char* pic1 = "6.bmp";
	const char* pic2 = "7.bmp";
	const char * pic3 = "8.bmp";
	const char* pic4 = "9.bmp";
	CImg<unsigned char> p1, p2, p3, p4;
	p1.load_bmp(pic1);
	p2.load_bmp(pic2);
	p3.load_bmp(pic3);
	p4.load_bmp(pic4);
	
	//show spherical coordinate test
	/*CImg<unsigned char> test(p1.width(), p1.height(), 1, 3);
	ImageAssistance::ConvertToSphericalCoordinate(p1, test);
	test.display();
	return 0;*/
	//show sift feature detect result
	/*mySift* ms = new mySift(p1, 4, 2, 0);
	auto kpd = ms->sift_detect();
	for (int i = 0; i < kpd.size(); i++) {
		auto kpdt = kpd[i];
		auto kp = kpdt.keypoint;
		auto sigma = kp.sigma;
		int x = kp.x;
		int y = kp.y;
		unsigned int color[3] = { 255,0,0 };
		p1.draw_circle(x, y, sigma, color, 1);
		//p1.draw_circle(x,y,)
	}
	p1.display();
	return 0;
	*/

	myStitch mst;
	auto stitPc = mst.stitch(p2, p1);
	stitPc = mst.stitch(p3, stitPc);
	stitPc = mst.stitch(p4, stitPc);
	stitPc.display();

	//stitPc.save_bmp("stitched_final.bmp");

	cout << "end" << endl;
	while (getchar());
	//ciSrc.display();
}
