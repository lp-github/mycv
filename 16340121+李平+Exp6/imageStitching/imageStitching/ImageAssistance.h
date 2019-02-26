#pragma once
#include"CImg.h"
#include<assert.h>
#include "mySift.h"
#include<iostream>
using namespace cimg_library;
using namespace std;
namespace ImageAssistance {
	inline void ConvertToSphericalCoordinate(const CImg<unsigned char> &src, CImg<unsigned char> &dst)//参数一为原图像的数据区首指针，参数二为投影后图像的数据区首指针，参数三为图像的宽，参数四为图像的高
	{
		assert(src.width() == dst.width() && src.height() == dst.height());

		cimg_forXY(dst, x, y) {
			dst(x, y, 0) = 0;
			dst(x, y, 1) = 0;
			dst(x, y, 2) = 0;
		}
		int width = src.width();
		int height = src.height();
		double theta = 15*1.0/180*3.1415926;
		double R = width * 1.0 / 2/tan(theta);//loss of precision
		double x, y;
		int drcpoint;
		for (int hnum = 0; hnum < height; hnum++) {
			for (int wnum = 0; wnum < width; wnum++) {

				double k = R / sqrt(R*R + (wnum - width / 2)*(wnum - width / 2));
				x = (wnum - width / 2)*1.0 / k + width * 1.0 / 2;
				y = (hnum - height / 2) *1.0 / k + height * 1.0 / 2;

				if (0 <= x && x < width && 0 <= y && y < height) {
					//double interplate
					/*a r1 b
					    p'
					  c r2 d
					*/
					int x0 = x, x1 = x0 + 1;
					int y0 = y, y1 = y0 + 1;
					x1 = x1 == width ? x1 - 1 : x1;
					y1 = y1 == height ? y1 - 1 : y1;

					int a[3] = { src(x0,y0,0),src(x0,y0,1),src(x0,y0,2) };
					int b[3] = { src(x1,y0,0),src(x1,y0,1),src(x1,y0,2) };
					int c[3] = { src(x0,y1,0),src(x0,y1,1),src(x0,y1,2) };
					int d[3] = { src(x1,y1,0),src(x1,y1,1),src(x1,y1,2) };

					x1 = x0 + 1;
					y1 = y0 + 1;

					double r1[3] = { a[0] * (x1 - x) + b[0] * (x - x0),a[1] * (x1 - x) + b[1] * (x - x0),a[2] * (x1 - x) + b[2] * (x - x0) };
					double r2[3] = { c[0] * (x1 - x) + d[0] * (x - x0),c[1] * (x1 - x) + d[1] * (x - x0),c[2] * (x1 - x) + d[2] * (x - x0) };
					double p[3] = { r1[0] * (y1 - y) + r2[0] * (y - y0), r1[1] * (y1 - y) + r2[1] * (y - y0),r1[2] * (y1 - y) + r2[2] * (y - y0) };
					dst(wnum, hnum, 0) = p[0];
					dst(wnum, hnum, 1) = p[1];
					dst(wnum, hnum, 2) = p[2];
				}
			}
		}
	}
	/*bool equal(const mySift::KeyPointDescriptor&a, const mySift::KeyPointDescriptor&b) {
		for (int i = 0; i < 128; i++) {
			if (a.Descriptor[i] == b.Descriptor[i]) {
				continue;
			}
			return false;
		}
		return true;
	}*/
	struct MatchPair {
		mySift::KeyPointDescriptor pointLeft;
		mySift::KeyPointDescriptor pointRight;
		double distance;
	};

	inline double CalculateDistance(const mySift::KeyPointDescriptor& pointLeft, const mySift::KeyPointDescriptor& pointRight) {
		auto ptr128_left = pointLeft.Descriptor;
		auto ptr128_right = pointRight.Descriptor;
		double sum = 0;
		for (int i = 0; i < 128; i++) {
			double d = ptr128_left[i] - ptr128_right[i];
			d = d * d;
			sum += d;
		}
		return pow(sum, 0.5);
	}
	inline void FeatureMatching(mySift::KeyPointDescriptor&feat, vector<mySift::KeyPointDescriptor>&features,
													mySift::KeyPointDescriptor &first,mySift::KeyPointDescriptor & second) {
		double minDst = 10000000.0f;
		double secDst = 99999999.0f;
		mySift::KeyPointDescriptor *  fir =NULL;
		mySift::KeyPointDescriptor* sec = NULL;
		for (int i = 0; i < features.size(); i++) {
			double dst = CalculateDistance(feat, features[i]);
			if (dst < minDst) {
				sec = fir;
				fir = &features[i];

				secDst = minDst;
				minDst = dst;
			}
			else if (dst < secDst) {
				sec = &features[i];
				secDst = dst;
			}
		}
		first = *fir;
		second = *sec;
	}
	inline vector<MatchPair> FeatureMatching( vector<mySift::KeyPointDescriptor>&left,  vector<mySift::KeyPointDescriptor>&right) {
		vector<MatchPair> ret;
		for (int i = 0; i < left.size(); i++) {
			mySift::KeyPointDescriptor leftkp = left[i];
			mySift::KeyPointDescriptor first,second;
			
			FeatureMatching(leftkp, right,first,second);
			auto dis1 = CalculateDistance(leftkp, first);
			auto dis2 = CalculateDistance(leftkp, second);

			if (dis1/dis2 <= 0.4) {
				//match
				MatchPair mp;
				mp.distance = CalculateDistance(leftkp, first);
				mp.pointLeft = leftkp;
				mp.pointRight = first;

				//cout << "left:" << leftkp.keypoint.x << ", " << leftkp.keypoint.y << " right:" << first.keypoint.x << ", " << first.keypoint.y << "\n";
				//cout << "distance:" << mp.distance<<endl;
				ret.push_back(mp);
			}
		}
		return ret;
	}


}
