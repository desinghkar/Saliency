#include <dir_files/dir_files.h>


using namespace std;

void dir_files::listFilesInFolder(string folder_path, vector<string>& FileList, string ext)
{
	struct dirent **namelist;
	no_files = scandir(folder_path.c_str(), &namelist, 0, alphasort);

	no_files_extension = 0;
	int i=2;
	while(i<no_files)
	{
		string file;
		file = namelist[i]->d_name;
		if(file.substr(file.find_last_of(".")+1) == ext)
		{
			FileList.push_back(file);
			no_files_extension++;
		}
		i++;
	}
	no_files = no_files-2;
	if(FileList.size()==0)
		cout<<"No files of that extension is available in the folder"<<endl;

}

dir_files::dir_files(string folder_path, vector<string>& FileList, string ext)
{
	listFilesInFolder(folder_path, FileList, ext);
	
}

int dir_files::getFilesCount()
{
	return no_files;
}

int dir_files::getFilesExtCount()
{
	return no_files_extension;
}
/*
int main(int ac, char* av[])
{
	vector<string> S;
	dir_files D("/workspace/karthik/RRC/PCL_workspace/Dataset_objects/for_eyetracker/original", S, "png");
	cout<<"Size of S is "<<S.size()<<endl;
	cout<<D.getFilesCount()<<endl;
	cout<<D.getFilesExtCount()<<endl;

	return 0;
}
*/
