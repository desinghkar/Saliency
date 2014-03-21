#include <fstream>
#include <limits>
#include <iostream>
#include <stdlib.h>
#include <sstream>
using namespace std;
std::fstream& GotoLine(std::fstream& file, unsigned int num){
	file.seekg(std::ios::beg);
	for(int i=0; i < num - 1; ++i){
		file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
	}
	return file;
}
int main(){
	using namespace std;
	fstream file("/backup/karthik_data/rgbd_with_gt_features/complete_features_uniq.txt");
	
	ofstream file_out;

	int i=0, k=0;

	cout<<"File number "<<k<<" is written "<<endl;
	while(i<780433)
	{
		if(i%5000==0)
		{
			file_out.close();
			k++;
			cout<<"File number "<<k<<" is written "<<endl;
			stringstream ss;
			ss<<k;
			string path_file;
			if(k<10)
				path_file = "/backup/karthik_data/sampling_5000/0"+ss.str()+".txt";
			else
				path_file = "/backup/karthik_data/sampling_5000/"+ss.str()+".txt";

			file_out.open(path_file.c_str());
		}
		double val;
		int v1 = rand()%780433 + 1;
		GotoLine(file, v1);
		
		file>>val;
		if(val==0)
			file_out<<"-1 ";
		else
			file_out<<"+1 ";

		for(int i=0; i<35; ++i)
		{
			file>>val;
			file_out<<i+1<<":"<<val<<" ";
		}
		file>>val;
		file_out<<"36:"<<val<<endl;



//		cout<<"i="<<i<<" k="<<k<<endl;
		i++;
	}

	return 0;
}
