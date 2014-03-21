#include "saliency_iros_2013/saliency_routines.h"
#include "dir_files/dir_files.h"

int main(int ac, char* av[])
{
	if(ac<3)
	{
		cout<<"Enter the paths to the pcds and paths to the pngs and also paths to the dir where the output blurred images are supposed to be saved"<<endl;
		return 0;
	}

	string dir_pcd = av[1];
	string dir_img = av[2];
	string dir_blur = av[3];

	vector<string> list_pcd, list_img;

	dir_files PCD(dir_pcd, list_pcd, "pcd");
	dir_files IMG(dir_img, list_img, "tiff");

	if(PCD.getFilesExtCount()!= IMG.getFilesExtCount())
	{
		cout<<"No of images is not equal to the num of pcds in the folder"<<endl;
		return 0;
	}

	for(int i=0; i<list_pcd.size(); i++)
	{
		string path_img = dir_img+"/"+list_img[i];
		string path_pcd = dir_pcd+"/"+list_pcd[i];

		cout<<"@ "<<path_img<<endl;
		PointCloud<PointXYZ> cloud;

		if(io::loadPCDFile<PointXYZ> (path_pcd.c_str(), cloud) ==-1)
		{
			cout<<"Couldn't read the file"<<endl;
			return 0;
		}

		double minZ=999;
		for(size_t j=0; j<cloud.points.size(); j++)
		{
			if(minZ>cloud.points[j].z)
				minZ = cloud.points[j].z;
		}
		cout<<"The minZ is "<<minZ<<endl;

		Mat image = imread(path_img.c_str());
//		namedWindow("image");
//		imshow("image", image);
//		waitKey(0);

		double depth_fg=0;
		cout<<"Give the depth for image "<<path_img<<" the foreground: ";
		cin>>depth_fg;

		Mat image_fg = Mat::zeros(image.size(), image.type());
		Mat image_bg = Mat::zeros(image.size(), image.type());
		Mat image_blur = Mat::zeros(image.size(), image.type());
/*
		for(int x=0; x<image.cols; x++)
			for(int y=0; y<image.rows; y++)
			{
				image_bg.at<Vec3b>(x, y)[0] = image.at<Vec3b>(x, y)[0];
				image_bg.at<Vec3b>(x, y)[1] = image.at<Vec3b>(x, y)[1];
				image_bg.at<Vec3b>(x, y)[2] = image.at<Vec3b>(x, y)[2];

			}
*/		GaussianBlur(image, image_blur, cv::Size(27, 27), 0);
//		namedWindow("blur");
//		imshow("blur", image_blur);
//		waitKey(0);

		for(size_t j=0; j<cloud.points.size(); j++)
		{
			int b = (j+1)%image.cols - 1;
			int a = (j+1)/image.cols - 1;

			if(((j+1)%image.cols)!= 0)
				a++;
			if((j+1)%image.cols==0)
				b = image.cols-1;
			if(cloud.points[j].z<=depth_fg)
			{
				image_blur.at<Vec3b>(a, b)[0] = image.at<Vec3b>(a, b)[0];
				image_blur.at<Vec3b>(a, b)[1] = image.at<Vec3b>(a, b)[1];
				image_blur.at<Vec3b>(a, b)[2] = image.at<Vec3b>(a, b)[2];
				
			}
		}
	//	imshow("blur", image_blur);
	//	waitKey(0);
		stringstream ss;
		path_img.erase(path_img.find_last_of("."), string::npos);
		ss<<path_img<<"_blurred_bg.png";
		
		imwrite(ss.str(), image_blur);

	}
	return 0;

}
