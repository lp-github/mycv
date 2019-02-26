#include<vector>
#include"CImg.h"
#include <opencv2/ml/ml.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include<assert.h>
#include"Otsu.h"
//
using namespace std;
using namespace cv;
using namespace ml;
using namespace cimg_library;
extern int n_trimg;     
extern int n_testimg;
extern int n_features;
extern bool sw;
//Function Defination
std::vector< std::vector<float> > read_images_extract_features(char filename[],int n_images);
std::vector<float> getFeatureFromSingleImage(CImg<unsigned char> img);
std::vector<int> read_lables(char filename[],int n_images);
CImg<unsigned char> preprocessingImage(CImg<unsigned char>& src);
CImg<unsigned char> convertFromMatToCImg(Mat mat);
void erosion(CImg<unsigned char>& src);
void expansion(CImg<unsigned char>& src);
void throwAwaySmallArea(CImg<unsigned char> & src);
void setFlag(const CImg<unsigned char> & src, int** & flagOnPos, int x,int y,int flagNumber);
void splitSegmentation(CImg<unsigned char> & gray, int rowdiv, int coldiv);
void removeShadow(CImg<unsigned char> & gray);
