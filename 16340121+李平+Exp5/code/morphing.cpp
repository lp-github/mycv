#include "morphing.h"



morphing::morphing()
{
}


morphing::~morphing()
{
}

void morphing::setSource(CImg<unsigned char> src)
{
	this->source = src;
}

void morphing::setTarget(CImg<unsigned char> tar)
{
	this->target = tar;
	target.resize(source.width(), source.height());
}

vector<CImg<unsigned char>> morphing::morph(int frames)
{
	
	vector<CImg<unsigned char>> resImgVec;
	//resImgVec.push_back(CImg<unsigned char>(source));
	//preprocess
		
	//resize
	

	//


	//processing
	//detect feature;
	//triangle depart
	initSrcTriangle();
	//interpolation for all triangles
	double steps = 1.0 / (frames+1);
	for (double i = 0; i <= 1.0; i+= steps) {
		//CImg<unsigned char> mid(source.width(),source.height(),1,3);
		CImg<unsigned char> mid(source);
		myPoint midPoint[39];
		for (int p = 0; p < 39; p++) {
			midPoint[p].x = (1 - i)*srcPoint[p].x + i * tarPoint[p].x;
			midPoint[p].y = (1 - i)*srcPoint[p].y + i * tarPoint[p].y;
			/*unsigned int color[3] = { 255,0,0 };
			mid.draw_circle(midPoint[p].x, midPoint[p].y, 3, color);*/
		}

		for (int j = 0; j < srcTriangle.size(); j++) {
			//break;
			auto srcTran = srcTriangle[j];
			int index1 = getPointIndex(myPoint{(int)srcTran.p1.x,(int) srcTran.p1.y});
			int index2 = getPointIndex(myPoint{ (int)srcTran.p2.x,(int)srcTran.p2.y });
			int index3 = getPointIndex(myPoint{ (int)srcTran.p3.x,(int)srcTran.p3.y });
			auto srcpoint1 = srcTran.p1;
			auto srcpoint2 = srcTran.p2;
			auto srcpoint3 = srcTran.p3;
			auto midpoint1 = midPoint[index1];
			auto midpoint2 = midPoint[index2];
			auto midpoint3 = midPoint[index3];
			auto tarpoint1 = tarPoint[index1];
			auto tarpoint2 = tarPoint[index2];
			auto tarpoint3 = tarPoint[index3];

			double a = 1.0 / maxEdge(midpoint1, midpoint2, midpoint3);
			for (double p = 0; p < 1; p += a) {
				for (double q = 0; q < 1 - p; q += a) {
					int mid_x = p * midpoint1.x + q * midpoint2.x + (1 - p - q)*midpoint3.x;
					int mid_y = p * midpoint1.y + q * midpoint2.y + (1 - p - q)*midpoint3.y;
					double mu, v;
					myPoint midpoint{ mid_x,mid_y };
					calculate_mu_v(midpoint1, midpoint2, midpoint3, midpoint, mu, v);
					int src_x = (1 - mu - v)*srcpoint1.x + v * srcpoint2.x + mu * srcpoint3.x;
					int src_y = (1 - mu - v)*srcpoint1.y + v * srcpoint2.y + mu * srcpoint3.y;

					int tar_x = (1 - mu - v)*tarpoint1.x + v * tarpoint2.x + mu * tarpoint3.x;
					int tar_y = (1 - mu - v)*tarpoint1.y + v * tarpoint2.y + mu * tarpoint3.y;

					mid(mid_x, mid_y, 0) = (1 - i)*source(src_x, src_y, 0) + i * target(tar_x, tar_y, 0);
					mid(mid_x, mid_y, 1) = (1 - i)*source(src_x, src_y, 1) + i * target(tar_x, tar_y, 1);
					mid(mid_x, mid_y, 2) = (1 - i)*source(src_x, src_y, 2) + i * target(tar_x, tar_y, 2);
				}
			}
			//auto midTran = Triangle(midPoint[getPointIndex(srcTran.p1)])

		}
		resImgVec.push_back(mid);
	}
	//resImgVec.push_back(CImg<unsigned char>(target));
	return resImgVec;
}

