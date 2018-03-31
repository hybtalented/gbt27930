/*
 * canread.cpp
 *
 *  Created on: 2017年12月5日
 *      Author: Administrator
 */
#include"canread.h"
#include"table field.h"
VOID CANREAD::read_data(istream& in,ostream&errout,bool igfl){
	string line;
	if(igfl){getline(in,line) ;}
	while(getline(in,line)){
		Table_field tf(line,ifs);
		DWORD canid = tf.getDWordHex(CANID_BASE);
		Basic_CAN_frame* newcan = Basic_CAN_frame::create_frame(canid);
		if(!newcan){
			errout<<"无法识别帧:"<<endl;
			for(UINT i = 0 ; i< tf.getNumField(); i++){
				errout<<tf.getString(i)<<'\t';
			}
			errout<<endl;
		}else{
			newcan->get_data(tf);
			if(!autoindex){
				newcan->setIndex(tf.getDWord(indexcol));
			}
			canframes.push_back(newcan);
		}
	}
}
vector<Basic_CAN_frame*>::iterator CANREAD::findOldframe(Basic_CAN_frame* can){
	vector<Basic_CAN_frame*>::iterator it= oldframes.begin();
	for(; it != oldframes.end(); ++it){
		if((*it)->getCANID() == can->getCANID()){
			return it;
		}
	}
	return it;
}
vector<Multi_frame*>::iterator  CANREAD::findOldframe(Multi_frame* mul){
	vector<Multi_frame*>::iterator it= oldmulframes.begin();
	for(;it != oldmulframes.end(); ++it){
		if((*it)->getPGN() == mul->getPGN()){
			return it;
		}
	}
	return it;
}
static void direct(bool b,string&&tpname,ostream& out){
	const char  * charge ="充电桩",*bms="BMS";
	out<<charge;
	if(b){
		for(UINT i = 0 ; i< 20 ; i++){
			out<<">>";
		}
		out<<"\t"<<tpname<<"\t";
		for(UINT i = 0 ; i< 20 ; i++){
			out<<">>";
		}
	}else{
		for(UINT i = 0 ; i< 20 ; i++){
			out<<"<<";
		}
		out<<"\t"<<tpname<<"\t";
		for(UINT i = 0 ; i< 20 ; i++){
			out<<"<<";
		}
	}
	out<<bms;
}
static inline void theindex(UINT index,ostream& out){
	out<<"第"<<index<<"帧:";
}
VOID CANREAD::print(ostream& out,ostream&out1 ,ostream&errout){
	out<<"帧编号"<<"阶段"<<ofs<<"帧id"<<ofs<<"帧名"<<ofs<<"内容"<<endl;
	for( UINT i = 0 ; i < canframes.size(); i++){
		DWORD canid = canframes[i]->getCANID();
		UINT index ;
		if(autoindex)
			index = i;
		else
			index=canframes[i]->getIndex();
		string detail = canframes[i]->get_detail() ;
		out<<index<<ofs<<canframes[i]->getState()<<ofs<<canframes[i]->typeName()<<ofs<<canframes[i]->get_frame_name()<<ofs<<detail<<endl;
		if(canid==TP_START_CANID||canid==TP_REPLY_CANID||canid==TP_DATA_CANID){//多帧传输
			bool flag2 = true;
			switch(canid){
			case TP_DATA_CANID:
				if(multiframe==NULL)break;
				multiframe->addFrame(canframes[i]);
				if(!multiframe->valid()){
					multiframe->delLastFrame();
				}
				break;
			case TP_REPLY_CANID:
				if(multiframe==NULL)break;
				if( canframes[i]->getPGN()==multiframe->getPGN()){
					multiframe->addFrame(canframes[i]);
					if(canframes[i]->getConsole() == TP_REPLY_CONSOLE ){
						if(!multiframe->valid())
						multiframe->delLastFrame();
					}else if(canframes[i]->getConsole() == TP_END_CONSOLE){
					if(multiframe->rec_finish()){
						vector<Multi_frame*>::iterator old = findOldframe(multiframe);
						bool flag = false;
						if(old == oldmulframes.end()){
							oldmulframes.push_back(multiframe);
							flag = true ;
							flag2 = false;
						}else if(!(*multiframe == **old) ){
							flag = true;
							**old = *multiframe ;
						}
						if(flag){
							vector<UINT> indexs=multiframe->getIndexs();
							direct(0,multiframe->typeName(),out1);out1<<endl;
							for(UINT j = 0 ; j < indexs.size();j++){
								theindex(indexs[j],out1);
							}
							out1<<endl<<multiframe->getState()<<"\t帧名:"<<multiframe->get_frame_name()<<"\n内容:\n"<<multiframe->get_detail()<<endl;
							out1<<"*****************************************************************************\n";
						}
					}else{
						vector<UINT> indexs=multiframe->getIndexs();
						errout<<"不合理的CAN帧：";
						direct(1,multiframe->typeName(),errout);errout<<endl;
						for(UINT j = 0 ; j < indexs.size();j++){
							theindex(indexs[j],errout);
						}
						errout<<endl<<multiframe->getState()<<",帧id"<<multiframe->typeName()<<",帧名:"<<multiframe->get_frame_name()<<"\n内容:\n"<<multiframe->get_detail()<<endl;
						errout<<"*****************************************************************************\n";
					}
					if(flag2){
						delete multiframe;
					}
					multiframe = NULL;
				}else{
					multiframe->delLastFrame();
					errout<<"未知多帧传输Reply帧，控制字为0x"<<hex<<canframes[i]->getConsole()<<":\t";
					vector<UINT> indexs=multiframe->getIndexs();
					direct(1,multiframe->typeName(),errout);errout<<endl;
					for(UINT j = 0 ; j < indexs.size();j++){
						theindex(indexs[j],errout);
					}
					errout<<endl<<multiframe->getState()<<"\t帧名:"<<multiframe->get_frame_name()<<"\n内容:\n"<<multiframe->get_detail()<<endl;
					errout<<"*****************************************************************************\n";
					delete multiframe;
					multiframe = NULL ;
				}
				}
				break;
			case TP_START_CANID:
				if(multiframe){
					delete multiframe;
				}
				if(canframes[i]->getConsole()!=TP_START_CONSOLE){
					multiframe =NULL;
				}else switch(canframes[i]->getPGN()){
				case BRM_PGN:
					multiframe = new CAN_BRM(dynamic_cast<CAN_TP_START*>(canframes[i]));
					break;
				case BCP_PGN:
					multiframe =new CAN_BCP(dynamic_cast<CAN_TP_START*>(canframes[i]));
					break;
				case BCS_PGN:
					multiframe = new CAN_BCS(dynamic_cast<CAN_TP_START*>(canframes[i]));
					break;
				default:
					multiframe = NULL;
					break;
				}
				break;
			}
		}else{
			vector<Basic_CAN_frame*>::iterator old =findOldframe(canframes[i]);
			bool flag = false;
			if(old == oldframes.end()){
				oldframes.push_back(canframes[i]);
				flag=true;
			}else if(!((*canframes[i])==**old)){
				*old = canframes[i];
				flag=true;
			}
			if(flag){
				direct(canframes[i]->direct(),canframes[i]->typeName(),out1);out1<<endl;
				theindex(index,out1);out1<<endl;
				out1<<"阶段:"<<canframes[i]->getState()<<"\t帧名:"<<canframes[i]->get_frame_name()<<"\n内容:"<<endl<<detail<<endl;
				out1<<"*****************************************************************************\n";
			}
		}
	}
}



