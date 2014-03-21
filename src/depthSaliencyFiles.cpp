#include "saliency_iros_2013/saliency_routines.h"
#include "dir_files/dir_files.h"
#include <algorithm>

int main(int ac, char* av[])
{
	if(ac<5)
	{
		cout<<"Please give the input cloud dir, input image dir, output dir for depthsaliency values, output dir for features"<<endl;
		return 0;
	}

	string dir_pcd = av[1];
	string dir_img = av[2];
	string dir_out_d = av[3];
	string dir_out_f = av[4];
	
	vector<string> list_pcd, list_img;

	dir_files PCD(dir_pcd, list_pcd, "pcd");
	dir_files IMG(dir_img, list_img, "png");

	if(PCD.getFilesExtCount() != IMG.getFilesExtCount() )
	{
		cout<<"Num of images is not equal to num of pcds in the folders"<<endl;
		return 0;
	}

	for(int i=0; i<list_pcd.size(); i++)
	{
		string path_img= dir_img+"/"+list_img[i];
		string path_pcd= dir_pcd+"/"+list_pcd[i];
		
		int k=i+1;
		cout<<"IMAGE NUM "<<k<<"######################################################################################### IMAGE NUM "<<k<<endl;
		//Compute the depth saliency scores and write interms of images and files
		Depth_Saliency DS(path_pcd, path_img, dir_out_d);
		DS.mergeCloudImage(); cout<<"Done with merging"<<endl;
		DS.removeNaNs(); cout<<"Done with removal of nans"<<endl;
		DS.estimateNormals(); cout<<"Done with normals estimation"<<endl; //commented as below one is being used from now
//		DS.resamplingSmoothing(); cout<<"Done with resampling and normals estimation"<<endl;
		DS.regionGrowing(); cout<<"Done with region growing"<<endl;
		DS.computeHistogram(100); cout<<"Done with histogram computation"<<endl;
		DS.computeSaliency(); cout<<"Done with computeSaliency"<<endl;
		DS.mapTo2DImage(k); cout<<"Done with depth map creation"<<endl;
		
		//Now compute the features	
		DS.computeColorHist(); cout<<"Done with colorHist creation"<<endl;
		DS.computeVerticalityHist(20); cout<<"Done with verticality creation"<<endl;
		DS.computeScaleSizeHist(9); cout<<"Done with scaleSize creation"<<endl;
		DS.computeCompactnessHist(); cout<<"Done with compact creation"<<endl;
		DS.computeContourCompact(); cout<<"Done with the contour compact creation"<<endl;
		DS.computePerspectiveScores(); cout<<"Done with the perspective Hist creation"<<endl;
		DS.computeDiscontinuities(); cout<<"Done with the discontinuitiesHist creation"<<endl;
		stringstream ss;
		ss<<k;
		string path_features;
		if(k<10)
			path_features = dir_out_f+"/features_map_0"+ss.str()+".txt";
		else
			path_features = dir_out_f+"/features_map_"+ss.str()+".txt";

		//Write features into a file
		DS.writeFeatures(path_features);
		
	}
	return 0;
}

