/*
 * table field.h
 *
 *  Created on: 2017Äê11ÔÂ29ÈÕ
 *      Author: Administrator
 */

#ifndef TABLE_FIELD_H_
#define TABLE_FIELD_H_
#include<string>
#include<vector>
#include<istream>
#include"datatype.h"
using namespace std;

class Table_field{
	string field;
	BYTE ifs;
	vector<UINT> fields;
	BOOL stand;
public:
	VOID initFields();
	VOID initFieldsStandard();
	UINT32 getNumField(){return fields.size();}
	Table_field(istream&, BYTE fs='\t', BOOL s = true);
	Table_field(BYTE fs='\t',BOOL s = true):ifs(fs),fields(),stand(s){}
	Table_field(string&&,BYTE fs = '\t', BOOL s = true);
	Table_field(string&, BYTE fs='\t', BOOL s = true);
	VOID setBuf(string&buf){
		field = buf ;
		if(stand){
			initFieldsStandard();
		}else{
			initFields();
		}
	}
	VOID setIDivide(BYTE id){ifs = id;}
	WORD getWordHex(UINT i);
	DWORD getDWordHex(UINT i);
	WORD getWord(UINT i);
	DWORD getDWord(UINT i);
	BYTE getByteHex(UINT i);
	BYTE getByte(UINT i);
	CONST_CHARPTR getString(UINT i){ if( i >=fields.size()) return NULL; return field.c_str()+fields[i];}
};




#endif /* TABLE_FIELD_H_ */
