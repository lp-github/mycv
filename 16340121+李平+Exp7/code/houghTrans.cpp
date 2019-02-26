#include "houghTrans.h"
#include<iostream>
using namespace std;

houghTrans::houghTrans()
{
}


houghTrans::~houghTrans()
{
}
void houghTrans::test()
{
	//ht = 
}
void houghTrans::vote(int x, int y)
{
	//cout << "vote for (" << x << "," << y << ")" << endl;
	static int q = 0;
	q++;
	for (int i = 0; i < num_of_intervals_of_theta; i++) {
		double rho = x * cos(get_index_theta(i)) + y * sin(get_index_theta(i));

		int theta_index = i;
		int rho_index = get_rho_index(rho);
		//cout << "max rho " << max_length_of_rho << endl;
		if (rho_index < 0 || rho_index >= num_of_intervals_of_rho)//it is possible happened, which is caused by a bad theta;
			continue;
		votes[theta_index][rho_index] += 1;
		//cout << "increase at" << theta_index << "," << rho_index << endl;
	}
}
void houghTrans::init()
{
	//memory allocate
	rhos = new double[num_of_intervals_of_rho];
	thetas = new double[num_of_intervals_of_theta];
	votes = new double*[num_of_intervals_of_theta];

	//int  

	for (int i = 0; i < num_of_intervals_of_theta; i ++ ) {
		votes[i] = new double[num_of_intervals_of_rho];
		thetas[i] = max_theta/num_of_intervals_of_theta*i;//[0,2pi)
		for (int j = 0; j < num_of_intervals_of_rho; j++) {
			votes[i][j] = 0;
		}
	}

	for (int i = 0; i < num_of_intervals_of_rho; i++) {
		rhos[i] = max_length_of_rho/num_of_intervals_of_rho * i;
	}
}
void houghTrans::mem_deallocate()
{
	delete []rhos;
	delete[]thetas;
	rhos = nullptr;
	thetas = nullptr;
	for (int i = 0; i < num_of_intervals_of_theta; i++) {
		delete []votes[i];
		votes[i] = nullptr;
	}
	delete[]votes;
	votes = nullptr;
}
int houghTrans::get_theta_index(double theta)
{
	//assert
	//if(theta < max_theta)
	assert(theta < max_theta && theta >= 0);
	return theta / max_theta * num_of_intervals_of_theta;
}
int houghTrans::get_rho_index(double rho)
{
	//assert(rho < max_length_of_rho);
	return rho / max_length_of_rho * num_of_intervals_of_rho;
}
double houghTrans::get_index_rho(int index)
{
	assert(index >= 0 && index < num_of_intervals_of_rho);
	return index * max_length_of_rho / num_of_intervals_of_rho;
}
double houghTrans::get_index_theta(int index)
{
	assert(index >= 0 && index < num_of_intervals_of_theta);
	return index * max_theta / num_of_intervals_of_theta;
}
void houghTrans::cluster(vector<double>& thetav, vector<double>& rhov)
{
	//accurate parameters
	double erho = 20;
	double etheta = 0.3;

	//
	boolean **clustered = new boolean* [num_of_intervals_of_theta];//-------------------------------------------------warning for memory not deallocate
	for (int i = 0; i < num_of_intervals_of_theta; i++) {
		clustered[i] = new boolean[num_of_intervals_of_rho];
		for (int j = 0; j < num_of_intervals_of_rho; j++) {
			clustered[i][j] = false;
		}
	}
	vector<double> res_thetav = vector<double>();
	vector<double> res_rhov = vector<double>();


	//cout << "clustering ";
	for (int i = 0; i < thetav.size(); i++) {
		double theta = thetav.at(i);
		double rho = rhov.at(i);
		int num = 0;
		double theta_sum = 0;
		double rho_sum = 0;
		if (clustered[get_theta_index(theta)][get_rho_index(rho)]) {
			continue;
		}
		
		for (int j = i; j < thetav.size(); j++) {
			if (clustered[get_theta_index(thetav.at(j))][get_rho_index(rhov.at(j))]) {
				continue;
			}
			if ((fabs(theta - thetav.at(j)) < etheta
				||fabs(max_theta-theta)+fabs(thetav.at(j))<etheta
				||fabs(max_theta-thetav.at(j))+fabs(theta)<etheta)
				&& fabs(rho - rhov.at(j)) < erho)
			{//can be clustered
				clustered[get_theta_index(thetav.at(j))][get_rho_index(rhov.at(j))] = true;
				if (fabs(theta - thetav.at(j)) > etheta) {
					continue;
				}
				theta_sum += thetav.at(j);
				rho_sum += rhov.at(j);
				num++;
			}
		}
		res_thetav.push_back(theta_sum / num);
		res_rhov.push_back(rho_sum / num);
	}
	//---------------------------------------------------------------------------------------------------------remove warning of  mem deallocate------------------
	//deallocate mem
	for (int i = 0; i < num_of_intervals_of_theta; i++) {
		delete []clustered[i];
		clustered[i] = nullptr;
	}
	delete[]clustered;
	clustered = nullptr;


	thetav.clear();
	rhov.clear();

	for (int i = 0; i < res_rhov.size(); i++) {
		thetav.push_back(res_thetav.at(i));
		rhov.push_back(res_rhov.at(i));
	}
}
void houghTrans::getPoint()
{
	if (pointed) {
		return;
	}
	pointed = true;
	pointx = vector<int>();
	pointy = vector<int>();
	vector<int> resxv = vector<int>();
	vector<int> resyv = vector<int>();
	//clustering
	{
		boolean * clustered = new boolean[pointx.size()];//-------------warning for memory deallocate-----------
		for (int i = 0; i < pointx.size(); i++) {
			clustered[i] = false;
		}
		vector<int> xv = vector<int>();
		vector<int> yv = vector<int>();
		for (int i = 0; i < pointx.size(); i++) {
			if (clustered[i]) {
				continue;
			}
			int x = xv.at(i);
			int y = yv.at(i);
			int num = 0;
			int sumx = 0;
			int sumy = 0;
			for (int j = 0; j < pointx.size(); j++) {
				if (clustered[j]) {
					continue;
				}
				int xc = xv.at(j);
				int yc = yv.at(j);
				if (abs(x - xc) + abs(y - yc) < 20) {//can be clustered
					clustered[j] = true;
					num++;
					sumx += xc;
					sumy += yc;
				}
				
				
			}
			resxv.push_back(sumx / num);
			resyv.push_back(sumy / num);
		}
		pointx.clear();
		pointy.clear();
		for (int i = 0; i < resxv.size(); i++) {
			pointx.push_back(resxv.at(i));
			pointy.push_back(resyv.at(i));
		}

	}
	//end
	cimg_forXY(source, x, y) {
		int num = 0;
		for (int m = 0; m < thetav.size(); m++) {
			double theta = thetav.at(m);
			double rho = rhov.at(m);
			if (point_on_line(x, y, theta, rho, 1.0)) {
				num++;
			}
		}
		if (num >= 2) {
			//cout << x << "," << y << endl;
			pointx.push_back(x);
			pointy.push_back(y);
			
			//source.draw_circle(x,y,)
		}
	}
	boolean *clustered = new boolean[pointx.size()];
	for (int i = 0; i < pointx.size(); i++) {
		clustered[i] = false;
	}
	vector<int> resx, resy;
	for (int i = 0; i < pointx.size(); i++) {
		if (clustered[i] == true)continue;

		int xi = pointx[i], yi = pointy[i];
		clustered[i] = true;
		double sumx = xi;
		double sumy = yi;
		int num = 1;
		for (int j = 0; j < pointx.size(); j++) {
			if (clustered[j] == true)continue;
			int xj = pointx[j], yj = pointy[j];
			if ((xi - xj)*(xi - xj) + (yi - yj)*(yi - yj) < 10) {
				clustered[j] = true;
				num++;
				sumx += xj;
				sumy += yj;
			}
		}
		int xc = sumx / num;
		int yc = sumy / num;
		resx.push_back(xc);
		resy.push_back(yc);
	}
	pointx = resx;
	pointy = resy;
	//cout << "pointx.size" << pointx.size();
}
CImg<unsigned char> houghTrans::paint_point()
{
	if (!pointed) {
		getPoint();
	}
	for (int i = 0; i < pointx.size(); i++) {
		int x = pointx.at(i);
		int y = pointy.at(i);
		unsigned int color[] = { 0,0,255 };
		source.draw_circle(x, y, 5, color, 1.0);
		cout << x << "," << y << endl;
	}
	
	return source;
}
CImg<unsigned char> houghTrans::draw_edge()
{
	if (!pointed) {
		getPoint();
	}
	//boolean *ispainted = new 
	for (int i = 0; i < thetav.size(); i++) {
		
		double theta = thetav.at(i);
		double rho = rhov.at(i);
		//cout << "for line:" << theta << rho << endl;
		//cout << "size of point" << pointx.size() << endl;
		for (int j = 0; j < pointx.size(); j++) {
			for (int k = 0; k < pointx.size(); k++) {
				int x1 = pointx.at(j);
				int x2 = pointx.at(k);
				int y1 = pointy.at(j);
				int y2 = pointy.at(k);
				if (point_on_line(x1, y1, theta, rho)&&point_on_line(x2,y2,theta,rho)) {
					//cout<<"on same line"<< x1 <<" "<< y1 <<"\t"<< x2 <<" "<< y2 << endl;
					if (abs(x1 - x2) + abs(y1 - y2) > 20) {
						unsigned int color[] = { 255,0,0 };
						source.draw_line(x1, y1, x2, y2, color, 2.0);
						//j = pointx.size();
						//k = j;
						//cout << "2 points:" << x1 << y1 << x2 << y2 << endl;
					}
				}
				
			}
		}
	}
	return source;
}
CImg<unsigned char> houghTrans::gray(CImg<unsigned char> src)
{
	if (src.spectrum() == 1) {
		return src;
	}
	assert(src.spectrum() == 3);
	CImg<unsigned char> res(src.width(), src.height(), 1, 1);
	cimg_forXY(src, x, y) {

		int r = src(x, y, 0);
		int g = src(x, y, 1);
		int b = src(x, y, 2);
		double newValue = (r * 0.2126 + g * 0.7152 + b * 0.0722);

		res(x, y) = (unsigned char)(newValue);
	}
	return res;
}
boolean houghTrans::point_on_line(int x, int y, double theta, double rho, double e)
{
	if (fabs(x*cos(theta) + y * sin(theta) - rho) <= e) {
		return true;
	}
	return false;
}

