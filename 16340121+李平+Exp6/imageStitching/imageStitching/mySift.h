#pragma once
#include"CImg.h"
#include<vector>
extern "C" {
#include<vl/sift.h>
}
using namespace cimg_library;
using namespace std;
class mySift
{
public:
	struct KeyPointDescriptor {
		int angleOffset;
		vl_sift_pix orientation;
		VlSiftKeypoint keypoint;
		vl_sift_pix Descriptor[128];
	};
	
	mySift(CImg<unsigned char>src, int octaves,int levels,int o_min);
	~mySift();
	vector<KeyPointDescriptor> sift_detect();

	CImg<unsigned char> src;
private:
	;
	int octaves;
	int levels;
	int o_min;
	

};

