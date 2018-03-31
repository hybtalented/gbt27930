/*
 * FileManager.h
 *
 *  Created on: 2018Äê1ÔÂ4ÈÕ
 *      Author: Administrator
 */

#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_
#include<io.h>
#include<string>
#include<fstream>
#include<map>
using namespace std;
extern string operinfo;
class fileManager{
public:
	fileManager();
	~fileManager(){}
	bool open(string&,ios_base::openmode opmode,string& info=operinfo);
	bool cd(string&,string& info=operinfo);
	string mkdir(string&, string&info=operinfo);
	string pwd();
	string ls()const;
private:
	string getPWD();
	void fixDirName(string&);
	map<string,fstream> openfiles;
	string curPATH;
	map<long,struct _finddata_t> fileinfos;
};




#endif /* FILEMANAGER_H_ */
