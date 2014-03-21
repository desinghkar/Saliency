#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "dir_files/dir_files.h"

#include <iostream>

using namespace std;
using namespace cv;

int main(int ac, char* av[])
{
	if(ac<2)
	{
		cout<<"Enter the path of the marked images in green color"<<endl;
		return 0;
	}
	string dir_img =av[1];
	vector<string> list_img;
	dir_files IMG(dir_img, list_img, "jpg");

	if(IMG.getFilesExtCount()==0)
	{
		cout<<"No images found"<<endl;
		return 0;

	}

	for(int i=0; i<list_img.size(); i++)
	{
		string path_img = dir_img+"/"+list_img[i];
		cout<<"@ "<<path_img<<endl;

		Mat img = imread(path_img.c_str());
		
		for(int a=0; a<img.rows; a++)
			for(int b=0; b<img.cols; b++)
			{
			//	cout<<"a , b "<<a<<" "<<b<<endl;
				if(img.at<Vec3b>(a, b)[1] ==255 && img.at<Vec3b>(a, b)[0]==0 && img.at<Vec3b>(a, b)[2]==0)
				{

				}
				else
				{
					img.at<Vec3b>(a, b)[0]=0;
					img.at<Vec3b>(a, b)[1]=0;
					img.at<Vec3b>(a, b)[2]=0;
				}
			}
		stringstream ss;
		path_img.erase(path_img.find_last_of("."), string::npos);
		ss<<path_img<<"_gt.png";
		imwrite(ss.str(), img);
	}
	return 0;
}
