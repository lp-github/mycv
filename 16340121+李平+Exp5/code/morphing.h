#pragma once
#include "CImg.h"
#include<vector>
#include<iostream>
#include"delaunay.h"
using namespace std;
using namespace cimg_library;
class morphing
{
public:
	morphing();
	~morphing();
	struct myPoint {
		int x;
		int y;
	};
	struct myTriangle {
		myPoint A;
		myPoint B;
		myPoint C;
	};

	void setSource(CImg<unsigned char> src);
	void setTarget(CImg<unsigned char> tar);

	vector<CImg<unsigned char>> morph(int frames = 11);
	void display();
	void show_source();
	void show_target();
	void pause();
	void initSrcTriangle();
	myTriangle createTriangle(myPoint A, myPoint B, myPoint C);
	int getPointIndex(const myPoint& p);
	double maxEdge(const myPoint & p1, const myPoint & p2, const myPoint & p3);
	void calculate_mu_v(const myPoint& p1, const myPoint & p2, const myPoint& p3,const myPoint &mid, double &mu, double &v);
private:

	CImg<unsigned char> source;
	CImg<unsigned char> target;
	vector<Triangle> srcTriangle;
	const  myPoint srcPoint[39]{
		{ 71, 260 },
	{ 106, 142 },
	{ 250, 42 },
	{ 384, 142 },
	{ 415, 260 },
	{ 260, 153 },
	{ 117, 219 },
	{ 379, 219 },
	{ 172, 283 },
	{ 321, 282 },
	{ 137, 308 },
	{ 222, 302 },
	{ 275, 301 },
	{ 354, 296 },
	{ 383, 315 },
	{ 106, 291 },
	{ 156, 322 },
	{ 189, 312 },
	{ 213, 329 },
	{ 288, 329 },
	{ 309, 312 },
	{ 343, 325 },
	{ 250, 375 },
	{ 230, 399 },
	{ 269, 398 },
	{ 126, 431 },
	{ 187, 429 },
	{ 251, 419 },
	{ 307, 429 },
	{ 368, 424 },
	{ 227, 462 },
	{ 267, 463 },
	{ 180, 486 },
	{ 249, 518 },
	{ 309, 485 },
	{ 0,0 },
	{ 0,699 },
	{ 489,0 },
	{ 489,699 },
	};
	struct myPoint tarPoint[39]{
		{75,271},
	{ 86,106 },
	{ 258,24 },
	{ 402,124 },
	{ 411,262 },
	{ 263,131 },
	{ 113,203 },
	{ 367,206 },
	{ 182,277 },
	{ 322,267 },
	{ 124,306 },
	{ 223,306 },
	{ 277,302 },
	{ 366,306 },
	{ 378,386 },
	{ 104,288 },
	{ 140,323 },
	{ 181,297 },
	{ 213,319 },
	{ 277,322 },
	{ 310,295 },
	{ 348,322 },
	{ 250,402 },
	{ 228,428 },
	{ 263,429 },
	{ 122,465 },
	{ 195,475 },
	{ 249,463 },
	{ 303,478 },
	{ 371,465 },
	{ 230,509 },
	{ 270,505 },
	{ 153,531 },
	{ 247,599 },
	{ 340,527 },
	{0,0},
	{0,699},
	{489,0},
	{489,699},
	};
};

