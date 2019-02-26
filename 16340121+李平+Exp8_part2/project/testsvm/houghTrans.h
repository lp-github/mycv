#pragma once
/*
draw line from a given double value image ,
so first we need a edge detected image;
*/
#include <cmath>
#include"CImg.h"
//#include"canny.h"
#include<assert.h>
#include<vector>
using namespace cimg_library;
using namespace std;
#define PI 3.141592653589793
class houghTrans
{
public:
	houghTrans();
	~houghTrans();
	CImg<unsigned char> hough_trans(CImg<unsigned char> source_,int threshold = 200);
	static void test();
	CImg<unsigned char> paint_point();
	CImg<unsigned char> draw_edge();
	CImg<unsigned char> gray(CImg<unsigned char>src);
	vector<int> pointx;
	vector<int> pointy;
private:
	CImg<unsigned char> source;
	int num_of_intervals_of_theta = 500;
	int num_of_intervals_of_rho = 800;
	double max_length_of_rho = -1.0;//decided by the image width and length;
	double max_theta = 2 * PI;
	
	vector<double> rhov;
	vector<double> thetav;

	

	double *rhos;
	double *thetas;
	double **votes;

	void vote(int x, int y);
	void init();
	void mem_deallocate();
	int get_theta_index(double theta);
	int get_rho_index(double rho);
	double get_index_rho(int index);
	double get_index_theta(int index);
	void drawline(double theta, double rho, unsigned int color[]);
	void cluster(vector<double>&thetav, vector<double>& rhov);
	
	boolean point_on_line(int x, int y, double theta, double rho, double e = 1.0);
	void getPoint();
	boolean pointed = false;
};


