#include "saliency_iros_2013/saliency_routines.h"
#include <algorithm>

Depth_Saliency::Depth_Saliency(string pcd, string img, string dir)
{
	readCloud(pcd);
	readImage(img);
	path_pcd = pcd;
	path_image = img;
	path_depth_saliency_map_dir = dir;

	cout<<"Cloud and Image read into the program"<<endl;
}

bool Depth_Saliency::readCloud(string pcd)
{
	cout<<"pcd is at "<<pcd<<endl;
	if(io::loadPCDFile<PointXYZ> (pcd, cloud)==-1)
	{
		cout<<"Couldn't read the file"<<endl;
		return false;
	}
	return true;
}

void Depth_Saliency::readImage(string img)
{
	image = imread(img);	
//	imshow("image", image);
//	waitKey(0);
}

void Depth_Saliency::mergeCloudImage()
{
	cloud_rgb.height = cloud.height;
	cloud_rgb.width = cloud.width;
	cloud_rgb.points.resize(cloud.points.size());

	cout<<"cloud_rgb points "<<cloud_rgb.points.size()<<endl;
	for(size_t i=0; i<cloud_rgb.points.size(); ++i)
	{
		int b = (i+1)%640 - 1;
		int a = (i+1)/640 - 1;
		if((i+1)%640!=0)
			a++;
		
		cloud_rgb.points[i].x = cloud.points[i].x;
		cloud_rgb.points[i].y = cloud.points[i].y;
		cloud_rgb.points[i].z = cloud.points[i].z;
		cloud_rgb.points[i].r = image.at<Vec3b>(a, b)[2];
		cloud_rgb.points[i].g = image.at<Vec3b>(a, b)[1];
		cloud_rgb.points[i].b = image.at<Vec3b>(a, b)[0];
	}
}

void Depth_Saliency::removeNaNs()
{
	cloud_rgb.is_dense = false;
	cloud_rgb_no_nans.is_dense = false;

	removeNaNFromPointCloud(cloud_rgb, cloud_rgb_no_nans, indices);
	original_points = cloud_rgb_no_nans.points.size();

	cout<<"After removing NaNs the size is "<<original_points<<endl;
	stringstream s;
	s << path_depth_saliency_map_dir << "/sampleRGB.pcd";

	PointCloud<PointXYZRGB>::Ptr cloud_rgb_ptr (new PointCloud<PointXYZRGB>() );
	cloud_rgb_ptr->height = cloud_rgb_no_nans.height;
	cloud_rgb_ptr->width = cloud_rgb_no_nans.width;
	cloud_rgb_ptr->points.resize(cloud_rgb_no_nans.points.size());

	for(size_t i=0; i<cloud_rgb_ptr->points.size(); ++i)
	{
		cloud_rgb_ptr->points[i].x = cloud_rgb_no_nans.points[i].x;
		cloud_rgb_ptr->points[i].y = cloud_rgb_no_nans.points[i].y;
		cloud_rgb_ptr->points[i].z = cloud_rgb_no_nans.points[i].z;
		cloud_rgb_ptr->points[i].r = cloud_rgb_no_nans.points[i].r;
		cloud_rgb_ptr->points[i].g = cloud_rgb_no_nans.points[i].g;
		cloud_rgb_ptr->points[i].b = cloud_rgb_no_nans.points[i].b;
	}

	//writer.write<pcl::PointXYZRGB> (s.str(), *cloud_rgb_ptr, false);
	
}

