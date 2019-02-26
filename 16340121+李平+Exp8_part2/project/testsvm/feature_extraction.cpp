/** This file contain code to read images and corresponding lables from the MNIST DATABASE ,store them in the arrays : tr_data , tr_lables and extract features **/

#include "commonfile.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

// Function Declarations
int reverseInt (int i);
vector<int> row_histogram(vector< vector<int> > img,int n_rows,int n_cols);
vector<int> col_histogram(vector< vector<int> > img,int n_rows,int n_cols);
vector< vector<int> > cal_1010_bitmap(vector< vector<int> > img,vector<int> hist_row,vector<int> hist_col,int n_rows,int n_cols);
vector<float> feature_extractor(vector< vector<int> > b);
float number_setpixels(vector< vector<int> > b,int r,int c);

/*--------------------------------------------------------------------------------------------------------------------*/
	
//Function Definations

vector< vector<float> >  read_images_extract_features(char filename[],int n_images)
{
    ifstream data(filename,ios::in | ios::binary);

    if (data.is_open())
    {
        int magic_number,number_images,n_rows,n_cols;
        // Start reading Database
        data.read((char*)&magic_number,sizeof(magic_number));
        magic_number= reverseInt(magic_number);
        data.read((char*)&number_images,sizeof(number_images));
        number_images= reverseInt(number_images);
        data.read((char*)&n_rows,sizeof(n_rows));
        n_rows= reverseInt(n_rows);
        data.read((char*)&n_cols,sizeof(n_cols));
        n_cols= reverseInt(n_cols);
		
        //cout<<"magic_number="<<magic_number<<endl;
        //cout<<"n_images="<<n_images<<endl;
        //cout<<"n_rows="<<n_rows<<endl;
        //cout<<"n_cols="<<n_cols<<endl;

        vector< vector<int> > img(n_rows,vector<int>(n_cols));
		vector< vector<float> > db_features(n_images,vector<float>(n_features));
		
        /** Reading each images into img 2D vector**/
        for(int i=0; i<n_images; ++i)
        {
			//cout << "image:" << i << "processing" << endl;
            for(int r=0; r<n_rows; ++r)
            {
                for(int c=0; c<n_cols; ++c)
                {
                    unsigned char temp=0;
                    data.read((char*)&temp,sizeof(temp));
                    img[r][c]=(int)temp;
                    if(img[r][c]<128) img[r][c]=0;
                    else img[r][c]=1;
                }
            }
			//calculate hog features;
			auto imageMat = Mat(n_rows, n_cols,CV_8UC1);
			for (int i = 0; i < n_rows; i++) {
				for (int j = 0; j < n_cols; j++) {
					imageMat.at<uchar>(i, j) = img[i][j];
				}
			}
			//cout << imageMat.rows << "\t" << imageMat.cols << endl;
			//resize(imageMat, imageMat, Size(32, 32));
			//auto hog = new HOGDescriptor(Size(32, 32), Size(16, 16), Size(8, 8), Size(8, 8), 9);
			auto hog = new HOGDescriptor(Size(28, 28), Size(14, 14), Size(7, 7), Size(7, 7), 9);
			vector<float> descriptors;//324 dimension
			hog->compute(imageMat, descriptors,Size(1,1),Size(0,0));
			//cout << "pass" << endl;
			db_features[i] = descriptors;
			continue;
           // if(i==241 || i==323 || i==388 || i==536 || i==592 || i==598 || i==944) continue;  /** CORRUPT IMAGES **/
				
            vector<int> hist_row(n_rows);
            vector<int> hist_col(n_cols);
			vector< vector<int> > b(10,vector<int>(10));
			
            hist_row = row_histogram(img,n_rows,n_cols);
            hist_col = col_histogram(img,n_rows,n_cols);
			//cout<<"fine1\n";
            b=cal_1010_bitmap(img,hist_row,hist_col,n_rows,n_cols);
			//cout<<"fine3\n";
            db_features[i]=feature_extractor(b);
        }
		return db_features;
    }
    else cout<<"error reading image_database\n";
}
/*
 make sure background black and numbers white
*/
std::vector<float> getFeatureFromSingleImage(CImg<unsigned char> src)
{
	
	static int filename = 0;
	//preprocessing image
	//transfer an image from 3d colorful to a gray and two valued with background black and foregroud white;
	src = preprocessingImage(src);
	//standardlize the image to 20*20 with 4*4*4 black holes
	//first decide the side length of number ,in case of cut the number off in a propriate way
	//such as : a number  '1' with height 10 and width 1 so the side length equals to 10 ,thus the black hole side length is equals to 10*0.2 =2; where 0.2=4/20
	//that is 2+10+2 ; use this new image to resize to 28*28 we get a ,4+20+4
	vector<int>row_his(src.height());
	vector<int>col_his(src.width());
	for (int i = 0; i < row_his.size(); i++) {
		row_his[i] = 0;
	}
	for (int i = 0; i < col_his.size(); i++) {
		col_his[i] = 0;
	}
	cimg_forXY(src, x, y) {
		row_his[y] += src(x, y) >= 128 ? 1 : 0;
		col_his[x] += src(x, y) >= 128 ? 1 : 0;
	}
	int sidey = 0;
	int lowy = -1,highy = -1;
	for (int i = 0; i < row_his.size(); i++) {
		if (row_his[i] > 0) {
			lowy = i;
			break;
		}
	}
	for (int i = row_his.size() - 1; i > 0; i--) {
		if (row_his[i] > 0) {
			highy = i;
			break;
		}
	}
	sidey = highy - lowy + 1;

	int sidex = 0;
	int lowx = -1, highx = -1;
	for (int i = 0; i < col_his.size(); i++) {
		if (col_his[i] > 0) {
			lowx = i;
			break;
		}
	}
	for (int i = col_his.size() - 1; i > 0; i--) {
		if (col_his[i] > 0) {
			highx = i;
			break;
		}
	}
	sidex = highx - lowx + 1;
	int side = sidex > sidey ? sidex : sidey;
	int holeside = 0.2*side;
	lowx = lowx - (side - sidex) / 2;
	highx = highx + (side - sidex) / 2 + 1;
	lowy = lowy - (side - sidey) / 2;
	highy = highy + (side - sidey) / 2 + 1;
	Mat m(side + holeside * 2, side + holeside * 2, CV_8UC1);
	//cout << "side x" << sidex << "\tside y" << sidey << "\t side" << side<<"\tholeside" << holeside << endl;
	for (int r = 0; r < m.rows; r++) {
		for (int c = 0; c < m.cols; c++) {
			if (r < holeside || c < holeside || r >= holeside + side || c >= holeside + side) {
				m.at<uchar>(r, c) = 0;
			}
			else {
				int nr = r - holeside + lowy;
				int nc = c - holeside + lowx;
				if (nr < src.height() && nc < src.width()&&nr >=0 && nc >=0) {
					m.at<uchar>(r, c) = src(nc, nr);
				}
				else {
					m.at<uchar>(r, c) = 0;
				}
			}
		}
	}

	resize(m, m, Size(28, 28));
	for (int i = 0; i < m.rows; i++) {
		for (int j = 0; j < m.cols; j++) {
			if (m.at<uchar>(i, j) <= 128) {
				m.at<uchar>(i, j) = 0;
			}
			else {
				m.at<uchar>(i, j) = 1;
			}
		}
	}

	//auto hog = new HOGDescriptor(Size(32, 32), Size(16, 16), Size(8, 8), Size(8, 8), 9);
	
	auto hog = new HOGDescriptor(Size(28, 28), Size(14, 14), Size(7, 7), Size(7, 7), 9);
	vector<float> descriptors;//324 dimension
	hog->compute(m, descriptors,Size(1,1),Size(0,0));
	return descriptors;
	
}

