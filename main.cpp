#include "canread.h"
#include "command analysis.h"
#include<iostream>
#include<string>
#include<fstream>
#include<stdexcept>
using namespace std;
inline string find_name(const string&,string&);
bool trans_file(istream&file,ostream& out);
int main(){
	CANREAD canread;
	string ifname,ifnamenoex,ifex;
	ifstream input;
	CMD_Analys cmd(cin);
	cmd.insert_cmd("open");
	cmd.insert_cmd("trans");
	cmd.insert_cmd("analyze");
	cmd.insert_cmd("help");
	cmd.insert_cmd("close");
	ofstream output,output1,errout;
	do{
		string ofname;
		cout<<" [ ";
		if(!ifname.size()){
			cout<<"!#*";
		}else
			cout<<ifname;
		cout<<"]# ";
		switch(cmd.get_cmd()){
		case 0:
			ifname = cmd.get_argument(1);
			if(!ifname.size()){
				cout<<"Open 后必须跟文件名！"<<endl;
				break;
			}else{
				cout<<"打开文件中..."<<endl;
				if(!!input){
					input.clear();
					input.close();
				}
				input.open(ifname.c_str());
				if(!input){
					cout<<"无法打开文件:\t"<<ifname<<endl;
					ifname.clear();
					break;
				}
				ifnamenoex = find_name(ifname, ifex);
				cout<<"成功打开输入文件文件:\t"<<ifname<<endl;
			}
			break;
		case 1:
			if(!input){
				cout<<"未打开任何文件！"<<endl;
				break;
			}
			ofname = ifnamenoex+"_trans."+ifex ;
			output.open(ofname.c_str());
			if(!output){
				cout<<"打开文件"<<ofname<<"失败!"<<endl;
				break;
			}
			if(trans_file(input,output)){
				cout<<"转换成功..."<<endl<<"结果已保存至"<<ofname<<endl;
			}else{
				cout<<"文件"<<ifname<<"转换失败,详情请见"<<ofname<<endl;
			}
			output.close();
			break;
		case 2:
			if(!input){
				cout<<"未打开任何文件！"<<endl;
				break;
			}

			cout<<"解析中..."<<endl;
			ofname = (ifnamenoex+"内容."+ifex);
			output.open(ofname.c_str());
			cout<<"报文内容将输出到文件"<<ofname<<"中--"<<endl;
			ofname = (ifnamenoex+"结果."+ifex);
			output1.open(ofname.c_str());
			cout<<"报文分析结果将输出到文件"<<ofname<<"中--"<<endl;
			ofname =(ifnamenoex+"错误."+ifex);
			errout.open(ofname .c_str());
			cout<<"报文分析中发生的错误将输出到文件"<<ofname<<"中--"<<endl;
			try{
				canread.read_data(input,errout);
				canread.print(output,output1,errout);
				input.close();
				output.close();
			}catch(runtime_error& ex){
				std::cerr<<ex.what()<<endl;
			}
			output.close();
			output1.close();
			errout.close();
			cout<<"报文分析完成!!!"<<endl;
			break;
		case 3:
			cout<<"\t open 文件名:"<<"打开输入文件名,将以文件名作为解析的文件.\n"<<endl;
			cout<<"\t trans :"<<"将open命令打开的CANtest抓取的报文转换成标准输入文件,并另存为文件名_tran.后缀名.\n"<<endl;
			cout<<"\t analyze :"<<"对open命令已打开的输入文件进行解析并将输出文件保存.\n"<<endl;
			cout<<"\t close"<<"关闭目前打开的文件\n"<<endl;
			cout<<"\t help :"<<"帮助.\n"<<endl;
			cout<<cmd.help();
			break;
		case 4:
			cout<<"正在关闭 \t"<<ifname<<endl;
			input.clear();
			input.close();
			cout<<"关闭文件"<<ifname<<"成功"<<endl;
			break;
		case -1:
			cout<<"无法识别命令!!!输入help查看帮助。"<<endl;
			break;
		default:
			cout<<"程序错误，需修改！！！"<<endl;
			break;
		}
	}while(1);
	return 0;
}
inline string find_name(const string&s,string& name){
	string::size_type index = s.find_last_of('.');
		name=string(s.begin()+index + 1,s.end());
	return string(s.begin(),s.begin()+index);
}
bool trans_file(istream&file,ostream& out){
	string s1,s2;
	while(getline(file,s1)){
		int j = 0;
		s2.resize(s1.size());
		for(unsigned int i = 0; i < s1.length(); i++){
			while(i<s1.length()&&(s1[i]==' '||s1[i]=='\t')) i++;
			while(i<s1.length()&&(s1[i]!=' '&&s1[i]!='\t'))s2[j++]=s1[i++];
			s2[j++]='\t';
		}
		s2.resize(j - 1 );
		out<<s2<<endl;
	}
	return true;
}
