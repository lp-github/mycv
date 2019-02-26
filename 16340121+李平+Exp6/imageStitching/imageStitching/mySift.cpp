#include "mySift.h"



mySift::mySift(CImg<unsigned char>ciSrc, int octaves, int levels, int o_min_)
{
	src = ciSrc;
	this->octaves = octaves;
	this->levels = levels;
	this->o_min = o_min_;
}


mySift::~mySift()
{
}

vector<mySift::KeyPointDescriptor> mySift::sift_detect()
{
	vector<mySift::KeyPointDescriptor> res;
	CImg<unsigned char> ciSrc = this->src;
	
	int noctaves = this->octaves, nlevels = this->levels, o_min = this->o_min;
	vl_sift_pix *ImageData = new vl_sift_pix[ciSrc.height()*ciSrc.width()];

	for (int i = 0; i < ciSrc.height(); i++) {
		for (int j = 0; j < ciSrc.width(); j++) {
			ImageData[i*ciSrc.width() + j] = ciSrc(j,i,0)*0.2126+ciSrc(j,i,1)*0.7152+ciSrc(j,i,2)*0.0722;
		}
	}

	VlSiftFilt *SiftFilt = NULL;
	SiftFilt = vl_sift_new(ciSrc.width(), ciSrc.height(), noctaves, nlevels, o_min);
	int KeyPoint = 0;
	int idx = 0;
	if (vl_sift_process_first_octave(SiftFilt, ImageData) != VL_ERR_EOF) {
		while (true) {
			vl_sift_detect(SiftFilt);

			KeyPoint += SiftFilt->nkeys;
			VlSiftKeypoint *pKeyPoint = SiftFilt->keys;
			//printf("octave with keypoint number:%d\n", SiftFilt->nkeys);
			CImg<unsigned char> tempImg(ciSrc);
			for (int i = 0; i < SiftFilt->nkeys; i++) {
				VlSiftKeypoint TempKeyPoint = *pKeyPoint;
				pKeyPoint++;
				idx++;
				//draw_circle
				int x0 = TempKeyPoint.x;
				int y0 = TempKeyPoint.y;
				int radius = 3;
				unsigned int color[3] = { 255,0,0 };
				src.draw_circle(x0, y0, radius, color, 3);
				//end
				//printf("%d : %d , %d\n", i, TempKeyPoint.x, TempKeyPoint.y);
				double angles[4];
				int angleCount = vl_sift_calc_keypoint_orientations(SiftFilt, angles, &TempKeyPoint);
				for (int j = 0; j < angleCount; j++) {
					double TemptAngle = angles[j];
					float *Descriptors = new float[128];
					vl_sift_calc_keypoint_descriptor(SiftFilt, Descriptors, &TempKeyPoint, TemptAngle);

					KeyPointDescriptor kpd = KeyPointDescriptor{};
					kpd.angleOffset = j;
					kpd.keypoint = TempKeyPoint;
					kpd.orientation = TemptAngle;
					//printf("%d:%f\n", j, angles[j]);
					int k = 0;
					//double sum = 0;
					while (k < 128) {
						kpd.Descriptor[k] = Descriptors[k];
						//sum += Descriptors[k];
						//printf("%d : %f  ", k, Descriptors[k]);
						k++;
					}
					//printf("\nsum:%f", sum);

					//printf("\n");
					res.push_back(kpd);
					delete[] Descriptors;
					Descriptors = NULL;
				}
			}
			
			if (vl_sift_process_next_octave(SiftFilt) == VL_ERR_EOF) {
				break;
			}
			//break;
			KeyPoint = NULL;
		}
	}

	vl_sift_delete(SiftFilt);
	delete[]ImageData;
	ImageData = NULL;
	return res;
}
