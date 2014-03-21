#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <fstream>


using namespace std;
using namespace cv;

int main(int ac, char* av[])
{
	if(ac<5)
	{
		cout<<"Enter the file list of text files of depth saliency and features & file list of rgb salient maps, ground truths and output path for all the features"<<endl;
		return 0;
	}
	ifstream depth_file, rgb_file, gt_file;
	depth_file.open(av[1]);
	rgb_file.open(av[2]);
	gt_file.open(av[3]);
	string path_out = av[4];

	int count=1;
	while(!depth_file.eof()&&!rgb_file.eof())
	{
		cout<<"IMAGE NUMBER "<<count<<" ##################################################### IMAGE NUMBER "<<count<<endl;
		ofstream rgbd_file;
		stringstream ss;
		ss<<count;
		string path_file;
		if(count<10)
			path_file=path_out+"/rgbd_features_0"+ss.str()+".txt";
		else
			path_file=path_out+"/rgbd_features_"+ss.str()+".txt";

		rgbd_file.open(path_file.c_str());

		string img, depth, gt;
		depth_file>>depth;
		rgb_file>>img;
		gt_file>>gt;

		cout<<"depth is "<<depth<<endl;
		cout<<"img is "<<img<<endl;
		cout<<"gt is "<<gt<<endl;

		Mat image = imread(img.c_str());
		Mat gt_img = imread(gt.c_str());
	//	namedWindow("image");
	//	imshow("image",image);
	//	waitKey(0);
		ifstream depth_features;
		depth_features.open(depth.c_str());
		cout<<"Writing the features into a file"<<endl;
		cout<<"rows is "<<image.rows<<" cols is "<<image.cols<<endl;
		for(int i=0; i<image.rows; i++)
		{
			cout<<".";
			for(int j=0; j<image.cols; j++)
			{



				double image_saliency = (double) image.at<uchar>(i, j) / 255;
				double gt_value = (double) gt_img.at<uchar>(i, j) / 255;
				rgbd_file<<gt_value<<" "<<image_saliency<<" ";
				for(int k=0; k<35; k++)
				{
					
//					cout<<"i is "<<i<<" j is "<<j<<" and k is "<<k<<endl;
					double depth_saliency_features;
					depth_features>>depth_saliency_features;

					rgbd_file<<depth_saliency_features<<" ";

				}
				rgbd_file<<endl;
			}
		}
		count++;
		cout<<endl<<"Done!!"<<endl;

	}
	return 0;
}