void Depth_Saliency::estimateNormals()
{
	NormalEstimation<PointXYZRGB, Normal> ne;
	PointCloud<PointXYZRGB>::Ptr cloud_ptr (new PointCloud<PointXYZRGB>() );
	cloud_ptr->height = cloud_rgb_no_nans.height;
	cloud_ptr->width = cloud_rgb_no_nans.width;
	cloud_ptr->points.resize(cloud_rgb_no_nans.points.size());

	for(size_t i=0; i<cloud_ptr->points.size(); ++i)
	{
		cloud_ptr->points[i].x = cloud_rgb_no_nans.points[i].x;
		cloud_ptr->points[i].y = cloud_rgb_no_nans.points[i].y;
		cloud_ptr->points[i].z = cloud_rgb_no_nans.points[i].z;
		cloud_ptr->points[i].r = cloud_rgb_no_nans.points[i].r;
		cloud_ptr->points[i].g = cloud_rgb_no_nans.points[i].g;
		cloud_ptr->points[i].b = cloud_rgb_no_nans.points[i].b;

	}

	ne.setInputCloud(cloud_ptr);
	search::KdTree<PointXYZRGB>::Ptr tree(new search::KdTree<PointXYZRGB>());
	ne.setSearchMethod(tree);
	ne.setRadiusSearch(0.03);
	ne.compute(cloud_normals);

	cout<<"Normals estimated"<<endl;

}

void Depth_Saliency::regionGrowing()
{
	PointCloud<PointXYZ>::Ptr cloud_no_nans_ptr (new PointCloud<PointXYZ>());
	PointCloud<Normal>::Ptr cloud_normals_ptr (new PointCloud<Normal>());

	cloud_no_nans_ptr->points.resize(cloud_rgb_no_nans.points.size());
	cloud_no_nans_ptr->height = cloud_rgb_no_nans.height;
	cloud_no_nans_ptr->width = cloud_rgb_no_nans.width;

	cloud_normals_ptr->points.resize(cloud_normals.points.size());
	cloud_normals_ptr->height = cloud_normals.height;
	cloud_normals_ptr->width = cloud_normals.width;
	
	cout<<"Cloudrgb no nans size is "<<cloud_no_nans_ptr->points.size()<<endl;
	for(size_t i=0; i<cloud_rgb_no_nans.points.size(); ++i)
	{
		cloud_no_nans_ptr->points[i].x = cloud_rgb_no_nans.points[i].x;
		cloud_no_nans_ptr->points[i].y = cloud_rgb_no_nans.points[i].y;
		cloud_no_nans_ptr->points[i].z = cloud_rgb_no_nans.points[i].z;
		
		cloud_normals_ptr->points[i].normal_x = cloud_normals.points[i].normal_x;
		cloud_normals_ptr->points[i].normal_y = cloud_normals.points[i].normal_y;
		cloud_normals_ptr->points[i].normal_z = cloud_normals.points[i].normal_z;
	}

	RegionGrowing<PointXYZ> rg;
	rg.setSmoothMode(false);
	rg.setCurvatureTest(false);
	rg.setCloud(cloud_no_nans_ptr);
	rg.setNormals(cloud_normals_ptr);
	rg.segmentPoints();
	segment_indices = rg.getSegments();

	cout<<"No of segments after region growing is "<<segment_indices.size()<<endl;

}

void Depth_Saliency::normalizeHist()
{
	for(vector<vector<double> >::size_type u=0; u<hist.size(); ++u)
	{
		double sum = 0;
		for(vector<double>::size_type v=0; v<hist[u].size(); ++v)
		{
			sum = sum + hist[u][v]*hist[u][v];
		}
		if(sqrt(sum)!=0)
		{
			for(vector<double>::size_type v=0; v<hist[u].size(); ++v)
			{
				hist[u][v]/=sqrt(sum);
			}
		}
	}
	cout<<"normalizeHist is done "<<endl;
}