int reverseInt (int i) //reverse a int from high-endian to low-endian
{
    unsigned char c1, c2, c3, c4;

    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;

    return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}

vector<int> row_histogram(vector< vector<int> > img,int n_rows,int n_cols)
{
    vector<int> r(n_rows);
    for(int i=0; i<n_rows; i++)
    {
        r[i]=0;
        for(int j=1; j<n_cols; j++)
        {
            if(img[i][j-1]!=img[i][j])
                r[i]++;
        }
        if(r[i]>8)//?
            r[i]=0;
    }
    return r;
}

vector<int> col_histogram(vector< vector<int> > img,int n_rows,int n_cols)
{
    vector<int> c(n_cols);
    for(int i=0; i<n_cols; i++)
    {
        c[i]=0;
        for(int j=1; j<n_rows; j++)
        {
            if(img[j-1][i]!=img[j][i])
                c[i]++;
        }
        if(c[i]>8)
            c[i]=0;
    }
    return c;
}

vector< vector<int> > cal_1010_bitmap(vector< vector<int> > a,vector<int> r,vector<int> c,int n_rows,int n_cols)
{
	//cout << "cal1010bitmap" << endl;
	//cout << endl << endl << endl;
	//cout << "n_rows:" << n_rows << "n_cols:" << n_cols << endl;
    int row=n_rows;
    int col=n_cols;
    int i,j,low=0,high=0;
    vector< vector<int> > b(10,vector<int>(10));
	
    for(i=1; i<row; i++)
    {
        if(r[i-1]!=r[i])
        {
            low=i;
            break;
        }
    }
	//cout <<"low:"<< low << endl;
    for(i=row-1; i>0; i--)
    {
        if(r[i]!=r[i-1])
        {
            high=i-1;
            break;
        }
    }
	//cout << "high:" << high << endl;
    row=high-low+1;
    i=row%10;
    i=i>0?10-i:i;
    row=row+i;
    if(i%2==0)
        low=low-(i/2);
    else
        low=low-(i/2)-1;
    high=high+(i/2);
    for(i=low; i<=high; i++)
    {
		for (j = 0; j < col; j++)
		{
			//int temp = 0;
			if (i >= n_rows||i < 0) a[i - low][j] = 0;
			a[i - low][j] = a[i][j];
		}
            
    }
	
    // for column
    for(i=1; i<col; i++)
    {
        if(c[i-1]!=c[i])
        {
            low=i;
            break;
        }
    }
	//cout << "low:" << low << endl;
    for(i=col-1; i>0; i--)
    {
        if(c[i]!=c[i-1])
        {
            high=i-1;
            break;
        }
    }
	//cout << "high:" << high << endl;
    col=high-low+1;
    i=col%10;
    i=i>0?10-i:i;
    col=col+i;
    if(i%2==0)
        low=low-(i/2);
    else
        low=low-(i/2)-1;
    high=high+(i/2);
    for(j=low; j<=high; j++)
    {
		for (i = 0; i < row; i++) {
			if (i<0 || i>n_rows || j < 0 || j >= n_cols) {
				//cout << "row:" << row << " col:" << col << endl;
				//cout << "high:" << high << " low:" << low << endl;
				//cout << i << "," << j << endl;
				//cout << n_rows << "," << n_cols << endl;
			}
			if (j >= n_cols||j<0) {//out of range
				a[i][j - low] = 0;
			}
			else {
				a[i][j - low] = a[i][j];
			}
			//cout << "cause the bug so not be printed" << endl;
		}
    }
	
    // mapping 10x10 matrix
    int row10=row/10;
    int col10=col/10;
    int x,y;

    for(i=0; i<10; i++)
        for(j=0; j<10; j++)
            b[i][j]=0;

    for(i=0; i<row; i++)
    {
        for(j=0; j<col; j++)
        {
            x=i/row10;
            y=j/col10;
            if(a[i][j]==1)
                b[x][y]+=1;
        }
    }

    //0-1 bitmap
    float thresh=(0.05)*row10*col10;

    for(i=0; i<10; i++)
        for(j=0; j<10; j++)
            if(b[i][j]>thresh)
                b[i][j]=1;
            else
                b[i][j]=0;
	if (sw) {
		CImg<unsigned char> temp(10, 10, 1, 1);
		cimg_forXY(temp, x, y) {
			temp(x, y) = b[y][x] == 0 ? 0 : 255;
		}
		temp.display();
	}
	/*CImg<unsigned char> temp(10, 10, 1, 1);
	cimg_forXY(temp, x, y) {
		temp(x, y) = b[y][x] == 0 ? 0 : 255;
	}
	temp.display();*/
    return b;
}

