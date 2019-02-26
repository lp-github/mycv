#include "myRansac.h"



myRansac::myRansac()
{
}


myRansac::~myRansac()
{
}

void myRansac::trans(myMat33 & transMat, mySift::KeyPointDescriptor & from, int & tox, int & toy)
{
	
	int u = from.keypoint.x;
	int v = from.keypoint.y;

	auto paras = transMat.paras;

	tox = u * paras[0] + v * paras[1] + u * v*paras[2] + paras[3];
	toy = u * paras[4] + v * paras[5] + u * v*paras[6] + paras[7];
}

void myRansac::ransacProcess(vector<ImageAssistance::MatchPair> & matchPairs,myMat33 & transMat)
{
	/*
	a00 a01 a02
	a10 a11 a12
	a20 a21 a22=1
	*/
	double p = 0.5;
	double n = 4;
	double P = 0.99;

	int k = log(1 - P) / log(1 - pow(p, n));//circle numbers
	
	srand((unsigned)time(NULL));
	int size = matchPairs.size();

	//myMat33 transMat_;
	vector<ImageAssistance::MatchPair>  inliner_ptr ;
	while (k--) {
		//randomly select four sets of matched pair
		int index1, index2, index3, index4;
		index1 = abs(rand()) % size;
		index2 = abs(rand()) % size;
		index3 = abs(rand()) % size;
		index4 = abs(rand()) % size;
		while (index2 == index1) {
			index2 = abs(rand()) % size;
		}
		while (index3 == index2 || index3 == index1) {
			index3 = abs(rand()) % size;
		}
		while (index4 == index3 || index4 == index2 || index4 == index1) {
			index4 = abs(rand()) % size;
		}

		vector<ImageAssistance::MatchPair> mps;
		mps.push_back(matchPairs[index1]);
		mps.push_back(matchPairs[index2]);
		mps.push_back(matchPairs[index3]);
		mps.push_back(matchPairs[index4]);



		CImg<float> a(4, 4, 1, 1);
		CImg<float> b(1, 4, 1, 1);
		CImg<float> c(4, 4, 1, 1);
		CImg<float> d(1, 4, 1, 1);
		for (int i = 0; i < 4; i++) {
			//auto 
			auto left = mps[i].pointLeft.keypoint;
			auto xi = left.x;
			auto yi = left.y;
			auto right = mps[i].pointRight.keypoint;
			auto xi_ = right.x;
			auto yi_ = right.y;
			a(0, i) = xi;
			a(1, i) = yi;
			a(2, i) = xi * yi;
			a(3, i) = 1;

			b(0, i) = xi_;

			c(0, i) = xi;
			c(1, i) = yi;
			c(2, i) = xi * yi;
			c(3, i) = 1;

			d(0, i) = yi_;
		}
		auto p03 = b.get_solve(a);
		auto p47 = d.get_solve(c);
		for (int i = 0; i < 4; i++) {
			transMat.paras[i] = p03(0, i);
			transMat.paras[i + 4] = p47(0, i);
		}
		
		//count inliners
		
		//debug
		/*cout << "first match from " << mps[0].pointLeft.keypoint.x << "," << mps[0].pointLeft.keypoint.y;
		cout << " to " << mps[0].pointRight.keypoint.x << "," << mps[0].pointRight.keypoint.y << endl;
		cout << "paras:\t" << c1 <<"\t"<< c2 <<"\t"<< c3<<"\t" << c4 <<"\t"<< c5 <<"\t"<< c6 <<"\t"<< c7<<"\t" << c8<<"\t"<<endl;*/
		vector<ImageAssistance::MatchPair> inliners;
		for (int i = 0; i < matchPairs.size(); i++) {
			//count inliners
			auto mp = matchPairs[i];
			auto from = mp.pointLeft;
			auto dstx = mp.pointRight.keypoint.x;
			auto dsty = mp.pointRight.keypoint.y;
			int tox, toy;
			trans(transMat, from, tox, toy);

			double dst = (tox - dstx)*(tox - dstx) + (toy - dsty)*(toy - dsty);
			dst = sqrt(dst);
			//cout << "dist:\t"<<dst << endl;
			if (dst < 4.0) {
				//inliners;
				//cout << "push back" << endl;
				inliners.push_back(mp);
			}
		}
		//cout << "inliners number:" << inliners.size()<<endl;
		//cout << "inliner ptr equal to NULL" <<( inliner_ptr == NULL) << endl;
		//compare with the inliner_ptr;
		if (inliner_ptr.size() < inliners.size()) {
			
			inliner_ptr = inliners;
			//cout << "\t\t\t\t\twe got a better inliner with number:" << inliner_ptr.size() << endl;
		}
		//while end
	}

	//calculate the real transform paras;

	CImg<float> a(4, inliner_ptr.size(), 1, 1);
	CImg<float> b(1, inliner_ptr.size(), 1, 1);
	CImg<float> c(4, inliner_ptr.size(), 1, 1 );
	CImg<float> d(1, inliner_ptr.size(), 1, 1);
	//cout << "adjust if inliner ptr equal to null:" << (inliner_ptr == NULL)<<endl;
	cout << "inlinerptr size:" << inliner_ptr.size() << endl;
	for (int i = 0; i < inliner_ptr.size(); i++) {
		//auto 
		auto mps = inliner_ptr;

		auto left = mps[i].pointLeft.keypoint;
		auto xi = left.x;
		auto yi = left.y;
		auto right = mps[i].pointRight.keypoint;
		auto xi_ = right.x;
		auto yi_ = right.y;
		a(0, i) = xi;
		a(1, i) = yi;
		a(2, i) = xi * yi;
		a(3, i) = 1;

		b(0, i) = xi_;

		c(0, i) = xi;
		c(1, i) = yi;
		c(2, i) = xi * yi;
		c(3, i) = 1;

		d(0, i) = yi_;
	}
	//return;
	//cout << "a: row" << a.height() << " col " << a.width() << endl;
	//cout << "b: row" << b.height() << " col " << b.width() << endl;
	//cout << "c: row" << c.height() << " col " << c.width() << endl;
	//cout << "d: row" << d.height() << " col " << d.width() << endl;
	auto p03 = b.get_solve(a);
	auto p47 = d.get_solve(c);

	for (int i = 0; i < 4; i++) {
		transMat.paras[i] = p03(0, i);
		transMat.paras[i + 4] = p47(0, i);
	}
}