void Depth_Saliency::computeHistogram(int n_bins)
{
	no_bins = n_bins;

	
	for(vector<vector<int> >::size_type u=0; u<segment_indices.size(); ++u)
	{
		bool indices_flag = false;
		int no_points = segment_indices[u].size();
		double k = ((no_points-1)*(no_points-1)+(no_points))/2;
		double ki = 1.0/k;

		vector<double> histogram;
		histogram.resize(no_bins);

		for(size_t i=0; i<histogram.size(); ++i)
			histogram[i] = 0.0;

		if(segment_indices[u].size()>100&&segment_indices[u].size()<10000) //Minimum size of the patch should have 100 points
		{
			double segment_z_max = 0.0;
			for(vector<int>::size_type v=0; v<segment_indices[u].size(); ++v)
			{
				PointXYZ r;
				r.z = cloud_rgb_no_nans.points[segment_indices[u][v]].z;
				segment_z_max +=r.z/segment_indices[u].size();

				for(vector<int>::size_type w=v+1; w<segment_indices[u].size(); ++w)
				{
					Normal p, q;
					p.normal_x = cloud_normals.points[segment_indices[u][v]].normal_x;
					p.normal_y = cloud_normals.points[segment_indices[u][v]].normal_y;
					p.normal_z = cloud_normals.points[segment_indices[u][v]].normal_z;

					q.normal_x = cloud_normals.points[segment_indices[u][w]].normal_x;
					q.normal_y = cloud_normals.points[segment_indices[u][w]].normal_y;
					q.normal_z = cloud_normals.points[segment_indices[u][w]].normal_z;

					double angle = abs(findAngle(p, q));
					double interval = M_PI/(1.0*no_bins);
					double index = angle/interval;
					//cout<<"index is "<<index<<endl;
					if((floor(index)==index)&&(index!=0))
						histogram[(int)(index-1)]+=ki;
					else
						histogram[(int)index]+=ki;

				}
			}
			if(segment_z_max<3)
				indices_flag = true;
		}
		else if(segment_indices[u].size()>=10000)
		{
			double segment_z_max = 0.0;
			PointCloud<PointNormal>::Ptr c (new PointCloud<PointNormal>());
			PointCloud<PointNormal>::Ptr cv (new PointCloud<PointNormal>());
			c->points.resize(segment_indices[u].size());
			c->height = segment_indices[u].size();
			c->width = 1;

			for(vector<int>::size_type i=0; i<segment_indices[u].size(); ++i)
			{
				c->points[i].x = cloud_rgb_no_nans.points[segment_indices[u][i]].x;
				c->points[i].y = cloud_rgb_no_nans.points[segment_indices[u][i]].y;
				c->points[i].z = cloud_rgb_no_nans.points[segment_indices[u][i]].z;

				c->points[i].normal_x = cloud_normals.points[segment_indices[u][i]].normal_x;
				c->points[i].normal_y = cloud_normals.points[segment_indices[u][i]].normal_y;
				c->points[i].normal_z = cloud_normals.points[segment_indices[u][i]].normal_z;
			}

			VoxelGrid<PointNormal> vx;
			vx.setInputCloud(c);
			vx.setLeafSize(0.01f, 0.01f, 0.01f);
			vx.filter(*cv);

			cout<<"Special case: points before voxelizing is "<<c->points.size()<<" and afteris "<<cv->points.size()<<endl;

			int n_points = cv->points.size();
			double l = ((n_points-1)*(n_points-1) + (n_points-1))/2;
			double li = 1.0/l;

			for(size_t i=0; i<cv->points.size(); ++i)
			{
				PointXYZ r;
				r.z = cv->points[i].z;
				segment_z_max += r.z/cv->points.size();
				for(size_t j=i+1; j<cv->points.size();++j)
				{
					Normal p, q;
					p.normal_x = cv->points[i].normal_x;
					p.normal_y = cv->points[i].normal_y;
					p.normal_z = cv->points[i].normal_z;
					
					q.normal_x = cv->points[j].normal_x;
					q.normal_y = cv->points[j].normal_y;
					q.normal_z = cv->points[j].normal_z;

					double angle = abs(findAngle(p, q));
					double interval = M_PI/(1.0*no_bins);
					double index = angle/interval;

					if((floor(index)==index)&&(index!=0))
						histogram[(int)(index-1)]+=li;
					else
						histogram[(int)index]+=li;
				
			
				}
			}
			if(segment_z_max<3)
				indices_flag = true;
		}
		//For points of greater area of a region some cases has to be written here!!!
		if(indices_flag)	
		{
			map_indices.push_back(segment_indices[u]);
			hist.push_back(histogram);
		//	cout<<endl<<"histogram is "<<endl;
		//	for(size_t i=0; i<histogram.size(); ++i)
		//		cout<<histogram[i];
		}
	}
	cout<<"computeHistogram is done "<<endl;
	cout<<"No of segments before computeHistogram is "<<segment_indices.size()<<" and after is "<<hist.size()<<" , "<<map_indices.size()<<endl;
}

