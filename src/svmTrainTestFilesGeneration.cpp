/* This code is to generate the train.txt and test.txt for the set of images whose svm format files are available at svm_files folder
 * This will take in those images numbers specified by the use into and create train.txt and consider rest of the images to be test.txt
 * This will take only the uniq samples in the train and test so as to avoid time waste while training and testing
 */
/* This can be written better with respect to the time and efficiency but not of high priority */
//Cpp
#include <iostream>
#include <fstream>
#include <dir_files/dir_files.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

time_t start, temp, end;

int main(int ac, char* av[])
{
	//Enter the folder that contains the svm files of the images and file with image number in the training
	if(ac<3)
	{
		cout<<"Enter the folder that contains the svm files of the images"<<endl;
		return 0;
	}

	string dir_svm_files = av[1];
	string req_train_images_list = av[2];

	ifstream file_list;
	file_list.open(req_train_images_list.c_str());
	vector<string> req_list; //Write all the req file no into a list of strings

	
	int no_train_images;
	file_list>>no_train_images;
	for(int i=0; i<no_train_images; i++)
	{
		string file_no;
		file_list>>file_no;
		
		req_list.push_back(file_no);
	}
	
	cout<<"The size of the req list is "<<req_list.size()<<endl;
	system("rm -rf svm_temp");
	system("mkdir svm_temp");
	
	//List out all the svm files available in the folder
	vector<string> svm_files_list;
	dir_files D_svm(dir_svm_files.c_str(), svm_files_list, "txt");

	cout<<"The no of svm_files are "<<svm_files_list.size()<<endl;

	for(int i=0; i<svm_files_list.size(); i++)
	{
		string temp = svm_files_list[i];
		bool flag = false;
		for(int j=0; j<req_list.size(); j++)
		{
			unsigned pos = temp.find(req_list[j]);
			if(pos!=string::npos)
			{
				flag = true;
			}
		}
		if(flag)//If available in the req list then this goes into the train.txt
		{
			string ss = dir_svm_files+"/"+svm_files_list[i];
			cout<<"Train "<<ss<<endl;
			string command = "cat "+ss+" >> ./svm_temp/train.txt";
			system(command.c_str());


		}
		else //If not available in the req list then this goes into the test.txt
		{
			string ss = dir_svm_files+"/"+svm_files_list[i];
			cout<<"Test "<<ss<<endl;
			string command = "cat "+ss+" >> ./svm_temp/test.txt";
			system(command.c_str());
		}
	}
	time (&start);
	system("wc -l ./svm_temp/train.txt");
	time (&temp);
	cout<<" ["<<difftime(temp, start)<<"]"<<endl;
	system("wc -l ./svm_temp/test.txt");
	time (&end);
	cout<<" ["<<difftime(end, temp)<<"]"<<endl;
	temp = end;
	//Now uniq the files using system command
	system("sort ./svm_temp/train.txt | uniq >> ./svm_temp/uniq_train.txt");
	time (&end);
	cout<<" ["<<difftime(end, temp)<<"]"<<endl;
	temp = end;
	system("sort ./svm_temp/test.txt | uniq >> ./svm_temp/uniq_test.txt");
	time (&end);
	cout<<" ["<<difftime(end, temp)<<"]"<<endl;
	temp = end;
	system("wc -l ./svm_temp/uniq_train.txt");
	time (&end);
	cout<<" ["<<difftime(end, temp)<<"]"<<endl;
	temp = end;
	system("wc -l ./svm_temp/uniq_test.txt");
	time (&end);
	cout<<" ["<<difftime(end, temp)<<"]"<<endl;
	temp = end;

	//This uniq is limited to avoid the repetitions in the file; To have a uniq sample list extra code has to be written

	return 0;
}
