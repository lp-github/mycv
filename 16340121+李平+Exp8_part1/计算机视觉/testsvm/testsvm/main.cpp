#include "commonfile.h"
#include <vector>
#include <iostream>
#include<fstream>
//#include<sstream>
#include"test.h"
#include"fileOpenHelper.h"
//using namespace std;
//command to run g++ -I"C:\opencv\build\include" -L"C:\opencv\build\x86\mingw\lib" main.cpp feature_extraction.cpp -lopencv_core242 -lopencv_ml242 -o main
bool sw = false;
int n_trimg = 60000;  //ѵ������С
int n_testimg = 10000;//���Լ���С
int n_features = 324;//hog��������ά�ȡ�

int main()
{

	auto hog = new HOGDescriptor(Size(28, 28), Size(14, 14), Size(7, 7), Size(7, 7), 9);
	n_features = hog->getDescriptorSize();
	cout << "nfeatures" << n_features << endl;
	test t;

	//t.train();
	t.loadSVM();

	t.processFromDir("C:\\Users\\Administrator\\source\\repos\\testsvm\\testsvm\\testSet\\ImageData");
	while (1);
}
