/*
 * FileManager.cpp
 *
 *  Created on: 2018Äê1ÔÂ4ÈÕ
 *      Author: Administrator
 */
#include<direct.h>
#include"FileManager.h"
#include<windows.h>
string operinfo;
string fileManager::getPWD(){
	char cPATH[1000];
	GetCurrentDirectory(1000,cPATH);
	curPATH.assign(cPATH);
	return "true";
}
void fileManager::fixDirName(string& s){
	for(string::iterator it = s.begin() ; it != s.end() ; ++it ){
		if(*it == '/'){
			*it = '\\';
		}
		if(*it == '\\'){
			if(*(it+1)=='/'){
				*(it+1)='\\';
			}
			if(*(it+1)!='\\'){
				s.insert(it,'\\');
			}
		}
	}
}
fileManager::fileManager(){

}
bool fileManager::open(string&name,ios_base::openmode opmode,string& info){
	fstream of;
	of.open(name,opmode);
	return true;
}
bool fileManager::cd(string&,string& info){
 return true;
}
string fileManager::mkdir(string&, string&info){
	return "pwd";
}
string fileManager::ls()const{
	return "pwd";
}
string pwd(){
 return "pwd";
}