vector <float> feature_extractor(vector< vector<int> > b)
{
    int f=0,i,j;
    float cnt,setpix=0;
	vector <float> features;
	//9
    for(i=0; i<=4; i+=2)
    {
        for(j=0; j<=4; j+=2)
        {
            setpix = number_setpixels(b,i,j);
			features.push_back(setpix);
        }
    }
	//100	
	for(i=0;i<10;++i)
	{
		cnt=0;
		for(j=0;j<10;++j) 
		{
			cnt+=b[j][i];
			features.push_back(cnt);
		}
	}
	//100
	for(i=0;i<10;++i)
	{
		cnt=0;
		for(j=0;j<10;++j)
		{		
			cnt+=b[i][j];
			features.push_back(cnt);
		}
	}
	
	float max_brow_cnt=0,max_brow=0,max_bcol_cnt=0,max_bcol=0;
	for(i=0;i<10;++i)
	{
		cnt=0;
		for(j=0;j<10;++j) cnt+=b[i][j];
		if(max_brow_cnt < cnt)
		{
			max_brow_cnt=cnt;
			max_brow=i;
		}
		
	}
	for(i=0;i<10;++i)
	{
		cnt=0;
		for(j=0;j<10;++j) cnt+=b[j][i];
		if(max_bcol_cnt < cnt)
		{
			max_bcol_cnt=cnt;
			max_bcol=i;
		}
	}
	//4
	features.push_back(max_brow);
	features.push_back(max_bcol);
	features.push_back(max_brow_cnt);
	features.push_back(max_bcol_cnt);
	
	//1
	cnt=0;
	for(i=3;i<=4;++i)
	{
		for(j=3;j<=4;++j)
		{
			cnt+=b[i][j];
		}
	}
	cnt=cnt>0?1:0;

	features.push_back(cnt);
	//features.push_back(1.2);//this test ability
	
	return features;
}

