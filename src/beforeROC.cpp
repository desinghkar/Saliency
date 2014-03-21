#include <iostream>
#include <fstream>
#include <algorithm>
#include <opencv2/core/core.hpp>
using namespace std;

int main(int ac, char * av[])
{
	if(ac<5)
	{
		cout<<"Enter the path to the input file, ground truth and expected output and estimated file"<<endl;
		return 0;
	}
	ifstream file_in, exp_in;
	file_in.open(av[1]);
	exp_in.open(av[2]);
	ofstream file_out, gt_out;
	gt_out.open(av[3]);
	file_out.open(av[4]);


	vector<double> values, temp, gt;
	temp.resize(5000);
	values.resize(5000);
	gt.resize(5000);
	for(int i=0; i<5000; i++)
	{
		file_in>>values[i];
		temp[i]=values[i];
		exp_in>>gt[i];
		for(int j=0; j<36; j++)
		{
			string temp;
			exp_in>>temp;
		}
	}

	sort(temp.begin(), temp.end());
	double min = temp[0];
	double max = temp[4999];

	for(int i=0; i<5000; i++)
	{
		values[i]=values[i]-min;
		values[i]=255*values[i]/(max-min);
		file_out<<values[i]<<endl;
		if(gt[i]==-1)
			gt_out<<0<<endl;
		else
			gt_out<<1<<endl;
	}


	return 0;
}
