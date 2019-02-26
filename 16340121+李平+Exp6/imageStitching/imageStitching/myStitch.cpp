#include "myStitch.h"



myStitch::myStitch()
{
}


myStitch::~myStitch()
{
}

CImg<unsigned char> myStitch::stitch(CImg<unsigned char>& pic1, CImg<unsigned char>& pic2)
{
	CImg<unsigned char> p1(pic1.width(), pic1.height(), 1, 3);
	CImg<unsigned char> p2(pic2.width(), pic2.height(), 1, 3);
	//convert to spherical coordinate
	ImageAssistance::ConvertToSphericalCoordinate(pic1, p1);
	ImageAssistance::ConvertToSphericalCoordinate(pic2, p2);

	//sift feature detect;
	int octaves = 4, levels = 2, o_min = 0;
	auto ms = new mySift(p1, octaves, levels, o_min);
	auto f1=ms->sift_detect();
	ms = new mySift(p2, octaves, levels, o_min);
	auto f2 = ms->sift_detect();

	//feature matching exausted search
	auto mp = ImageAssistance::FeatureMatching(f1, f2);

	//ransac calculate the transform mat
	myRansac mr;
	myRansac::myMat33 transMatFromLeftToRight;
	mr.ransacProcess(mp, transMatFromLeftToRight);
	
	//image stitch
	//CImg<unsigned char> afterTrans(p2.width())
	//mannual stitch test ;
	/*CImg<unsigned char> afterTrans(p2.width() * 3, p2.height() * 3, 1, 3);
	//return afterTrans;
	cimg_forXY(p1, x, y) {
		afterTrans(x, y, 0) = p1(x, y, 0);
		afterTrans(x, y, 1) = p1(x, y, 1);
		afterTrans(x, y, 2) = p1(x, y, 2);
	}
	cimg_forXY(p2, x, y) {
		auto para = transMat.paras;
		int tox, toy;
		mySift::KeyPointDescriptor from;
		from.keypoint.x = x;
		from.keypoint.y = y;
		mr.trans(transMat, from, tox, toy);

		afterTrans(tox, toy, 0) = p2(x, y, 0);
		afterTrans(tox, toy, 1) = p2(x, y, 1);
		afterTrans(tox, toy, 2) = p2(x, y, 2);
	}*/
	//return p1;
	//image stitch automatically;
	//p1 transfer and the result reset its location
	//p1 transfer ,that causes the size of picture changes 
	int x_min = 100000000, y_min = 1000000000, x_max = - 100000000, y_max = -100000000;
	mySift::KeyPointDescriptor from;
	from.keypoint.x = 0;
	from.keypoint.y = 0;
	int tox, toy;
	//mr.tran
	 mr.trans(transMatFromLeftToRight, from, tox, toy);
	 if (tox > x_max) {
		 x_max = tox;
	 }
	 if (tox < x_min) {
		 x_min = tox;
	 }
	 if (toy > y_max) {
		 y_max = toy;
	 }
	 if (toy < y_min) {
		 y_min = toy;
	 }
	 from.keypoint.x = p1.width();
	 from.keypoint.y = 0;
	 mr.trans(transMatFromLeftToRight, from, tox, toy);
	 if (tox > x_max) {
		 x_max = tox;
	 }
	 if (tox < x_min) {
		 x_min = tox;
	 }
	 if (toy > y_max) {
		 y_max = toy;
	 }
	 if (toy < y_min) {
		 y_min = toy;
	 }
	 from.keypoint.x =0;
	 from.keypoint.y = p1.height();
	 mr.trans(transMatFromLeftToRight, from, tox, toy);
	 if (tox > x_max) {
		 x_max = tox;
	 }
	 if (tox < x_min) {
		 x_min = tox;
	 }
	 if (toy > y_max) {
		 y_max = toy;
	 }
	 if (toy < y_min) {
		 y_min = toy;
	 }
	 from.keypoint.x = p1.width();
	 from.keypoint.y = p1.height();
	 mr.trans(transMatFromLeftToRight, from, tox, toy);
	 if (tox > x_max) {
		 x_max = tox;
	 }
	 if (tox < x_min) {
		 x_min = tox;
	 }
	 if (toy > y_max) {
		 y_max = toy;
	 }
	 if (toy < y_min) {
		 y_min = toy;
	 }

	 //compare x_min y_min x_max y_max with 0,p2width and height

	 x_min = x_min >= 0 ? 0 : x_min;
	 y_min = y_min >= 0 ? 0 : y_min;
	 x_max = x_max > p2.width() ? x_max : p2.width();
	 y_max = y_max > p2.height() ? y_max : p2.height();

	 int new_width = x_max - x_min;
	 int new_height = y_max - y_min;

	 CImg<unsigned char> p2after_moved(new_width, new_height, 1, 3);
	 cimg_forXY(p2after_moved, x, y) {
		 p2after_moved(x, y, 0) = 0;
		 p2after_moved(x, y, 1) = 0;
		 p2after_moved(x, y, 2) = 0;
	 }
	 //move p2 to p2after_moved;
	
	 int xoffset = -x_min;
	 int yoffset = -y_min;

	 cimg_forXY(p2, x, y) {
		 p2after_moved(x + xoffset, y + yoffset, 0) = p2(x, y, 0);
		 p2after_moved(x + xoffset, y + yoffset, 1) = p2(x, y, 1);
		 p2after_moved(x + xoffset, y + yoffset, 2) = p2(x, y, 2);
	 }
	 //transfer p1 to p2after_moved
	 //cout << "before transfer p1 to p2" << endl;
	 //cout << "p1:" << p1.width()<<"*" << p1.height() << endl;
	// cout << "p2:" << p2.width() << "*" << p2.height() << endl;
	// cout << "p2aftertransfer:" << new_width << "*" << new_height << endl;
	 cimg_forXY(p1, x, y) {
		 //cout << "scanning loop begin..." << endl;
		 from.keypoint.x = x;
		 from.keypoint.y = y;
		 //cout << "x:" << x << " y:" << y << endl;
		 mr.trans(transMatFromLeftToRight, from, tox, toy);
		// cout << "tox:" << tox << " toy:" << toy << endl;
		/* if (tox >= p2after_moved.width() || toy >= p2after_moved.height()) {
			 //cout << "out of range" << endl;
			 //cout << "tox " << tox << " toy " << toy << endl;
			 continue;
		 }*/
		 tox += xoffset;
		 toy += yoffset;
		 int r = p1(x, y, 0);
		 int g = p1(x, y, 1);
		 int b = p1(x, y, 2);
		 if (r == 0 && g == 0 && b == 0) continue;
		 int r2 = p2after_moved(tox, toy, 0);
		 int g2 = p2after_moved(tox, toy, 1);
		 int b2 = p2after_moved(tox, toy, 2);
		 p2after_moved(tox, toy, 0) = r;
		 p2after_moved(tox, toy, 1) = g;
		 p2after_moved(tox, toy, 2) = b;
	 }
	 //blending
	 //no blending
	 return p2after_moved;
}