void Depth_Saliency::computeSaliency()
{
	normalizeHist();
	double region_z, region_y, region_x;
	for(vector<vector<double> >::size_type u=0; u<hist.size(); ++u)
	{
		double sum=0.0;
		vector<double> histp = hist[u];
		double ratio_dims = (1.0*map_indices[u].size())/(1.0*original_points);
		region_z = region_x = region_y = 0;
		for(vector<vector<int> >::size_type w=0; w<map_indices[u].size(); ++w)
		{
			region_z += cloud_rgb_no_nans[map_indices[u][w]].z;
			region_y += cloud_rgb_no_nans[map_indices[u][w]].y;
			region_x += cloud_rgb_no_nans[map_indices[u][w]].x;
		}
		region_z = region_z/map_indices[u].size();
		region_x = region_x/map_indices[u].size();
		region_y = region_y/map_indices[u].size();

		for(vector<vector<double> >::size_type v=0; v<hist.size(); ++v)
		{
			if(u!=v)
			{
				vector<double> histq = hist[v];
				sum = sum + dotProduct(histp, histq);
			}
		}
		
		double comb_score = 2*sum*ratio_dims;
		double comb = comb_score*region_z;

	
//		cout<<"Sum is "<<sum<<" ratio dims is "<<ratio_dims<<" comb score is "<<comb_score<<" region z is "<<region_z<<" comb is "<<comb<<endl;
//		if(region_z>0)
//			cout<<"region_z score above 0 "<<endl;
//		if(comb>0)
//			cout<<"comb score above 0 "<<endl;
		saliency.push_back(comb);

	}
	cout<<"Computesaliency is done "<<endl;
	cout<<"segments in saliency is "<<saliency.size()<<endl;
}

void Depth_Saliency::mapTo2DImage(int image_no)
{ 
	depth_saliency_map = Mat::zeros(480, 640, CV_8UC1);
	int k=0; 
	double max_saliency = 0.0;
	for(int i=0;  i<saliency.size(); ++i)
	{
		if(max_saliency<saliency[i])
			max_saliency = saliency[i];
	}
	cout<<"max_saliency is "<<max_saliency<<endl;
	depth_saliency_value.resize(map_indices.size());	
	for(vector<vector<int> >::size_type u=0; u<map_indices.size(); ++u, ++k)
	{
		for(vector<int>::size_type v=0; v<map_indices[u].size(); ++v)
		{
			int b = (indices[map_indices[u][v]]+1)%640 - 1;
			int a = (indices[map_indices[u][v]]+1)/640 - 1;

			if((map_indices[u][v]+1)%640!=0)
				a++;

			depth_saliency_map.at<uchar>(a, b) = (uchar) 255*(1-saliency[k]/max_saliency);

		}
		depth_saliency_value[k] = (1-saliency[k]/max_saliency);
	//	cout<<(1-saliency[k]/max_saliency)<<endl;


	}
	string path1;
	stringstream ss;
	ss<<image_no;
	if(image_no<10)
		path1 = path_depth_saliency_map_dir+"/depth_map_0"+ss.str()+".png";
	else
		path1 = path_depth_saliency_map_dir+"/depth_map_"+ss.str()+".png";
	imwrite(path1, depth_saliency_map);

	writeDepthSaliency(path1);
	int maxS = 0;

	for(int i=0; i<depth_saliency_map.rows; ++i)
	{
		for(int j=0; j<depth_saliency_map.cols; ++j)
		{
			if(maxS<depth_saliency_map.at<uchar>(i, j))
				maxS = depth_saliency_map.at<uchar>(i, j);

		}
	}

	for(int i=0; i<depth_saliency_map.rows; i++)
		for(int j=0; j<depth_saliency_map.cols; ++j)
		{
			depth_saliency_map.at<uchar>(i, j) = depth_saliency_map.at<uchar>(i, j)*255/maxS;
		}

	cout<<"maxS is "<<maxS<<endl;

	cout<<"mapTo2DImage is done "<<endl;

//	string path2 = path_depth_saliency_map_dir+"/depth_map_2.png";
//	imwrite(path2, depth_saliency_map);

}

