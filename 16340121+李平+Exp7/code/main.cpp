#include"CImg.h"
#include"Otsu.h"
#include"A4Refactor.h"
using namespace cimg_library;
void main() {
	
	CImg<unsigned char> src;
	src.load_bmp("data\\1.bmp");
	A4Refactor a4 = A4Refactor();
	a4.RefactProcessing(src);
}