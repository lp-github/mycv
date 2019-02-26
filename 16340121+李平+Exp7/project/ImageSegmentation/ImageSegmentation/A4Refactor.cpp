#include "A4Refactor.h"



A4Refactor::A4Refactor()
{
}


A4Refactor::~A4Refactor()
{
}

void A4Refactor::resort(vector<int>& posx, vector<int>& posy)
{
	assert(posx.size() == 4 && posy.size() == 4);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (i == j)continue;
			for (int m = 0; m < 4; m++) {
				if (m == i || m == j)continue;
				for (int n = 0; n < 4; n++) {
					if (n == i || n == j || n == m)continue;
					int xa = posx[i], xb = posx[j], xc = posx[m], xd = posx[n];
					int ya = posy[i], yb = posy[j], yc = posy[m], yd = posy[n];
					//CImg<unsigned char>
				}
			}
		}
	}
}

CImg<unsigned char> A4Refactor::RefactProcessing(CImg<unsigned char>& source)
{
	//segmentation
	auto o = new Otsu();
	auto afterSeg = o->ImageSegmentationProcessing(source);
	//afterSeg.display();
	CImg<unsigned char> temp(afterSeg.width(), afterSeg.height(), 1, 1);
	//fushi pengzhang

	cimg_forXY(afterSeg, x, y) {
		int depressed = 0;
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j++) {
				int xa = x + i;
				int ya = y + j;
				if (xa < 0 || xa >= afterSeg.width() || ya < 0 || ya >= afterSeg.height()) {
					depressed++;
					continue;
				}
				if (afterSeg(xa, ya) == 255) {
					depressed++;
				}
			}
		}
		temp(x, y) = depressed == 9 ? 0 : afterSeg(x,y);
	}
	//temp.display();
	//huoghtrans
	auto ht = houghTrans();
	ht.hough_trans(temp,100);
	auto edge = ht.draw_edge();
	//edge.display();

	auto point = ht.paint_point();
	//point.display();
	//refact
	printf("vector size:%d\n", ht.pointx.size());
	assert(ht.pointx.size() == 4);
	assert(ht.pointy.size() == 4);
	for (int i = 0; i < 4; i++) {
		printf("%d,%d\n", ht.pointx[i], ht.pointy[i]);
	}
	CImg<unsigned char> result(628,888,1,3);

	int xa, xb, xc, xd, ya, yb, yc, yd;
	cout << "please sort the point above as shown below:\n"<<endl;
	cout << "\tA\tB\n\tC\tD\n" << endl;
	cout << "please enter A's x and y after sort:\n";
	cin >> xa >> ya;
	cout << "please enter B's x and y after sort:\n";
	cin >> xb >> yb;
	cout << "please enter C's x and y after sort:\n";
	cin >> xc >> yc;
	cout << "please enter D's x and y after sort:\n";
	cin >> xd >> yd;
	cout << "confirm your sort:" << endl;
	cout << xa << "," << ya << "\t" << xb << "," << yb << endl;
	cout << xc << "," << yc << "\t" << xd << "," << yd << endl;
	//calculate trans matrix
	double a, b, c, d, e, f, g, h;
	int x1 = 0, x3 = 0, y1 = 0, y2 = 0, x2 = result.width()-1, x4 = result.width()-1, y3 = result.height()-1, y4 = result.height()-1;

	CImg<float> A(4, 4, 1, 1);
	A(0, 0) = x1;
	A(0, 1) = x2;
	A(0, 2) = x3;
	A(0, 3) = x4;

	A(1, 0) = y1;
	A(1, 1) = y2;
	A(1, 2) = y3;
	A(1, 3) = y4;

	A(2, 0) = x1*y1;
	A(2, 1) = x2*y2;
	A(2, 2) = x3*y3;
	A(2, 3) = x4*y4;

	A(3, 0) = 1;
	A(3, 1) = 1;
	A(3, 2) = 1;
	A(3, 3) = 1;

	CImg<float> bv(1, 4,1,1),cv(1,4,1,1);
	bv(0, 0) = xa;
	bv(0, 1) = xb;
	bv(0, 2) = xc;
	bv(0, 3) = xd;

	cv(0, 0) = ya;
	cv(0, 1) = yb;
	cv(0, 2) = yc;
	cv(0, 3) = yd;

	auto xParas = bv.get_solve(A);
	auto yParas = cv.get_solve(A);
	a = xParas(0, 0);
	b = xParas(0, 1);
	c = xParas(0, 2);
	d = xParas(0, 3);

	e = yParas(0, 0);
	f = yParas(0, 1);
	g = yParas(0, 2);
	h = yParas(0, 3);

	//trans from source to result
	cimg_forXY(result, x, y) {
		double xsrc = a * x + b * y + c * x*y + d;
		double ysrc = e * x + f * y + g * x*y + h;
		//pa pb
		//pc pd
		int thisx = xsrc;
		int thisy = ysrc;
		int nextx = xsrc + 1;
		int nexty = ysrc + 1;
		if (nextx == source.width()) {
			nextx = xsrc;
		}
		if (nexty == source.height()) {
			nexty = ysrc;
		}
		int xpa = thisx;
		int xpb = thisx + 1;
		int xpc = thisx;
		int xpd = thisx + 1;
		int ypa = thisy;
		int ypb = thisy;
		int ypc = thisy + 1;
		int ypd = thisy + 1;
		
		double colorA[3] = { source(thisx,thisy,0),source(thisx,thisy,1),source(thisx,thisy,2) };
		double colorB[3] = { source(nextx,thisy,0),source(nextx,thisy,1),source(nextx,thisy,2) };
		double colorC[3] = { source(thisx,nexty,0),source(thisx,nexty,1),source(thisx,nexty,2) };
		double colorD[3] = { source(nextx,nexty,0),source(nextx,nexty,1),source(nextx,nexty,2) };
		double colorUP[3] = { colorA[0] * (xpb - xsrc) + colorB[0] * (xsrc - xpa),colorA[1] * (xpb - xsrc) + colorB[1] * (xsrc - xpa),colorA[2] * (xpb - xsrc) + colorB[2] * (xsrc - xpa) };
		double colorDOWN[3] = { colorC[0] * (xpd - xsrc) + colorD[0] * (xsrc - xpc), colorC[1] * (xpd - xsrc) + colorD[1] * (xsrc - xpc), colorC[2] * (xpd - xsrc) + colorD[2] * (xsrc - xpc) };
		double color[3] = { colorUP[0] * (ypd - ysrc) + colorDOWN[0] * (ysrc - ypb),colorUP[1] * (ypd - ysrc) + colorDOWN[1] * (ysrc - ypb) ,colorUP[2] * (ypd - ysrc) + colorDOWN[2] * (ysrc - ypb) };
		result(x, y, 0) = color[0];
		result(x, y, 1) = color[1];
		result(x, y, 2) = color[2];

		/*result(x, y, 0) = source(xsrc,ysrc,0);
		result(x, y, 1) = source(xsrc, ysrc, 1);
		result(x, y, 2) = source(xsrc, ysrc, 2);*/
	}
	source.display();
	result.display();
	result.save_bmp("result.bmp");
	cout << a << "\t" << b << "\t" << c << "\t" << d<<endl;
	while(getchar()!='q');
	//result.resize()
	return source;
}
