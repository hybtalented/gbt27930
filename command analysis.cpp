/*
 * command analysis.cpp
 *
 *  Created on: 2018年1月3日
 *      Author: Administrator
 */

#include"command analysis.h"
#include<cctype>
#include <algorithm>
#include<sstream>
int CMD_Analys::get_cmd(){
	string s;
	getline(in,s);
	transform(s.begin(),s.end(),s.begin(),::tolower);
	line.setBuf(s);
	if(find_in_strings(string(line.getString(0)), def_cmd)!=-1){
		return -1;
	}
	return find_in_strings(string(line.getString(0)),cmd);
}
void CMD_Analys::insert_cmd(const string&s){
	string s1;
	s1.resize(s.size());
	transform(s.begin(),s.end(),s1.begin(),::tolower);
	cmd.push_back(s1);
}
string CMD_Analys::help(){
	ostringstream s;
	s<<"CMD_Analys类自带的命令帮助"<<endl;
	return s.str();
}
int find_in_strings(const string&cmd,vector<string>& cmds){
	for(int i = 0; i < cmds.size(); ++i){
		if(cmd == cmds[i]){
			return i;
		}
	}
 return -1;
}

