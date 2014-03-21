#include "saliency_iros_2013/saliency_routines.h"
#include "dir_files/dir_files.h"

int main(int ac, char* av[])
{
	if(ac<2)
	{
		cout<<"Enter the path to pcds and enter the path to fixations maps and also fixationcount maps"<<endl;
		return 0;

	}
	string dir_pcd = av[1];
	string dir_eye = av[2];
	string dir_fix = av[3];

	vector<string> list_pcd, list_eye, list_fix;
	dir_files PCD(dir_pcd, list_pcd, "pcd");
	dir_files EYE(dir_eye, list_eye, "png");
	dir_files FIX(dir_fix, list_fix, "png");

	if(PCD.getFilesExtCount()!= EYE.getFilesExtCount() && PCD.getFilesExtCount()!= FIX.getFilesExtCount())
	{
		cout<<"No of images is not equal to the num of pcds in the folder"<<endl;
		return 0;
	}
	vector<int> depth_of_field_plot1, depth_of_field_plot2, depth_of_field_plot3;
	depth_of_field_plot1.resize(10);
	depth_of_field_plot2.resize(10);
	depth_of_field_plot3.resize(10);

	for(int i=0; i<10; i++)
	{
		depth_of_field_plot1[i]=depth_of_field_plot2[i]=0;
	}
	for(int i=0; i<list_pcd.size(); i++)
	{
		string path_img = dir_eye+"/"+list_eye[i];
		string path_pcd = dir_pcd+"/"+list_pcd[i];
		string path_fix = dir_fix+"/"+list_fix[i];
		
		cout<<"@ "<<path_img<<endl;
		PointCloud<PointXYZ> cloud;
		if(io::loadPCDFile<PointXYZ> (path_pcd.c_str(), cloud)==-1 )
		{
			cout<<"Coudn't read the file"<<endl;
			return 0;
		}
		
		double maxZ=0;
		for(size_t j=0; j<cloud.points.size(); j++)
		{
			if(maxZ<cloud.points[j].z)
				maxZ = cloud.points[j].z;
		}

		cout<<"The maxZ is "<<maxZ<<endl;
		cout<<"The maxZ/10 would be "<<maxZ/10.0<<endl;
		double interval = maxZ/10.0;

		Mat image_eye = imread(path_img.c_str());
		Mat image_fix = imread(path_fix.c_str());
		namedWindow("image");
		imshow("image", image_eye);
		waitKey(0);
		for(size_t j=0; j<cloud.points.size(); j++)
		{
		//	cout<<" j = "<<j;
			int b = (j+1)%image_eye.cols - 1;
			int a = (j+1)/image_eye.cols - 1;

			if(((j+1)%image_eye.cols)!= 0)
				a++;
			if((j+1)%image_eye.cols==0)
				b = image_eye.cols-1;
			
		//	cout<<" a, b "<<a<<" "<<b<<endl;
			if(image_eye.at<uchar>(a, b)>0)
			{
				//3 cases 
				//1. number of unique fixations at thsi depth
				//2. number of fixations at this depth including repeating ones
				
				double index = cloud.points[j].z/interval;
				if((floor(index)==index)&&(index!=0))
				{
					depth_of_field_plot1[(int)(index-1)]++;
					depth_of_field_plot2[(int)(index-1)]+=image_fix.at<uchar>(a, b);
				}
				else
				{
					depth_of_field_plot1[(int)index]++;
					depth_of_field_plot2[(int)index]+=image_fix.at<uchar>(a, b);
				}
				
			}
			

		}
		
	}
	cout<<"Depth of filed plot 1 is"<<endl;
	for(int i=0; i<10; i++)
		cout<<depth_of_field_plot1[i]<<endl;
	cout<<"Depth of filed plot 2 is"<<endl;
	for(int i=0; i<10; i++)
		cout<<depth_of_field_plot2[i]<<endl;


}