void morphing::display()
{
}

void morphing::show_source()
{
	for (int i = 0; i < 39; i++) {
		int x = srcPoint[i].x;
		int y = srcPoint[i].y;
		const unsigned int color[] = { 255,0,0 };
		source.draw_circle(x, y, 5, color);
	}
	source.save_bmp("1_draw.bmp");
	source.display();
	pause();
}

void morphing::show_target()
{
	for (int i = 0; i < 39; i++) {
		int x = tarPoint[i].x;
		int y = tarPoint[i].y;
		const unsigned int color[] = { 255,0,0 };
		target.draw_circle(x, y, 5, color);
	}
	target.display();
}

void morphing::pause()
{
	//struct myPoint m;
	

	while (getchar() != 'q');
	
}

void morphing::initSrcTriangle()
{
	Delaunay delaunay;
	vector<Vector2> vertices;
	for (int i = 0; i < 39; i++) {
		int x = srcPoint[i].x;
		int y = srcPoint[i].y;
		auto vert = Vector2(x, y);
		vertices.push_back(vert);
	}

	srcTriangle = delaunay.triangulate(vertices);
	/*
	for (int i = 0; i < srcTriangle.size(); i++) {
		auto trian = srcTriangle[i];
		auto a = trian.p1;
		auto b = trian.p2;
		auto c = trian.p3;
		unsigned int color[] = { 255,0,0 };

		source.draw_line(a.x, a.y, b.x, b.y, color, 5);
		source.draw_line(a.x, a.y, c.x, c.y, color, 5);
		source.draw_line(b.x, b.y, c.x, c.y, color, 5);
	}*/
	//source.display();
	//here waiting to be finished!:
}

morphing::myTriangle morphing::createTriangle(myPoint A, myPoint B, myPoint C)
{
	myTriangle mt = myTriangle();
	mt.A = A;
	mt.B = B;
	mt.C = C;
	return mt;
}

int morphing::getPointIndex(const myPoint & p)
{
	for (int i = 0; i < 39; i++) {
		auto traPoint = srcPoint[i];
		if (traPoint.x == p.x && traPoint.y == p.y) {
			return i;
		}
	}
	return -1;
}

double morphing::maxEdge(const myPoint & p1, const myPoint & p2, const myPoint & p3)
{
	double e1 = (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y);
	double e2 = (p1.x - p3.x)*(p1.x - p3.x) + (p1.y - p3.y)*(p1.y - p3.y);
	double e3 = (p2.x - p3.x)*(p2.x - p3.x) + (p2.y - p3.y)*(p2.y - p3.y);
	if (e1 < e2) {
		e1 = e2;
	}
	if (e1 < e3) {
		e1 = e3;
	}
	return sqrt(e1);
}

void morphing::calculate_mu_v(const myPoint & p1, const myPoint & p2, const myPoint & p3, const myPoint & mid, double & mu, double & v)
{
	//mu(xc-xa)+v(xb-xa) = xmid
	//mu(yc-ya) + v(yb-ya) = ymid;
	int xc = p3.x;
	int xa = p1.x;
	int xb = p2.x;
	int ya = p1.y;
	int yb = p2.y;
	int yc = p3.y;
	int xmid = mid.x - xa;
	int ymid = mid.y - ya;
	if (yc == ya) {
		mu = 1;
		v = 0;
	}
	else {
		v = (xmid*yc - xmid * ya - ymid * xc + ymid * xa)*1.0 / (xb*yc - xa * yc - xb * ya - yb * xc + yb * xa + ya * xc);
		mu = (ymid - v * (yb - ya))*1.0 / (yc - ya);
	}
	//double k1 = 
	//----------------------------------------exception of divided by zero-----------
}
