#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

int main(int ac, char* av[])
{
	if(ac<3)
	{
		cout<<"Give the file list of sampled files for svm training and path for the output file"<<endl;
		return 0;
	}
	ifstream file_list;
	file_list.open(av[1]);

	string path_out = av[2];
	
	int k=1;
	while(!file_list.eof())
	{
		string path_file;
		file_list>>path_file;

		string path_out_file1, path_out_file2;
		stringstream ss;
		ss<<k;
		if(k<10)
		{
			path_out_file1 = path_out+"/image_file_0"+ss.str()+".txt";
			path_out_file2 = path_out+"/depth_file_0"+ss.str()+".txt";
		}
		else
		{
			path_out_file1 = path_out+"/image_file_"+ss.str()+".txt";
			path_out_file2 = path_out+"/depth_file_"+ss.str()+".txt";
		}
		cout<<"IMAGE NO "<<k<<" ################################################### IMAGE NO "<<endl;
		ifstream file_in;
		file_in.open(path_file.c_str());
		ofstream file_out1, file_out2;
		
		file_out1.open(path_out_file1.c_str());
		file_out2.open(path_out_file2.c_str());

		for(int i=0; i<5000; i++)
		{
			for(int j=0; j<37; j++)
			{
				string val;
				file_in>>val;
				if(j==1)
					file_out1<<val<<endl;
				if(j==2)
					file_out2<<val<<endl;
			}
		}
		k++;
	}
}