void Depth_Saliency::computeColorHist()
{ 


//	cout<<"The size of colorHist before computation is "<<colorHist.size()<<endl;
	for(vector<vector<int> >::size_type u=0; u<map_indices.size(); ++u)
	{
		vector<double> color;
		color.resize(3);
		for(size_t i =0; i<color.size(); ++i)
			color[i]=0;
		double R=0, G=0, B=0;
		for(vector<vector<int> >::size_type v=0; v<map_indices[u].size(); ++v)
		{

			R += cloud_rgb_no_nans[map_indices[u][v]].r;
			G += cloud_rgb_no_nans[map_indices[u][v]].g;
			B += cloud_rgb_no_nans[map_indices[u][v]].b;


		}
		color[0] = R/(255*map_indices[u].size());
		color[1] = G/(255*map_indices[u].size());
		color[2] = B/(255*map_indices[u].size());

		colorHist.push_back(color);
//		for(int i=0; i<color.size(); ++i)
//			cout<<color[i]<<" ";
//		cout<<endl;

	}
	cout<<"The size of colorHist is "<<colorHist.size()<<endl;
} 

void Depth_Saliency::computeVerticalityHist(int n_bins)
{ 
//	cout<<"The size of verticalHist before is "<<verticalityHist.size()<<endl;
	for(vector<vector<int> >::size_type u=0; u<map_indices.size(); ++u)
	{
		vector<double> vertical;
		vertical.resize(n_bins);
		for(size_t i=0; i<vertical.size(); ++i)
			vertical[i]=0;
		
		double no_normals = 0;
		for(vector<int>::size_type v=0; v<map_indices[u].size(); ++v)
		{

			Normal p, q;
			p.normal_x = cloud_normals[map_indices[u][v]].normal_x;
			p.normal_y = cloud_normals[map_indices[u][v]].normal_y;
			p.normal_z = cloud_normals[map_indices[u][v]].normal_z;

			q.normal_x = 0.0;
			q.normal_y = 0.0;
			q.normal_z = -1.0;
			
			double angle = abs(findAngle(p, q));
			double interval = M_PI/(6.0*n_bins);
			if(angle<=M_PI/6.0)
			{
				no_normals++;
				double index = angle/interval;

				if((floor(index)==index)&&(index!=0))
					vertical[(int)(index-1)]+=1;
				else
					vertical[(int)(index)]+=1;
			}
		}
		double max_points = 0;
		for(int i=0; i<n_bins; ++i)
			if(max_points<vertical[i])
				max_points = vertical[i];

		for(int i=0; i<n_bins; ++i)
		{
			vertical[i]=vertical[i]/max_points;
//			cout<<vertical[i]<<" ";
		}
//		cout<<endl;
		verticalityHist.push_back(vertical);
		
	}
	cout<<"The size of verticalHist is "<<verticalityHist.size()<<endl;
}

void Depth_Saliency::computeScaleSizeHist(int n_bins)
{
//	cout<<"The size of the scaleSize hist before is "<<scaleSizeHist.size()<<endl;
	for(vector<vector<int> >::size_type u=0; u<map_indices.size(); ++u)
	{
		vector<double> scale;
		scale.resize(n_bins);
		for(size_t i=0; i<scale.size(); ++i)
			scale[i]=0;
		double xmin=99, xmax=-99, xrange=0;
		double ymin=99, ymax=-99, yrange=0;
		double zmin=99, zmax=-99, zrange=0;

		for(vector<vector<int> >::size_type v=0; v<map_indices[u].size(); ++v)
		{
			PointXYZ p;
			p.x = cloud_rgb_no_nans[map_indices[u][v]].x;
			p.y = cloud_rgb_no_nans[map_indices[u][v]].y;
			p.z = cloud_rgb_no_nans[map_indices[u][v]].z;

			if(xmin>p.x)
				xmin=p.x;
			if(xmax<p.x)
				xmax=p.x;
			if(ymin>p.y)
				ymin=p.y;
			if(ymax<p.y)
				ymax=p.y;
			if(zmin>p.z)
				zmin=p.z;
			if(zmax<p.z)
				zmax=p.z;

			xrange = xmax - xmin;
			yrange = ymax - ymin;
			zrange = zmax - zmin;

			scale[0]=xrange/10; scale[1]=(xmin+5)/10; scale[2]=(xmax+5)/10;
			scale[3]=yrange/10; scale[4]=(ymin+5)/10; scale[5]=(ymax+5)/10;
			scale[6]=zrange/5; scale[7]=(zmin)/5; scale[8]=(zmax)/5;
		}
		scaleSizeHist.push_back(scale);
//		for(int i=0; i<scale.size(); ++i)
//			cout<<scale[i]<<" ";
//		cout<<endl;
	}
	cout<<"The size of the scaleSize hist after is "<<scaleSizeHist.size()<<endl;

}

