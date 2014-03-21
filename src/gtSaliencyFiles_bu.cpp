/* This code is to generate the saliency scores from the ground truth images into their respective files */
//Cpp
#include <iostream>
#include <fstream>
#include <dir_files/dir_files.h>

//OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main(int ac, char* av[])
{
	//Input folder containing the images of saliency maps and output folder where the files have to be written
	if(ac<3)
	{
		cout<<"Enter the input folder for the Gt saliency maps and output folder where files have to be written"<<endl;
		return 0;
	}
	string dir_in = av[1];
	string dir_out = av[2];
	vector<string> list;
	
	dir_files D(dir_in, list, "png");
	
	for(int k=0; k<list.size(); k++)
	{
		cout<<list[k]<<endl;
		string in_path=dir_in+"/"+list[k];
		Mat image = imread(in_path.c_str());
		ofstream fileout;

		string temp = list[k];
		temp.erase(temp.find_last_of("."), string::npos);
		string out_path = dir_out+"/"+temp+".txt";
		
		fileout.open(out_path.c_str());
		for(int i=0; i<image.rows; i++)
		{	
			for(int j=0; j<image.cols; j++)
			{
				double t = image.at<uchar>(i, j);	
				fileout<<t<<endl;
			}
		}
		fileout.close();
	}

	return 0;
}
