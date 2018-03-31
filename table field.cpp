/*
 * table field.cpp
 *
 *  Created on: 2017Äê11ÔÂ29ÈÕ
 *      Author: Administrator
 */
#include"table field.h"
#include<sstream>
using namespace std;
VOID Table_field::initFields(){
	fields.clear();
	for(UINT i=0,size=field.size() ; i < size; ++i ){
			while( i <size && field[i] == ifs && field[ i ] !='\0'  ) ++i ;
			fields.push_back(i);
			while(i <size &&  field[i]!=ifs && field[ i ] !='\0')  ++i ;
			if( i< size)
				field[ i ] ='\0';
	}
}
VOID Table_field::initFieldsStandard(){
	fields.clear();
	for(UINT i=0,size=field.size() ; i < size; ++i ){
			fields.push_back(i);
			while(i< size && field[ i ] !='\0'&& field[i]!=ifs )  ++i ;
			if( i< size )
				field[ i ] = '\0';
	}
}
Table_field::Table_field(istream& in , BYTE fs , BOOL st ):ifs(fs),stand(st){
	getline(in, field);
	if(stand){
			initFieldsStandard();
	}else{
			initFields();
	}
}
Table_field::Table_field(string&& s,BYTE fs, BOOL st ):field(s),ifs(fs),stand(st){
	if(stand){
		initFieldsStandard();
	}else{
		initFields();
	}
}
Table_field::Table_field(string& s,BYTE fs, BOOL st):field(s),ifs(fs),stand(st){
	if(stand){
		initFieldsStandard();
	}else{
		initFields();
	}
}
WORD Table_field::getWordHex(UINT i){
	DWORD ret;
	if(i >= fields.size())return 0;
	istringstream ssin( field.c_str() + fields[i] );
	ssin >> hex >> ret;
	return ret;
}
DWORD Table_field::getDWordHex(UINT i){
	DWORD ret;
	if(i >= fields.size())return 0;
	istringstream ssin(field.c_str() + fields[i] );
	ssin >> hex >> ret;
	return ret;
}
WORD Table_field::getWord(UINT i){
	DWORD ret;
	if(i >= fields.size())return 0;
	const string s=field.c_str() + fields[i] ;
	istringstream ssin(field.c_str() + fields[i]);
	ssin >> ret;
	return ret;
}
DWORD Table_field::getDWord(UINT i){
	DWORD ret;
	if(i >= fields.size())return 0;
	istringstream ssin(field.c_str() + fields[i] );
	ssin >> ret;
	return ret;
}
BYTE Table_field::getByteHex(UINT i){
	DWORD ret;
	if(i >= fields.size())return 0;
	istringstream ssin(field.c_str() + fields[i] );
	ssin >> hex >> ret;
	return ret;
}
BYTE Table_field::getByte(UINT i){
	DWORD ret;
	if(i >= fields.size())return 0;
	istringstream ssin(field.c_str() + i );
	ssin >> ret;
	return ret;
}