float number_setpixels(vector< vector<int> > b,int r,int c)
{
    int i,j;
	float ret=0;
    for(i=0;i<4;++i)
    {
        for(j=0;j<4;++j)
        {
            ret+=(float)b[i+r][j+c];
        }
    }
    return ret;
}

vector<int> read_lables(char filename[],int n_images)
{
    ifstream lables(filename,ios::in | ios::binary);

    if (lables.is_open())
    {
        int magic_number,number_images;
        unsigned char lable;
		vector<int> db_lable(n_images);
			
        // Start reading Database
        lables.read((char*)&magic_number,sizeof(magic_number));
        magic_number= reverseInt(magic_number);
        //cout<<magic_number<<"\n";
        lables.read((char*)&number_images,sizeof(number_images));
        number_images= reverseInt(number_images);
        //cout<<number_images<<"\n";

        for(int i=0;i<n_images;++i)
        {
            lables.read((char*)&lable,sizeof(lable));
            db_lable[i]=(int)lable;
            //cout<<(int)lable<<endl;
        }
		return db_lable;
    }
    else cout<<"error reading lables_database\n";
}
/*
gauss filting
change to gray
use ostu to segmentation the image 
turn background into black, the number into white
*/
CImg<unsigned char> preprocessingImage(CImg<unsigned char>& src)
{
	//compromise
	
	if (src.spectrum() == 1)return src;
	assert(src.spectrum() == 3);
	cout << "spectrum 3"<<endl;
	//src.display();

	
	src.resize(28, 28, -100, -100, 1);
	int width = src.width();
	int height = src.height();
	//src.display();
	Otsu os;
	auto result = os.ImageSegmentationProcessing(src);
	int back = result(0, 0) + result(width - 1, height - 1) + result(0, height - 1) + result(width - 1, 0);
	back = back / 4;
	cout << "\nback"<<back << endl;
	back == back < 128 ? 0 : 255;
	//confirm backgroud is black not white;
	if (back == 255) {
		cout << "white backgroud ,reverse" << endl;
		cimg_forXY(result, x, y) {
			result(x, y) = 255 - result(x, y);
		}
	}
	CImg<unsigned char> res(result.width() + 10, result.height() + 10, 1, 1);
	cimg_forXY(res, x, y) {
		res(x, y) = 0;
	}
	cimg_forXY(result, x, y) {
		res(x + 5, y + 5) = result(x, y);
	}
	return res;
}