void Depth_Saliency::computeCompactnessHist()
{

//	cout<<"The size of the compactnessHist before is "<<compactnessHist.size()<<endl;
	for(vector<vector<int> >::size_type u=0; u<map_indices.size(); ++u)
	{
		vector<double> compact;
		vector<double> minmidmax;

		compact.resize(2);
		minmidmax.resize(3);
		
		for(int i=0, j=0; i<scaleSizeHist[u].size(); i+=3, j++)
			minmidmax[j] = scaleSizeHist[u][i];
		
		sort(minmidmax.begin(), minmidmax.end());
	//	for(int i=0; i<minmidmax.size(); i++)
	//		cout<<minmidmax[i]<<" ";
	//	cout<<endl;

		compact[0] = minmidmax[0]/minmidmax[2];
		compact[1] = minmidmax[1]/minmidmax[2];

		compactnessHist.push_back(compact);
//		for(int i=0; i<compact.size(); i++)
//			cout<<compact[i]<<" ";
//		cout<<endl;

	}
	cout<<"The size of the compactnessHist before is "<<compactnessHist.size()<<endl;
}

void Depth_Saliency::writeFeatures(string path_out_feature_file)
{
	int no_features=map_indices.size(), feature_length=colorHist[0].size()+verticalityHist[0].size()+scaleSizeHist[0].size()+compactnessHist[0].size();
	cout<<"No of features = "<<no_features<<" and feature length is "<<feature_length;

	vector<Mat> feature_map;
//	cout<<"the size of feature_map before is "<<feature_map.size()<<endl;

	for(int i=0; i<feature_length; ++i)
	{
		Mat feature_matrix = Mat::zeros(480, 640, CV_64F);
		feature_map.push_back(feature_matrix);

	}
	cout<<"the size of feature_map is "<<feature_map.size()<<endl;
	int k=0;



	for(vector<vector<int> >::size_type u=0; u<map_indices.size(); ++u, ++k)
	{

		for(vector<int>::size_type v=0; v<map_indices[u].size(); ++v)
		{
			int index=0;
//			cout<<" u is "<<u<<endl;
			int b= (indices[map_indices[u][v]]+1)%640 - 1;
			int a= (indices[map_indices[u][v]]+1)/640 - 1;
			if((map_indices[u][v]+1)%640!=0)
				a++;
			
			//Depth_Saliency filling
	//		cout<<"index = "<<index<<" k = "<<k<<endl;
	//		feature_map[index++].at<double>(a, b) = depth_saliency_value[k];
			
	//		cout<<"index = "<<index<<" k = "<<k<<endl;
			for(int i=0; i<colorHist[u].size(); ++i)
				feature_map[index++].at<double>(a, b) = colorHist[u][i];
	//		cout<<"index = "<<index<<" k = "<<k<<endl;
			for(int i=0; i<verticalityHist[u].size(); ++i)
				feature_map[index++].at<double>(a, b) = verticalityHist[u][i];
	//		cout<<"index = "<<index<<" k = "<<k<<endl;
			for(int i=0; i<scaleSizeHist[u].size(); ++i)
				feature_map[index++].at<double>(a, b) = scaleSizeHist[u][i];
	//		cout<<"index = "<<index<<" k = "<<k<<endl;
			for(int i=0; i<compactnessHist[u].size(); ++i)
				feature_map[index++].at<double>(a, b) = compactnessHist[u][i];
	//		cout<<"index = "<<index<<" k = "<<k<<endl;

		}
	}
	

	ofstream fileout;
	fileout.open(path_out_feature_file.c_str());
	fileout<<feature_length<<endl;
//	cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$$"<<feature_length-2<<endl;
	for(int i=0; i<feature_map[0].rows; ++i)
		for(int j=0; j<feature_map[0].cols; ++j)
		{
			for(int z=0; z<feature_length; z++)
			{
				fileout<<feature_map[z].at<double>(i, j)<<" ";
			}
			fileout<<endl;
		}
	cout<<"Written into the file"<<endl;
}

