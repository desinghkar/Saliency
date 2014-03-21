#ifndef DIR_FILES_H
#define DIR_FILES_H

#include <iostream>
#include <dirent.h>
#include <opencv2/core/core.hpp>
using namespace std;

class dir_files{

	struct dirent **namelist;
	int no_files;
	int no_files_extension;

	public:

	dir_files() { };
	~dir_files() { };

	dir_files(string, vector<string>&, string);

	//List the files onto a vector of string of a specific type of file extension (path, vector, ext)
	void listFilesInFolder(string, vector<string>&, string);

	//get the no of files in the folder
	int getFilesCount();

	//get the no of files of specific ext
	int getFilesExtCount();
};

#endif

