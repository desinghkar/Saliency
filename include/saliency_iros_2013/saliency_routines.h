#ifndef SALIENCY_ROUTINES_H
#define SALIENCY_ROUTINES_H
#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET
//PCL include files
#include <pcl/filters/filter.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/features/normal_3d.h>
#include <pcl/features/boundary.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/segmentation/region_growing.h>
#include <pcl/kdtree/kdtree.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/surface/mls.h>
//OpenCV include files
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

//Cpp
#include <fstream>
#include <string.h>


using namespace std;
using namespace pcl;
using namespace cv;

class Depth_Saliency
{
	//Input to the depth_saliency is only pcd but since it doesn't have RGB information we have image also
	string path_pcd;
	string path_image;

	//Output is depth_saliency image with instantiations based on the different methods being employed and also the file of the
	//depth_saliency score and features attached to it
	Mat depth_saliency_map;


	// Clouds used in the class
	PointCloud<PointXYZ> cloud;
	PointCloud<PointXYZRGB> cloud_rgb;
	PointCloud<PointXYZRGB> cloud_rgb_no_nans;
	PointCloud<Normal> cloud_normals;
	PointCloud<PointXYZRGB> cloud_segmented;


	Mat image;

	//Writer to write the pcd files
	PCDWriter writer;

	int original_points;
	int no_bins;
	vector<int> indices;
	vector<vector<int> > segment_indices;
	vector<vector<int> > map_indices;
	PointIndicesPtr indices_ptr;

	vector<double> saliency;
	vector<vector<double> > hist;
	vector<vector<double> > colorHist;
	vector<vector<double> > verticalityHist;
	vector<vector<double> > scaleSizeHist;
	vector<vector<double> > compactnessHist;
	vector<vector<double> > perspectiveHist;
	vector<vector<double> > discontinuitiesHist;

	vector<double> depth_saliency_value;
	vector<double> contourCompactness;

	public:
	string path_depth_saliency_map_dir;
	Depth_Saliency(){ };
	~Depth_Saliency(){ };
	Depth_Saliency(string, string, string); //Input pcd, images and output folder path

	//setInputCloud
	void setInputCloud(PointCloud<PointXYZ>::Ptr, Mat&);


	//Read cloud
	bool readCloud(string);

	//Read image
	void readImage(string);

	//Merge cloud + image to get RGBD cloud
	void mergeCloudImage();

	//Remove nans
	void removeNaNs();

	//Resampling and smoothing to avoid varied surface curvatures and noisy pointcloud
	void resamplingSmoothing();

	//Calculate Normals
	void estimateNormals();

	//Region Growing
	void regionGrowing();

	//Normalize Hist
	void normalizeHist();

	//Salient Histogram
	void computeHistogram(int);

	//Dotproduct to compute the saliency
	void computeSaliency();


	//Compute the color histogram; avg of the rgb of a region
	void computeColorHist();

	//Compute the verticality histogram; which is the avg angle of the surface with respect to the camera
	void computeVerticalityHist(int);

	//Compute the scale and size of the region in an histogram format
	void computeScaleSizeHist(int);

	//Compute the compactness of the region
	void computeCompactnessHist();

	//Compute the contourcompactness of the region
	void computeContourCompact();

	//Compute the perspective score of the region
	void computePerspectiveScores();

	//Compute the discontinuities
	void computeDiscontinuities();

	//Map to a 2D image
	void mapTo2DImage(int);

	//Write the features + depth score into a file
	void writeFeatures(string);

	//Write the depth saliency values into a file
	void writeDepthSaliency(string);

	
};

double dotProduct(vector<double>, vector<double>);
double findAngle(Normal, Normal);

#endif