void Depth_Saliency::writeDepthSaliency(string image_path)
{
	ofstream image_file;
	string temp;
	temp = image_path;
	temp.erase(temp.find_last_of("."), string::npos);
	temp = temp+".txt";
	image_file.open(temp.c_str());

	Mat image = imread(image_path.c_str());
	for(int i=0; i<image.rows; i++)
	{
		for(int j=0; j<image.cols; j++)
		{
			image_file<<(double)image.at<uchar>(i, j)<<endl;
		}
	}
	cout<<temp<<" $$$$$$$$$$$$$$$ Done with writing "<<endl;	
}
double dotProduct(vector<double> p, vector<double> q)
{
	double temp = 0.0;
	for(vector<double>::size_type i=0; i<p.size(); i++)
		temp = temp + p[i]*q[i];
	return temp;
}

double findAngle(Normal p, Normal q)
{
	double num = (p.normal_x*q.normal_x) + (p.normal_y*q.normal_y) + (p.normal_z*q.normal_z);
	if(abs(num)<=1)
		return (abs(acos(num)/M_PI));
	else
		return (abs(acos(1)/M_PI));
}
/*
int main(int ac, char* av[])
{
	if(ac<4)
	{
		cout<<"Please give the input cloud and input image and output dir"<<endl;
		return 0;
	}
	cout<<"ac is "<<ac<<endl;
	cout<<"av 0 is "<<av[1]<<" "<<av[2]<<" "<<av[3]<<endl;

	ifstream file_image, file_pcd;
	file_image.open(av[1]);
	file_pcd.open(av[2]);
	string path = av[3];

	int i=1;
	while(!file_image.eof()&&!file_pcd.eof())
	{
		cout<<"IMAGE NUM = "<<i<<"  #############################################################################"<<endl;
		string img, pcd;
		file_image>>img;
		file_pcd>>pcd;
		Depth_Saliency DS(img, pcd, path);
		DS.mergeCloudImage(); cout<<"Done with merging"<<endl;
		DS.removeNaNs(); cout<<"Done with removal of nans"<<endl;
		DS.estimateNormals(); cout<<"Done with normals estimation"<<endl;
		DS.regionGrowing(); cout<<"Done with region growing"<<endl;
		DS.computeHistogram(100); cout<<"Done with histogram computation"<<endl;
		DS.computeSaliency(); cout<<"Done with computeSaliency"<<endl;
		DS.mapTo2DImage(i); cout<<"Done with depth map creation"<<endl;
		DS.computeColorHist(); cout<<"Done with colorHist creation"<<endl;
		DS.computeVerticalityHist(20); cout<<"Done with verticality creation"<<endl;
		DS.computeScaleSizeHist(9); cout<<"Done with scaleSize creation"<<endl;
		DS.computeCompactnessHist(); cout<<"Done with compact creation"<<endl;

		stringstream ss;
		ss<<i;
		string path_features;
		if(i<10)
			path_features = path+"/depth_map_0"+ss.str()+".txt";
		else
			path_features = path+"/depth_map_"+ss.str()+".txt";

		DS.writeFeatures(path_features);
		i++;
	}
	cout<<"Done"<<endl;
	return 0;
}
*/
