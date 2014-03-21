/* This code is to train the svm on train.txt and test on test.txt after they have been unified earlier
 * This requires values of C for which the regression has to be calculated to display the min square errors
 */
//Cpp
#include <iostream>
#include <fstream>
#include <dir_files/dir_files.h>
#include <stdlib.h>

using namespace std;
ofstream file_out;

void train_svm(string svm_exe, string path_out, double C, string train_file, string test_file)
{
	stringstream sc;
	sc<<C;
	string command = svm_exe+"/svm-train -s 3 -p 0.1 -t 2 -c "+sc.str()+" "+train_file+" > new_train.txt";
	system(command.c_str());


	command = "mv uniq_train.txt.model ./temp_model_files/uniq_train_file_"+sc.str()+".model"; 
	system(command.c_str());
	command = svm_exe+"/svm-predict "+train_file+" ./temp_model_files/uniq_train_file_"+sc.str()+".model output_"+sc.str()+".txt > new_test1.txt";
	system(command.c_str());

	ifstream file_in1;
	file_in1.open("./new_test1.txt");
	bool flag=true;
	while(!file_in1.eof()&&flag)
	{
		string temp;
		file_in1>>temp;
		if(temp=="=")
		{
			double mse;
			file_in1>>mse;
			cout<<"C = "<<C<<" train_mse = "<<mse;
			flag= false;
		}
	}
	command = svm_exe+"/svm-predict "+test_file+" ./temp_model_files/uniq_train_file_"+sc.str()+".model output_"+sc.str()+".txt > new_test.txt";
	system(command.c_str());

	ifstream file_in2;
	file_in2.open("./new_test.txt");

	while(!file_in2.eof())
	{
		string temp;
		file_in2>>temp;
		if(temp=="=")
		{
			double mse;
			file_in2>>mse;
			cout<<" test_mse = "<<mse<<endl;
			return;
		}
	}
}

int main (int ac, char* av[])
{
	//Enter the path of the svm executable and C value 
	if(ac<3)
	{
		cout<<"Enter the path of the svm binary files and C values in a file"<<endl;
		return 0;
	}
	string dir_svm_bin = av[1];
	string temp = av[2];
	//Assuming that the number of C values will be the first element of the file
	ifstream C_list;
	C_list.open(temp.c_str());
	int no_C;
	C_list>>no_C;


//	cout<<"Given no of  Cs is "<<no_C<<endl;
	system("rm -rf temp_model_files"); system("rm -rf temp_output_files");
	system("mkdir temp_model_files"); system("mkdir temp_output_files");
	for(int i=0; i<no_C; ++i)
	{
		double C;
		C_list>>C;
		train_svm(dir_svm_bin, "./", C, "./svm_temp/uniq_train.txt", "./svm_temp/uniq_test.txt");
	}
	return 0;
}