void houghTrans::drawline(double theta, double rho, unsigned int color[])
{
	//cimg_forXY(source,x)
	cout << rho << "=x*" << cos(theta) << "+y*" << sin(theta) << endl;
	double e =1;
	//cout << "width" << source.width() << endl;
	cimg_forXY(source, x, y) {
		if (abs(x*cos(theta) + y * sin(theta) - rho) <=e) {
			source(x, y, 0) = color[0];
			source(x, y, 1) = color[1];
			source(x, y, 2) = color[2];
			//cout << "paint the point" << x<<"," << y << endl;
		}
	}
}
/*
from edge image to
image with lines;
*/
CImg<unsigned char> houghTrans::hough_trans(CImg<unsigned char> source_,int threshold)
{
	//decide the number of intervals of of rho £º¦Ñ = xcos¦È+ ysin¦È£».
	this->source = source_;
	max_length_of_rho = sqrt(source.width()*source.width() + source.height() * source.height());
	
	init();
	//gray the image and ¶þÖµ»¯
	
	//cimg_forXY(source_)
	//vote
	assert(source.spectrum() == 1);
	cimg_forXY(source, x, y) {
		assert(source(x, y) == 0 || source(x, y) == 255);
	}
	cimg_forXY(source, x, y) {
		if (source(x, y) == 255) {
			if (x == 0 || y == 0 || x == source.width() - 1 || y == source.height() - 1) {
				continue;
			}
			vote(x, y);
		}
		else {
			//do nothing
		}
	}
	
	//find max votes
	int max_votes_num = 0;
	int posx = 0;
	int posy = 0;
	
	for (int j = 0; j < num_of_intervals_of_rho; j++) {
		for (int i = 0; i < num_of_intervals_of_theta; i++) {
			if (votes[i][j] > max_votes_num) {
				max_votes_num = votes[i][j];
				posx = i;
				posy = j;
			}
		}
	}

	cout << "max vote num :" << max_votes_num << endl;
	
	CImg<unsigned char> ti = CImg<unsigned char>(source.width(), source.height(), 1, 3);
	cimg_forXY(source, x, y) {
		ti(x, y, 0) = source(x, y);
		ti(x, y, 1) = source(x, y);
		ti(x, y, 2) = source(x, y);
	}
	source = ti;
	
	CImg<unsigned char> paras = CImg<unsigned char>(500, 800, 1, 1);
	cimg_forXY(paras, x, y) {
		paras(x, y) = votes[x][y] /10;
	}
	//paras.display();
	
	//drawline(source,x,y);
	
	//get votation larger than the threshold
	//int threshold = 0.45* (source.width()>source.height()?source.height():source.width());
	cout << "threshold is :" << threshold<<endl;
	thetav = vector<double>();
	rhov = vector<double>();
	//filter the line above the threshold
	for (int i = 0; i < num_of_intervals_of_theta; i++) {
		for (int j = 0; j < num_of_intervals_of_rho; j++) {
			if (votes[i][j] > threshold) {
				//cout << "vote:" << votes[i][j]<<endl;
				thetav.push_back(get_index_theta(i));
				rhov.push_back(get_index_rho(j));
			}
		}
	}
	//clustering
	cluster(thetav, rhov);
	//drawline

	for (int i = 0; i < thetav.size(); i++) {
		double theta = thetav.at(i);
		double rho = rhov.at(i);
		unsigned int color[] = { 0,0,255 };
		drawline(theta, rho, color);
	}
	//paint_point();
	//draw_edge();
	mem_deallocate();
	return source;
}
/*
theta:5.60879 rho: 0
theta:2.4672 rho: 0
theta:1.60491 rho: 46.1978
theta:1.57708 rho: 676.285
theta:0.0581195 rho: 55.0203
theta:0.00418879 rho: 481.228
*/