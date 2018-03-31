/*
 * CANdata.cpp
 *
 *  Created on: 2017年11月29日
 *      Author: Administrator
 */
#include"CANdata.h"
#include<sstream>
#include<iomanip>
using namespace std;
UINT DATA_BASE=7, CANID_BASE=3,LENGTH_BASE=6;
CONST_CHARPTR Basic_CAN_frame::states[7]={"充电握手阶段","充电辨识阶段","充电配置阶段","充电阶段","充电结束阶段","错误报文","多帧传输"};
Basic_CAN_frame *Basic_CAN_frame::create_frame(DWORD can_id){
	switch(can_id){
	case CHM_CANID:
		return new CAN_CHM();
	case BHM_CANID:
		return new CAN_BHM();
	case CRM_CANID:
		return new CAN_CRM();
	case CTS_CANID:
		return new CAN_CTS();
	case CML_CANID:
		return new CAN_CML();
	case BRO_CANID:
		return new CAN_BRO();
	case CRO_CANID:
		return new CAN_CRO();
	case BCL_CANID:
		return new CAN_BCL();
	case CCS_CANID:
		return new CAN_CCS();
	case BSM_CANID:
		return new CAN_BSM();
	case BST_CANID:
		return new CAN_BST();
	case CST_CANID:
		return new CAN_CST();
	case BSD_CANID:
		return new CAN_BSD();
	case CSD_CANID:
		return new CAN_CSD();
	case BEM_CANID:
		return new CAN_BEM();
	case CEM_CANID:
		return new CAN_CEM();
	case TP_START_CANID:
		return new CAN_TP_START();
	case TP_REPLY_CANID:
		return new CAN_TP_REPLY();
	case TP_DATA_CANID:
		return new CAN_TP_DATA();
	default:
		return NULL;
	}
}
//================================= 握手阶段 ===========================//
//-------------- CHM ----------------//
BOOL CAN_CHM::get_data(Table_field &tf){
	if(tf.getByteHex(LENGTH_BASE)==3&& tf.getNumField()>= 10){
		vmajor = tf.getByteHex( DATA_BASE + 0 );
		vminor = tf.getByteHex(DATA_BASE + 1 ) + (tf.getByteHex(DATA_BASE + 2)<<8);
		return true;
	}
	return false;
}
string CAN_CHM::get_detail()const{
		ostringstream oss;
		oss<<"充电桩协议版本号：";
		if(vmajor != 0xff || vminor != 0xffff)
			oss<<'v'<<(UINT)vmajor<<'.'<<vminor<<"...";
		else
			oss<<"无法识别！";
		return oss.str();
}
BOOL CAN_CHM::operator==(Basic_CAN_frame& cf)const{
	if(cf.getCANID() != canid)return false;
	CAN_CHM& tmp=dynamic_cast<CAN_CHM&>(cf);
	return tmp.state==state&&tmp.vmajor==vmajor && tmp.vminor==vminor;
}
Basic_CAN_frame& CAN_CHM::operator=(Basic_CAN_frame& can){
	if(getCANID() != can.getCANID()){
		return *this;
	}
	CAN_CHM & chm = dynamic_cast<CAN_CHM&> (can);
	index = chm.index;
	vmajor = chm.vmajor;
	vminor = chm.vminor;
	return *this;
}
//-------------- BHM ---------------//
BOOL CAN_BHM::get_data(Table_field &tf){
	if(tf.getByteHex(LENGTH_BASE)==2&& tf.getNumField()>= 9){
		hvol = tf.getByteHex( DATA_BASE + 0 )+(tf.getByteHex(DATA_BASE + 1 )<<8);
		return true;
	}
	return false;
}
string CAN_BHM::get_detail()const{
		ostringstream oss;
		oss<<"BMS最高允许电压：";
		if(hvol==0xffff)
			oss<<"未初始化;";
		else
			oss<<double(hvol)*0.1<<'V';
		return oss.str();
}
BOOL CAN_BHM::operator==(Basic_CAN_frame& cf)const{
	if(cf.getCANID() != canid)return false;
	CAN_BHM& tmp=dynamic_cast<CAN_BHM&>(cf);
	return tmp.hvol == hvol;
}
Basic_CAN_frame& CAN_BHM::operator=(Basic_CAN_frame& can){
	if(getCANID() != can.getCANID()){
		return *this;
	}
	CAN_BHM & bhm = dynamic_cast<CAN_BHM&> (can);
	index = bhm.index ;
	hvol = bhm.hvol;
	return *this;
}
//========================== 辨识阶段 =============================//
//-------------- BRM 多帧传输 ------------------//
 string CAN_BRM::get_detail()const{
		 ostringstream oss;
		 oss<<"通信协议版本号：v"<<(UINT)getByte(0)<<'.'<<getWord(1)<<".";
		 oss<<"电池类型:";
		 switch(getByte(3)){
		 case 0x01:
			oss<<"铅酸电池.";
			break;
		 case 0x02:
			oss<<"镍氢电池.";
			break;
		 case 0x03:
		 	oss<<"磷酸电池.";
		 	break;
		 case 0x04:
		 	oss<<"锰酸电池";
		 	break;
		 case 0x05:
		 	oss<<"钴酸电池.";
		 	break;
		 case 0x06:
			oss<<"三元材料电池.";
		 	break;
		 case 0x07:
		 	oss<<"聚合物锂电池.";
		 	break;
		 case 0x08:
		 	oss<<"钛酸电池.";
		 	break;
		 case 0xff:
			oss<<"未知电池.";
			break;
		 default:
			oss<<"错误!!!";
		 }
		 oss<<"整车动力蓄电池额定容量:"<< 0.1 * getWord(4) << "A。h."<<"额定电压为:"<< 0.1 * getWord(6)<<"V.";
		BYTE name[5]; name[4] = '\0';for(UINT i = 0 ; i < 4 ; i++)name[i]=getByte( 8 + i);
		if(name[0]!=0xff)
			oss<<"电池生产产商——"<<name<<";";
		UINT num = getDWord(12);
		if(num != 0xffffffff)
			oss<<"电池组序号：0x"<<hex<<setw(8)<<num<<';'<<dec;
		for(UINT i = 0 ; i < 3 ; i++) name[ i] = getByte(16+i);
		if(name[0]!=0xff || name[1]!=0xff||name[2]!=0xff)
			oss<<"生产日期:"<<1985 + name[0]<<"年"<<(UINT)name[1]<<"月"<<(UINT)name[2]<<"日;";
		num = getByte(19)+(getWord(20)<<8);
		if(num != 0xffffff){
			oss<<"充电次数:"<<num<<"次;";
		}
		switch(getByte(22)){
		case 0:
			oss<<"电池产权-租赁.";
			break;
		case 1:
			oss<<"电池产权-车辆自有.";
			break;
		}
		oss<<"车辆识别号:";
		for(UINT i = 0; i < 17; i++)
			oss<<getByte(24 + i);
		oss<<".\t";
		oss<<"BMS软件版本号："<<oct<<(getByte(44)<<8)+getByte(45)<<"年"<<(UINT)getByte(43)<<"月"<<(UINT)getByte(42)<<"日，第"<<(UINT)getByte(41)<<"次编译.";
	 return oss.str();
 }
BOOL CAN_BRM::rec_finish()const{
	if(frames.size() < BRM_TP_SIZE)
		return false;
	return true;
}
BOOL CAN_BRM::valid()const{
	UINT size=frames.size();
	if( size >= 3 && size <= BRM_TP_SIZE - 1  ){
		if(frames[ size - 1 ]->canid != TP_DATA_CANID)
			return false;
		CAN_TP_DATA* tmp_d = dynamic_cast<CAN_TP_DATA*>(frames[size -1 ]);
		return tmp_d->packet == size - 2  ;
	}else if(size == 1){
		if((frames[0])->canid != TP_START_CANID )
			return false;
		CAN_TP_START* tmp_s = dynamic_cast<CAN_TP_START*>(frames[0]);
		return tmp_s->console == TP_START_CONSOLE && tmp_s->PGN == BRM_PGN && tmp_s->length == 49 && tmp_s->packets == BRM_TP_SIZE - 3;
	}else if(size == 2){
		if((frames[1])->canid != TP_REPLY_CANID )
			return false;
		CAN_TP_REPLY* tmp_r = dynamic_cast<CAN_TP_REPLY*>(frames[1]);
		return tmp_r->compts.reply.console == TP_REPLY_CONSOLE && tmp_r->compts.reply.PGN == BRM_PGN && tmp_r->compts.reply.packets== BRM_TP_SIZE - 3 && tmp_r->compts.reply.nextpacket == 1;
	}else if(size == BRM_TP_SIZE){
		if((frames[ BRM_TP_SIZE - 1])->canid != TP_REPLY_CANID )
			return false;
		CAN_TP_REPLY* tmp_r = dynamic_cast<CAN_TP_REPLY*>(frames[ BRM_TP_SIZE - 1]);
		return tmp_r->compts.end.console == TP_END_CONSOLE && tmp_r->compts.end.PGN == BRM_PGN && tmp_r->compts.end.packets== BRM_TP_SIZE - 3 && tmp_r->compts.end.length == 49 ;
	}else  return false;
}
//--------------- CRM ---------------//
BOOL CAN_CRM::get_data(Table_field &tf){
	if(tf.getByteHex(LENGTH_BASE)==8 && tf.getNumField()>= 15){
		rec = tf.getByteHex( DATA_BASE + 0 );
		mnum=tf.getByteHex(DATA_BASE + 1 )+(tf.getByteHex(DATA_BASE + 2 )<<8)+(tf.getByteHex(DATA_BASE + 3 )<<16) +(tf.getByteHex(DATA_BASE + 4 )<<24);
		mloc=tf.getByteHex(DATA_BASE + 5 )+(tf.getByteHex(DATA_BASE + 6 )<<8)+(tf.getByteHex(DATA_BASE + 7 )<<16) ;
		return true;
	}
	return false;
}
string CAN_CRM::get_detail()const{
		ostringstream oss;
		oss<<"BMS辨识结果:";
		switch(rec){
		case 0x00:
			oss<<"未辨识!";
			break;
		case 0xAA:
			oss<<"成功辨识...";
			break;
		default:
			oss<<"未知!";
		}
		if(mnum!=0xffffffff)
			oss<<"充电机编号:0x"<<hex<<setw(8)<<mnum<<".\t";
		if(mloc !=0xffffff)
			oss<<"充电机地区编号:0x"<<setw(6)<<hex<<mloc<<'.';
		return oss.str();
}
BOOL CAN_CRM::operator==(Basic_CAN_frame& cf)const{
	if(cf.getCANID() != canid)return false;
	CAN_CRM& tmp=dynamic_cast<CAN_CRM&>(cf);
	return tmp.rec == rec && tmp.mnum == mnum && tmp.mloc == mloc;
}
Basic_CAN_frame& CAN_CRM::operator=(Basic_CAN_frame& can){
	if(getCANID() != can.getCANID()){
		return *this;
	}
	CAN_CRM & crm = dynamic_cast<CAN_CRM&> (can);
	index = crm.index ;
	rec = crm.rec ;
	mnum = crm.mnum;
	mloc = crm.mloc;
	return *this;
}
//========================== 配置阶段 =======================//
//-------------- BCP 多帧传输 ------------------//
 string CAN_BCP::get_detail()const{
	ostringstream oss;
	oss<<"单体动力蓄电池最高允许电压:"<<0.01*getWord(0)<<"V.";
	oss<<"最高允许充电电流为:"<<0.1*getWord(2) - 400<<"A,最高允许总电压为"<<0.1 *getWord(6)<<"V.动力蓄电池标称总能量为"<<0.1*getWord(4)<<"Kw。h.";
	oss<<"最高允许温度:"<<(UINT)getByte(8)-50<<"摄氏度.当前电池电量(SOC):"<<0.1*(UINT)getWord(9)<<"%,总电压:"<<0.1*getWord(11)<<"V.";
	return oss.str();
 }
BOOL CAN_BCP::rec_finish()const{
	if(frames.size() < BCP_TP_SIZE)
		return false;
	return true;
}
BOOL CAN_BCP::valid()const{
	UINT size=frames.size();
	if( size >= 3 && size <= BCP_TP_SIZE - 1  ){
		if(frames[ size - 1 ]->canid != TP_DATA_CANID)
			return false;
		CAN_TP_DATA* tmp_d = dynamic_cast<CAN_TP_DATA*>(frames[size -1 ]);
		return tmp_d->packet == size - 2  ;
	}else if(size == 1){
		if((frames[0])->canid != TP_START_CANID )
			return false;
		CAN_TP_START* tmp_s = dynamic_cast<CAN_TP_START*>(frames[0]);
		return tmp_s->console == TP_START_CONSOLE && tmp_s->PGN == BCP_PGN && tmp_s->length == 13 && tmp_s->packets == BCP_TP_SIZE - 3;
	}else if(size == 2){
		if((frames[1])->canid != TP_REPLY_CANID )
			return false;
		CAN_TP_REPLY* tmp_r = dynamic_cast<CAN_TP_REPLY*>(frames[1]);
		return tmp_r->compts.reply.console == TP_REPLY_CONSOLE && tmp_r->compts.reply.PGN == BCP_PGN && tmp_r->compts.reply.packets== BCP_TP_SIZE - 3 && tmp_r->compts.reply.nextpacket == 1;
	}else if(size == BCP_TP_SIZE){
		if((frames[ BCP_TP_SIZE - 1])->canid != TP_REPLY_CANID )
			return false;
		CAN_TP_REPLY* tmp_r = dynamic_cast<CAN_TP_REPLY*>(frames[ BCP_TP_SIZE - 1]);
		return tmp_r->compts.end.console == TP_END_CONSOLE && tmp_r->compts.end.PGN == BCP_PGN && tmp_r->compts.end.packets== BCP_TP_SIZE - 3 && tmp_r->compts.end.length == 49 ;
	}else return false;
}
//---------------- CTS -------------------//
static inline UINT GET_BCD_BYTE(BYTE b){
	return ((b>>4)&0x0f) * 10 + (b&0x0f);
}
static UINT GET_BCD_WORD(WORD b){
	return GET_BCD_BYTE((b>>8)&0xff)*100+GET_BCD_BYTE((b)&0xff);
}
BOOL CAN_CTS::get_data(Table_field &tf){
	if(tf.getByteHex(LENGTH_BASE)==7 && tf.getNumField()>= 14){
		second = tf.getByteHex( DATA_BASE + 0 );
		minute=tf.getByteHex(DATA_BASE + 1 );
		hour=tf.getByteHex(DATA_BASE + 2 );
		day=tf.getByteHex(DATA_BASE + 3 );
		month=tf.getByteHex(DATA_BASE + 4 );
		year=tf.getByteHex(DATA_BASE + 5 )+(tf.getByteHex(DATA_BASE + 6 )<<8);
		return true;
	}
	return false;
}
string CAN_CTS::get_detail()const{
		ostringstream oss;
		if(second != 0xff || minute != 0xff || hour != 0xff || day != 0xff || month != 0xff || year != 0xffff)
			oss<<"同步时间:\t"<<setw(4)<<GET_BCD_WORD(year)<<"年"<<setw(2)<<(UINT)GET_BCD_BYTE(month)<<"月"<<setw(2)<<(UINT)GET_BCD_BYTE(day)<<"日"<<setw(2)<<(UINT)GET_BCD_BYTE(hour)<<"时"<<setw(2)<<(UINT)GET_BCD_BYTE(minute)<<"分"<<setw(2)<<(UINT)GET_BCD_BYTE(second)<<"秒";
		return oss.str();
}
BOOL CAN_CTS::operator==(Basic_CAN_frame& cf)const{
	if(cf.getCANID() != canid)return false;
	CAN_CTS& tmp=dynamic_cast<CAN_CTS&>(cf);
	return tmp.second== second && tmp.minute == minute && tmp.hour == hour && tmp.day == day && tmp.month == month && tmp.year == year;
}
Basic_CAN_frame& CAN_CTS::operator=(Basic_CAN_frame& can){
	if(getCANID() != can.getCANID()){
		return *this;
	}
	CAN_CTS & cts = dynamic_cast<CAN_CTS&> (can);
	index =cts.index;
	second = cts.second  ;
	minute = cts.minute;
	hour = cts.hour;
	day = cts.day ;
	month = cts.month ;
	year = cts.year ;
	return *this;
}
//------------------ CML --------------------//
BOOL CAN_CML::get_data(Table_field &tf){
	if(tf.getByteHex(LENGTH_BASE)==8 && tf.getNumField()>= 15){
		houtvol = tf.getByteHex( DATA_BASE + 0 )+(tf.getByteHex(DATA_BASE + 1 )<<8);
		loutvol=tf.getByteHex(DATA_BASE + 2 ) + (tf.getByteHex(DATA_BASE + 3 )<<8);
		houtcur=tf.getByteHex(DATA_BASE + 4 ) + (tf.getByteHex(DATA_BASE + 5 )<<8);
		loutcur=tf.getByteHex(DATA_BASE + 6 )+(tf.getByteHex(DATA_BASE + 7 )<<8);
		return true;
	}
	return false;
}
string CAN_CML::get_detail()const{
		ostringstream oss;
		if(houtvol != 0xffff)
			oss<<"充电桩最高输出电压： "<<0.1 *houtvol<<"V.";
		if(loutvol != 0xffff)
			oss<<"充电桩最低输出电压:"<<0.1 * loutvol <<"V.";
		if(houtcur != 0xffff)
			oss<<"充电桩最大输出电流"<< 0.1 * houtcur - 400 << "A.";
		if(loutcur != 0xffff)
			oss<<"充电桩最小输出电流"<< 0.1 * loutcur - 400 << "A.";
		return oss.str();
}
BOOL CAN_CML::operator==(Basic_CAN_frame& cf)const{
	if(cf.getCANID() != canid)return false;
	CAN_CML& tmp=dynamic_cast<CAN_CML&>(cf);
	return tmp.houtvol== houtvol && tmp.loutvol == loutvol && tmp.houtcur == houtcur && tmp.loutcur == loutcur;
}
Basic_CAN_frame& CAN_CML::operator=(Basic_CAN_frame& can){
	if(getCANID() != can.getCANID()){
		return *this;
	}
	CAN_CML & cml = dynamic_cast<CAN_CML&> (can);
	index =cml.index;
	houtvol = cml.houtvol;
	loutvol = cml.loutvol;
	houtcur = cml.houtcur;
	loutcur = cml.loutcur;
	return *this;
}
//---------------- BRO -------------------//
BOOL CAN_BRO::get_data(Table_field &tf){
	if(tf.getByteHex(LENGTH_BASE)==1 && tf.getNumField()>= 8){
		rec = tf.getByteHex( DATA_BASE + 0 );
		return true;
	}
	return false;
}
string CAN_BRO::get_detail()const{
	ostringstream oss;
	oss<<"BMS充电就绪状态--";
	switch(rec){
	case 0x00:
		oss<<"未就绪!";
		break;
	case 0xAA:
		oss<<"准备就绪...";
		break;
	default:
		oss<<"未知!";
			}
	return oss.str();
}
BOOL CAN_BRO::operator==(Basic_CAN_frame& cf)const{
	if(cf.getCANID() != canid)return false;
	CAN_BRO& tmp=dynamic_cast<CAN_BRO&>(cf);
	return tmp.rec== rec ;
}
Basic_CAN_frame& CAN_BRO::operator=(Basic_CAN_frame& can){
	if(getCANID() != can.getCANID()){
		return *this;
	}
	CAN_BRO & bro = dynamic_cast<CAN_BRO&> (can);
	index =bro.index;
	rec= bro.rec ;
	return *this;
}
//------------------ CRO ------------------//
BOOL CAN_CRO::get_data(Table_field &tf){
	if(tf.getByteHex(LENGTH_BASE)==1 && tf.getNumField()>= 8){
		rec = tf.getByteHex( DATA_BASE + 0 );
		return true;
	}
	return false;
}
string CAN_CRO::get_detail()const{
	ostringstream oss;
	oss<<"充电桩充电就绪状态--";
	switch(rec){
	case 0x00:
		oss<<"未就绪!";
		break;
	case 0xAA:
		oss<<"准备就绪...";
		break;
	default:
		oss<<"未知!";
	}
	return oss.str();
}
BOOL CAN_CRO::operator==(Basic_CAN_frame& cf)const{
	if(cf.getCANID() != canid)return false;
	CAN_CRO& tmp=dynamic_cast<CAN_CRO&>(cf);
	return tmp.rec== rec ;
}
Basic_CAN_frame& CAN_CRO::operator=(Basic_CAN_frame& can){
	if(getCANID() != can.getCANID()){
		return *this;
	}
	CAN_CRO & cro = dynamic_cast<CAN_CRO&> (can);
	index =cro.index;
	rec =cro.rec ;
	return *this;
}
//============================== 充电阶段 ============================//
//------------------- BCL -------------------//
BOOL CAN_BCL::get_data(Table_field &tf){
	if(tf.getByteHex(LENGTH_BASE)==5 && tf.getNumField()>= 12){
		nvol = tf.getByteHex( DATA_BASE + 0 )+(tf.getByteHex(DATA_BASE + 1 )<<8);
		ncur = tf.getByteHex(DATA_BASE + 2 ) + (tf.getByteHex(DATA_BASE + 3 )<<8);
		cmod = tf.getByteHex(DATA_BASE + 4 );
		return true;
	}
	return false;
}
string CAN_BCL::get_detail()const{
	ostringstream oss;
	if(nvol != 0xffff)
		oss<<"BMS需求电压:"<<0.1 *nvol<<"V.";
	if(ncur != 0xffff)
		oss<<"BMS需求电流:"<< 0.1 * ncur - 400 << "A.";
	oss<<"充电模式--";
	switch(cmod){
		case 0x01:
			oss<<"恒压充电!";
			break;
		case 0x02:
			oss<<"恒流充电...";
			break;
		default:
			oss<<"未知!";
	}
	return oss.str();
}
BOOL CAN_BCL::operator==(Basic_CAN_frame& cf)const{
	if(cf.getCANID() != canid)return false;
	CAN_BCL& tmp=dynamic_cast<CAN_BCL&>(cf);
	return tmp.nvol== nvol && tmp.ncur == ncur && tmp.cmod == cmod;
}
Basic_CAN_frame& CAN_BCL::operator=(Basic_CAN_frame& can){
	if(getCANID() != can.getCANID()){
		return *this;
	}
	CAN_BCL & bcl = dynamic_cast<CAN_BCL&> (can);
	index =bcl.index;
	nvol =bcl.nvol ;
	ncur = bcl.ncur;
	cmod =bcl.cmod ;
	return *this;
}
//-------------- BCS 多帧传输 ------------------//
 string CAN_BCS::get_detail()const{
	ostringstream oss;
	oss<<"充电电压测量值:"<<0.1*getWord(0)<<"V.";
	oss<<"充电电流测量值:"<<0.1 * getWord(2) - 400<<"A.最高当体蓄电池电压及其组号:("<<0.1 * (0xfff & getWord(4))<<"V,0x"<<(0xf&(getWord(4)>>12))<<").当前荷电状态(SOC):"<<(UINT)getByte(6)<<"%.目前剩余充电时间为"<<getWord(7)<<"分钟.";
	return oss.str();
 }
BOOL CAN_BCS::rec_finish()const{
	if(frames.size() < BCS_TP_SIZE)
		return false;
	return true;
}
BOOL CAN_BCS::valid()const{
	UINT size=frames.size();
	if( size >= 3 && size <= BCS_TP_SIZE - 1  ){
		if(frames[ size - 1 ]->canid != TP_DATA_CANID)
			return false;
		CAN_TP_DATA* tmp_d = dynamic_cast<CAN_TP_DATA*>(frames[size -1 ]);
		return tmp_d->packet == size - 2  ;
	}else if(size == 1){
		if((frames[0])->canid != TP_START_CANID )
			return false;
		CAN_TP_START* tmp_s = dynamic_cast<CAN_TP_START*>(frames[0]);
		return tmp_s->console == TP_START_CONSOLE && tmp_s->PGN == BCS_PGN && tmp_s->length == 13 && tmp_s->packets == BCS_TP_SIZE - 3;
	}else if(size == 2){
		if((frames[1])->canid != TP_REPLY_CANID )
			return false;
		CAN_TP_REPLY* tmp_r = dynamic_cast<CAN_TP_REPLY*>(frames[1]);
		return tmp_r->compts.reply.console == TP_REPLY_CONSOLE && tmp_r->compts.reply.PGN == BCS_PGN && tmp_r->compts.reply.packets== BCS_TP_SIZE - 3 && tmp_r->compts.reply.nextpacket == 1;
	}else if(size == BCS_TP_SIZE){
		if((frames[ BCS_TP_SIZE - 1])->canid != TP_REPLY_CANID )
			return false;
		CAN_TP_REPLY* tmp_r = dynamic_cast<CAN_TP_REPLY*>(frames[ BCS_TP_SIZE - 1]);
		return tmp_r->compts.end.console == TP_END_CONSOLE && tmp_r->compts.end.PGN == BCS_PGN && tmp_r->compts.end.packets== BCS_TP_SIZE - 3 && tmp_r->compts.end.length == 49 ;
	}else return false;
}
//---------------- CCS ------------------//
BOOL CAN_CCS::get_data(Table_field &tf){
	if(tf.getByteHex(LENGTH_BASE)==7 && tf.getNumField()>= 14){
		ovol = tf.getByteHex( DATA_BASE + 0 )+(tf.getByteHex(DATA_BASE + 1 )<<8);
		ocur = tf.getByteHex(DATA_BASE + 2 ) + (tf.getByteHex(DATA_BASE + 3 )<<8);
		chargetime = tf.getByteHex(DATA_BASE + 4 ) + (tf.getByteHex(DATA_BASE + 5 )<<8);
		chargeallow = tf.getByteHex(DATA_BASE + 6);
		return true;
	}
	return false;
}
string CAN_CCS::get_detail()const{
	ostringstream oss;
	if(ovol != 0xffff)
		oss<<"电压输出为"<<0.1 * ovol<<"V.";
	if(ocur != 0xffff)
		oss<<"电流输出为"<< 0.1 * ocur - 400 << "A.";
	if(chargetime != 0xffff)
			oss<<"目前已经充电"<< chargetime << "分钟.";
	oss<<"当前充电状态--";
	switch(chargeallow&0x03){
		case 0x00:
			oss<<"禁止充电!";
			break;
		case 0x01:
			oss<<"正在充电...";
			break;
		default:
			oss<<"未知!";
	}
	return oss.str();
}
BOOL CAN_CCS::operator==(Basic_CAN_frame& cf)const{
	if(cf.getCANID() != canid)return false;
	CAN_CCS& tmp=dynamic_cast<CAN_CCS&>(cf);
	return tmp.ovol== ovol && tmp.ocur == ocur && tmp.chargetime == chargetime && tmp.chargeallow == chargeallow;
}
Basic_CAN_frame& CAN_CCS::operator=(Basic_CAN_frame& can){
	if(getCANID() != can.getCANID()){
		return *this;
	}
	CAN_CCS & ccs = dynamic_cast<CAN_CCS&> (can);
	index =ccs.index;
	ovol = ccs.ovol;
	ocur = ccs.ocur;
	chargetime =ccs.chargetime;
	chargeallow = ccs.chargeallow;
	return *this;
}
//------------------ BSM ----------------//
BOOL CAN_BSM::get_data(Table_field &tf){
	if(tf.getByteHex(LENGTH_BASE)==7 && tf.getNumField()>= 14){
		maxvolid = tf.getByteHex( DATA_BASE + 0 ) ;
		maxtem = tf.getByteHex(DATA_BASE + 1 );
		maxtemid = tf.getByteHex(DATA_BASE + 2 ) ;
		mintem = tf.getByteHex(DATA_BASE + 3 );
		mintemid = tf.getByteHex(DATA_BASE + 4 ) ;
		chargestate.data =tf.getByteHex(DATA_BASE + 5 ) + (tf.getByteHex(DATA_BASE + 6 ) << 8 );
		return true;
	}
	return false;
}
string CAN_BSM::get_detail()const{
	ostringstream oss;
	if(maxvolid != 0xff)
		oss<<"当前最大单体电池电压编号：0x"<<hex<<setw(2)<<(UINT)maxvolid<<".";
	if(maxtem != 0xff)
		oss<<"最高电池温度编号:0x"<< hex<<setw(2)<< (UINT)maxtemid << ",其温度为:"<<dec<<(UINT)maxtem - 50<<"摄氏度.";
	if(mintem != 0xff)
			oss<<"最低电池温度编号:0x"<< hex<<setw(2)<< (UINT)mintemid << ",其温度为:"<<dec<<(UINT)mintem - 50<<"摄氏度.";
	oss<<"当前动力蓄电池状态--";
	if(chargestate.data == 0)
		oss<<"正常。";
	else{
		switch(chargestate.compts.volstate){
		case 1:
		oss<<"电压过高,";
		break;
		case 2:
		oss<<"电压过低,";
		break;
		}
		switch(chargestate.compts.chargestate){
		case 1:
			oss<<"荷电状态过高,";
			break;
		case 2:
			oss<<"荷电状态过低,";
			break;
		}
		switch(chargestate.compts.curstate){
		case 1:
			oss<<"充电过流,";
			break;
		case 2:
			oss<<"充电过流标志位不可信,";
			break;
		}
		switch(chargestate.compts.temstate){
		case 1:
			oss<<"温度过高,";
			break;
		case 2:
			oss<<"温度标志位不可信,";
			break;
		}
		switch(chargestate.compts.insultstate){
			case 1:
				oss<<"绝缘状态正常,";
				break;
			case 2:
				oss<<"绝缘状态标志位不可信,";
				break;
		}
		switch(chargestate.compts.OCS){
			case 1:
				oss<<"输出连接器不正常,";
				break;
			case 2:
				oss<<"输出连接器状态标志位不可信,";
				break;
		}
		switch(chargestate.compts.CA){
			case 0:
				oss<<"禁止充电。";
				break;
			case 1:
				oss<<"允许充电。";
				break;
			case 2:
				oss<<"充电允许状态未知.";
		}
	}
	return oss.str();
}
BOOL CAN_BSM::operator==(Basic_CAN_frame& cf)const{
	if(cf.getCANID() != canid)return false;
	CAN_BSM& tmp=dynamic_cast<CAN_BSM&>(cf);
	return  tmp.maxtem == maxtem  && tmp.mintem == mintem && tmp.chargestate.data == chargestate.data && tmp.maxvolid== maxvolid && tmp.maxtemid == maxtemid && tmp.mintemid == mintemid;
}
Basic_CAN_frame& CAN_BSM::operator=(Basic_CAN_frame& can){
	if(getCANID() != can.getCANID()){
		return *this;
	}
	CAN_BSM & bsm = dynamic_cast<CAN_BSM&> (can);
	index =bsm.index;
	maxvolid =  bsm.maxvolid ;
	maxtem = bsm.maxtem ;
	maxtemid = bsm.maxtemid;
	mintem = bsm.mintem ;
	mintemid = bsm.mintemid;
	chargestate.data = bsm.chargestate.data ;
	return *this;
}
//--------------- BST -------------------//
BOOL CAN_BST::get_data(Table_field &tf){
	if(tf.getByteHex(LENGTH_BASE)==4 && tf.getNumField()>= 11){
		stopinfo.data = tf.getByteHex( DATA_BASE + 0 ) ;
		faultinfo.data[ 0 ] = tf.getByteHex(DATA_BASE + 1 ) ;
		faultinfo.data[ 1 ] = tf.getByteHex(DATA_BASE + 2 ) ;
		stopfaultinfo.data = tf.getByteHex(DATA_BASE + 3 ) ;
		return true;
	}
	return false;
}
string CAN_BST::get_detail()const{
	ostringstream oss;
	oss<<"BMS中止充电原因--";
	if(stopinfo.data==0)
		oss<<"无;";
	else{
		if(stopinfo.compts.stopbySOC == 1)
			oss<<"所需SOC值已达到;";
		if(stopinfo.compts.stopbyVT == 1)
			oss<<"总电压设定值已达到;";
		if(stopinfo.compts.stopbyVN == 1)
			oss<<"单体电池电压设定值已达到;";
		if(stopinfo.compts.stopbyCM == 1)
			oss<<"充电桩主动要求中止充电;";
	}
	oss<<"BMS中止充电的故障原因--";
	if(faultinfo.data[0] == 0 && faultinfo.data[1] == 0)
		oss<<"无.";
	else{
		switch(faultinfo.compt.insultFault){
		case 1:
			oss<<"绝缘故障,";
			break;
		case 2:
			oss<<"绝缘故障标志位不可信,";
			break;
		}
		switch(faultinfo.compt.OCTFault){
		case 1:
			oss<<"输出连接器过温,";
			break;
		case 2:
			oss<<"输出连接器标志位不可信,";
			break;
		}
		switch(faultinfo.compt.BMStemFault){
		case 1:
			oss<<"BMS、输出连接器过温,";
			break;
		case 2:
			oss<<"BMS、输出连接器标志位不可信,";
			break;
		}
		switch(faultinfo.compt.CCFault){
			case 1:
				oss<<"充电连接器故障,";
				break;
			case 2:
				oss<<"充电连接器标志位不可信,";
				break;
		}
		switch(faultinfo.compt.BTFault){
			case 1:
				oss<<"电池组温度过高,";
				break;
			case 2:
				oss<<"电池组温度状态标志位不可信,";
				break;
		}
		switch(faultinfo.compt.HVolFault){
		case 1:
				oss<<"高压继电器故障。";
				break;
		case 2:
			oss<<"高压继电器标志位异常。";
			break;
		}
		switch(faultinfo.compt.T2VolFault){
		case 1:
			oss<<"检测点2电压检测故障。";
				break;
		case 2:
			oss<<"检测点2电压状态标志位异常。";
			break;
		}
		switch(faultinfo.compt.OtherFault){
		case 1:
			oss<<"其他故障。";
				break;
		case 2:
			oss<<"其他故障标志位异常。";
			break;
		}
	}
	oss<<"BMS中止充电错误原因--";
	if((stopfaultinfo.data&0x0f) == 0)
		oss<<"无.";
	else{
		switch(stopfaultinfo.compt.curfault){
		case 1:
			oss<<"电流过大,";
			break;
		case 2:
			oss<<"电流过大标志位异常,";
			break;
		}
		switch(stopfaultinfo.compt.volfault){
		case 1:
			oss<<"电压异常.";
			break;
		case 2:
			oss<<"电压异常标志位异常.";
			break;
		}
	}
	return oss.str();
}
BOOL CAN_BST::operator==(Basic_CAN_frame& cf)const{
	if(cf.getCANID() != canid)return false;
	CAN_BST& tmp=dynamic_cast<CAN_BST&>(cf);
	return  tmp.stopinfo.data == stopinfo.data  && tmp.faultinfo.data[ 0 ] == faultinfo.data[ 0 ] && tmp.faultinfo.data[ 1 ] == faultinfo.data[ 1 ] && tmp.stopfaultinfo.data == stopfaultinfo.data;
}
Basic_CAN_frame& CAN_BST::operator=(Basic_CAN_frame& can){
	if(getCANID() != can.getCANID()){
		return *this;
	}
	CAN_BST & bst = dynamic_cast<CAN_BST&> (can);
	index =bst.index;
	stopinfo.data = bst.stopinfo.data;
	faultinfo.data[0] = bst.faultinfo.data[0];
	faultinfo.data[1] = bst.faultinfo.data[1];
	stopfaultinfo.data = bst.stopfaultinfo.data;
	return *this;
}
//------------------ CST ------------------//
BOOL CAN_CST::get_data(Table_field &tf){
	if(tf.getByteHex(LENGTH_BASE)==4 && tf.getNumField()>= 11){
		stopinfo.data = tf.getByteHex( DATA_BASE + 0 ) ;
		faultinfo.data[ 0 ] = tf.getByteHex(DATA_BASE + 1 ) ;
		faultinfo.data[ 1 ] = tf.getByteHex(DATA_BASE + 2 ) ;
		stopfaultinfo.data = tf.getByteHex(DATA_BASE + 3 ) ;
		return true;
	}
	return false;
}
string CAN_CST::get_detail()const{
	ostringstream oss;
	oss<<"充电桩中止充电原因--";
	if(stopinfo.data == 0){
		oss<<"无";
	}else{
		if(stopinfo.compts.stopbyCM == 1)
			oss<<"充电机设定条件已达到;";
		if(stopinfo.compts.stopbyMan == 1)
			oss<<"人工中止;";
		if(stopinfo.compts.stopbyFault == 1)
			oss<<"故障中止;";
		if(stopinfo.compts.stopbyBMS == 1)
			oss<<"BMS主动要求中止充电;";
	}
	oss<<"充电桩充电中止的故障原因--";
	if(faultinfo.data[0] == 0 && (faultinfo.data[1]&0x0f) == 0)
		oss<<"无.";
	else{
		switch(faultinfo.compt.HTempFault){
		case 1:
			oss<<"充电机过温,";
			break;
		case 2:
			oss<<"充电机过温标志位不可信,";
			break;
		}
		switch(faultinfo.compt.CCFault){
		case 1:
			oss<<"充电连接连接器故障,";
			break;
		case 2:
			oss<<"充电连接器标志位不可信,";
			break;
		}
		switch(faultinfo.compt.CMtemFault){
		case 1:
			oss<<"充电机内部过温,";
			break;
		case 2:
			oss<<"充电机内部过温标志位不可信,";
			break;
		}
		switch(faultinfo.compt.CurFault){
			case 1:
				oss<<"所需电流无法传送,";
				break;
			case 2:
				oss<<"电流传送标志位不可信,";
				break;
		}
		switch(faultinfo.compt.CSFault){
			case 1:
				oss<<"充电机急停故障,";
				break;
			case 2:
				oss<<"充电机急停标志位不可信,";
				break;
		}
		switch(faultinfo.compt.OtherFault){
		case 1:
			oss<<"其他故障。";
				break;
		case 2:
			oss<<"其他故障标志位异常。";
			break;
		}
	}
	oss<<"充电桩中止充电错误原因--";
	if((stopfaultinfo.data&0x0f) == 0)
		oss<<"无.";
	else{
		switch(stopfaultinfo.compt.curfault){
		case 1:
			oss<<"电流不匹配,";
			break;
		case 2:
			oss<<"电流匹配标志位异常,";
			break;
		}
		switch(stopfaultinfo.compt.volfault){
		case 1:
			oss<<"电压异常.";
			break;
		case 2:
			oss<<"电压异常标志位异常.";
			break;
		}
	}
	return oss.str();
}
BOOL CAN_CST::operator==(Basic_CAN_frame& cf)const{
	if(cf.getCANID() != canid)return false;
	CAN_CST& tmp=dynamic_cast<CAN_CST&>(cf);
	return  tmp.stopinfo.data == stopinfo.data  && tmp.faultinfo.data[0] == faultinfo.data[0] &&  tmp.faultinfo.data[1] == faultinfo.data[1] &&tmp.stopfaultinfo.data == stopfaultinfo.data;
}
Basic_CAN_frame& CAN_CST::operator=(Basic_CAN_frame& can){
	if(getCANID() != can.getCANID()){
		return *this;
	}
	CAN_CST & cst = dynamic_cast<CAN_CST&> (can);
	index =cst.index;
	stopinfo.data = cst.stopinfo.data;
	faultinfo.data[0] = cst.faultinfo.data[0];
	faultinfo.data[1] = cst.faultinfo.data[1];
	stopfaultinfo.data = cst.stopfaultinfo.data;
	return *this;
}
//================================ 充电结束阶段 ==========================//
//----------------- BSD ------------------//
BOOL CAN_BSD::get_data(Table_field &tf){
	if(tf.getByteHex(LENGTH_BASE)==7 && tf.getNumField()>= 14){
		SOC = tf.getByteHex( DATA_BASE + 0 ) ;
		LVol = tf.getByteHex(DATA_BASE + 1 ) + (tf.getByteHex(DATA_BASE + 2 )<<8);
		HVol =  tf.getByteHex(DATA_BASE + 3 ) + (tf.getByteHex(DATA_BASE + 4 )<<8);
		LTem =  tf.getByteHex(DATA_BASE + 5 );
		HTem = tf.getByteHex(DATA_BASE + 6);
		return true;
	}
	return false;
}
string CAN_BSD::get_detail()const{
	ostringstream oss;
	if(SOC != 0xff)
		oss<<"当前荷电状态(SOC)"<< (UINT)SOC<<"%.";
	if(LVol != 0xffff)
		oss<<"单体电池最低电压"<< 0.01 * LVol << "V.";
	if(HVol != 0xffff)
		oss<<"单体电池最高电压"<< 0.01 * HVol << "V.";
	if(LTem != 0xff)
		oss<<"动力蓄电池最低温度"<<(UINT) LTem - 50 << "摄氏度.";
	if(HTem != 0xff)
		oss<<"动力蓄电池最高温度"<< (UINT)HTem - 50 << "摄氏度.";
	return oss.str();
}
BOOL CAN_BSD::operator==(Basic_CAN_frame& cf)const{
	if(cf.getCANID() != canid)return false;
	CAN_BSD& tmp=dynamic_cast<CAN_BSD&>(cf);
	return tmp.SOC== SOC && tmp.LVol == LVol && tmp.HVol == HVol && tmp.LTem == LTem && tmp.HTem == HTem;
}
Basic_CAN_frame& CAN_BSD::operator=(Basic_CAN_frame& can){
	if(getCANID() != can.getCANID()){
		return *this;
	}
	CAN_BSD & bsd = dynamic_cast<CAN_BSD&> (can);
	index =bsd.index;
	SOC = bsd.SOC ;
	LVol = bsd.LVol;
	HVol =bsd.HVol;
	LTem = bsd.LTem;
	HTem = bsd.HTem;
	return *this;
}
//---------------- CSD ------------------//
BOOL CAN_CSD::get_data(Table_field &tf){
	if(tf.getByteHex(LENGTH_BASE)==8 && tf.getNumField()>= 15){
		chargetime = tf.getByteHex( DATA_BASE + 0 ) + (tf.getByteHex(DATA_BASE + 1 )<<8);
		outenergy = tf.getByteHex(DATA_BASE + 2 ) + (tf.getByteHex(DATA_BASE + 3 ) << 8 ) ;
		CM_id =  tf.getByteHex(DATA_BASE + 4 )+ (tf.getByteHex(DATA_BASE + 5 )<<8) + (tf.getByteHex(DATA_BASE + 6 )<<16)+ (tf.getByteHex(DATA_BASE + 7 )<<24);
		return true;
	}
	return false;
}
string CAN_CSD::get_detail()const{
	ostringstream oss;
	if(chargetime != 0xffff)
		oss<<"充电所用时间:"<< chargetime <<"分钟.";
	if(outenergy != 0xffff)
		oss<<"输出能量"<< 0.1 * outenergy << "Kw。h.";
	if(CM_id != 0xffffffff)
		oss<<"充电机编号:0x"<<hex<<setw(8)<<CM_id<<'.';
	return oss.str();
}
BOOL CAN_CSD::operator==(Basic_CAN_frame& cf)const{
	if(cf.getCANID() != canid)return false;
	CAN_CSD& tmp=dynamic_cast<CAN_CSD&>(cf);
	return tmp.chargetime== chargetime && tmp.outenergy == outenergy && tmp.CM_id == CM_id;
}
Basic_CAN_frame& CAN_CSD::operator=(Basic_CAN_frame& can){
	if(getCANID() != can.getCANID()){
		return *this;
	}
	CAN_CSD & csd = dynamic_cast<CAN_CSD&> (can);
	index =csd.index;
	chargetime = csd.chargetime ;
	outenergy = csd.outenergy;
	CM_id = csd.CM_id ;
	return *this;
}
//========================== 错误报文 =========================//
//------------ BEM -----------------//
BOOL CAN_BEM::get_data(Table_field &tf){
	if(tf.getByteHex(LENGTH_BASE)== 4 && tf.getNumField()>= 11){
		errinfo.data = tf.getByteHex( DATA_BASE + 0 ) + (tf.getByteHex(DATA_BASE + 1 )<<8 ) + (tf.getByteHex(DATA_BASE + 2 )<< 16 ) + (tf.getByteHex(DATA_BASE + 3 )<<24 ) ;
		return true;
	}
	return false;
}
string CAN_BEM::get_detail()const{
	ostringstream oss;
	oss<<"当前充电桩通信状态--";
	if(errinfo.data == 0)
		oss<<"无.";
	else{
		switch(errinfo.compts.CRM00_OV){
		case 1:
			oss<<"CRM=0x00报文超时";
			break;
		case 2:
			oss<<"CRM00超时标志位不可信,";
			break;
		}
		switch(errinfo.compts.CRMAA_OV){
		case 1:
			oss<<"CRM=0xAA报文超时,";
			break;
		case 2:
			oss<<"CRMAA超时标志位不可信,";
			break;
		}
		switch(errinfo.compts.CML_OV){
		case 1:
			oss<<"CML报文超时,";
			break;
		case 2:
			oss<<"CML超时标志位不可信,";
			break;
		}
		switch(errinfo.compts.CRO_OV){
		case 1:
			oss<<"CRO报文超时,";
			break;
		case 2:
			oss<<"CRO超时标志位不可信,";
			break;
		}
		switch(errinfo.compts.CCS_OV){
		case 1:
			oss<<"CCS报文超时,";
			break;
		case 2:
			oss<<"CCS超时标志位不可信,";
			break;
		}
		switch(errinfo.compts.CST_OV){
		case 1:
			oss<<"CST报文超时,";
			break;
		case 2:
			oss<<"CST超时标志位不可信,";
			break;
		}
		switch(errinfo.compts.CSD_OV){
		case 1:
			oss<<"CSD报文超时,";
			break;
		case 2:
			oss<<"CSD超时标志位不可信,";
			break;
		}
	}
	return oss.str();
}
BOOL CAN_BEM::operator==(Basic_CAN_frame& cf)const{
	if(cf.getCANID() != canid)return false;
	CAN_BEM& tmp=dynamic_cast<CAN_BEM&>(cf);
	return  tmp.errinfo.data == errinfo.data ;
}
Basic_CAN_frame& CAN_BEM::operator=(Basic_CAN_frame& can){
	if(getCANID() != can.getCANID()){
		return *this;
	}
	CAN_BEM & bem = dynamic_cast<CAN_BEM&> (can);
	index =bem.index;
	errinfo.data = bem.errinfo.data;
	return *this;
}
//------------------ CEM ---------------------//
BOOL CAN_CEM::get_data(Table_field &tf){
	if(tf.getByteHex(LENGTH_BASE)== 4 && tf.getNumField()>= 11){
		errinfo.data = tf.getByteHex( DATA_BASE + 0 ) + (tf.getByteHex(DATA_BASE + 1 )<<8 ) + (tf.getByteHex(DATA_BASE + 2 )<< 16 ) + (tf.getByteHex(DATA_BASE + 3 )<<24 ) ;
		return true;
	}
	return false;
}
string CAN_CEM::get_detail()const{
	ostringstream oss;
	oss<<"当前BMS通信状态--";
	if(errinfo.data == 0)
		oss<<"无.";
	else{
		switch(errinfo.compts.BRM_OV){
		case 1:
			oss<<"BRM报文超时";
			break;
		case 2:
			oss<<"BRM超时标志位不可信,";
			break;
		}
		switch(errinfo.compts.BCP_OV){
		case 1:
			oss<<"BCP报文超时,";
			break;
		case 2:
			oss<<"BCP超时标志位不可信,";
			break;
		}
		switch(errinfo.compts.BRO_OV){
		case 1:
			oss<<"BRO报文超时,";
			break;
		case 2:
			oss<<"BRO超时标志位不可信,";
			break;
		}
		switch(errinfo.compts.BCS_OV){
		case 1:
			oss<<"BCS报文超时,";
			break;
		case 2:
			oss<<"BCS超时标志位不可信,";
			break;
		}
		switch(errinfo.compts.BCL_OV){
		case 1:
			oss<<"BCL报文超时,";
			break;
		case 2:
			oss<<"BCL超时标志位不可信,";
			break;
		}
		switch(errinfo.compts.BST_OV){
		case 1:
			oss<<"BST报文超时,";
			break;
		case 2:
			oss<<"BST超时标志位不可信,";
			break;
		}
		switch(errinfo.compts.BSD_OV){
		case 1:
			oss<<"BSD报文超时,";
			break;
		case 2:
			oss<<"BSD超时标志位不可信,";
			break;
		}
	}
	return oss.str();
}
BOOL CAN_CEM::operator==(Basic_CAN_frame& cf)const{
	if(cf.getCANID() != canid)return false;
	CAN_CEM& tmp=dynamic_cast<CAN_CEM&>(cf);
	return  tmp.errinfo.data == errinfo.data ;
}
Basic_CAN_frame& CAN_CEM::operator=(Basic_CAN_frame& can){
	if(getCANID() != can.getCANID()){
		return *this;
	}
	CAN_CEM & cem = dynamic_cast<CAN_CEM&> (can);
	index =cem.index;
	errinfo.data = cem.errinfo.data;
	return *this;
}
//======================== 多帧传输 =========================//
//--------------- TP_START --------------------//
BOOL CAN_TP_START::get_data(Table_field &tf){
	if(tf.getByteHex(LENGTH_BASE)== 8 && tf.getNumField()>= 15){
		console = tf.getByteHex( DATA_BASE + 0 ) ;
		length = tf.getByteHex(DATA_BASE + 1 ) + (tf.getByteHex(DATA_BASE + 2 )<< 8 );
		packets = tf.getByteHex(DATA_BASE + 3 ) ;
		reserve = tf.getByteHex(DATA_BASE + 4);
		PGN = tf.getByteHex(DATA_BASE + 6) ;
		return true;
	}
	return false;
}
string CAN_TP_START::get_detail()const{
	ostringstream oss;
	if(console == TP_START_CONSOLE)
		oss<<"多帧传输总字节数："<<length<<",数据包数:"<<(UINT)packets<<".传输包的PGN为0x00"<<hex<<setw(2)<<PGN<<"00.";
	else
		oss<<"解析错误，并非多包数据请求帧！！！";
	return oss.str();
}
BOOL CAN_TP_START::operator==(Basic_CAN_frame& cf)const{
	if(cf.getCANID() != canid)return false;
	CAN_TP_START& tmp=dynamic_cast<CAN_TP_START&>(cf);
	return  tmp.console == console && tmp.length == length && tmp.packets == packets && tmp.PGN == PGN ;
}
Basic_CAN_frame& CAN_TP_START::operator=(Basic_CAN_frame& can){
	if(getCANID() != can.getCANID()){
		return *this;
	}
	CAN_TP_START & tp = dynamic_cast<CAN_TP_START&> (can);
	index =tp.index;
	console = tp.console;
	length = tp.length;
	packets = tp.packets;
	reserve= tp.reserve;
	PGN = tp.PGN ;
	return *this;
}
//--------------- TP_REPLY --------------------//
BOOL CAN_TP_REPLY::get_data(Table_field &tf){
	if(tf.getByteHex(LENGTH_BASE)== 8 && tf.getNumField()>= 15){
		for( UINT i = 0 ; i < 5 ; i++ ){
			compts.data[ i ] = tf.getByteHex( DATA_BASE + i ) ;
		}
		compts.data[ 5 ] = tf.getByteHex(DATA_BASE + 6) ;
		return true;
	}
	return false;
}
string CAN_TP_REPLY::get_detail()const{
	ostringstream oss;
	if(compts.reply.console == TP_REPLY_CONSOLE)
		oss<<"多帧传输开始回复帧--剩余数据包数："<<(UINT)compts.reply.packets<<".多帧传输下一数据包编号为:"<<(UINT)compts.reply.nextpacket<<".传输包的PGN为0x00"<<hex<<setw(2)<<(UINT)compts.reply.PGN<<"00.";
	else if(compts.end.console == TP_END_CONSOLE)
		oss<<"多帧传输结束帧--已传输总字节数："<<(UINT)compts.end.length<<",数据包数:"<<(UINT)compts.end.packets<<".传输包的PGN为0x00"<<hex<<setw(2)<<(UINT)compts.end.PGN<<"00.";
	else
		oss<<"解析错误，并非多包数据回复帧！！！";
	return oss.str();
}
BOOL CAN_TP_REPLY::operator==(Basic_CAN_frame& cf)const{
	if(cf.getCANID() != canid)return false;
	CAN_TP_REPLY& tmp=dynamic_cast<CAN_TP_REPLY&>(cf);
	for(UINT i = 0 ; i < 6 ; i++)
		if(tmp.compts.data[ i ] != compts.data[ i ])
			return false ;
	return true;
}
Basic_CAN_frame& CAN_TP_REPLY::operator=(Basic_CAN_frame& can){
	if(getCANID() != can.getCANID()){
		return *this;
	}
	CAN_TP_REPLY & tp = dynamic_cast<CAN_TP_REPLY&> (can);
	index =tp.index;
	for(UINT i = 0 ; i < 6 ; i++){
		compts.data[i] = tp.compts.data[i];
	}
	return *this;
}
//--------------- TP_DATA --------------------//
BOOL CAN_TP_DATA::get_data(Table_field &tf){
	if(tf.getByteHex(LENGTH_BASE)== 8 && tf.getNumField()>= 15){
		packet = tf.getByteHex(DATA_BASE);
		for( UINT i = 0 ; i < 7; i++ ){
			data[ i ] = tf.getByteHex( DATA_BASE + i + 1 ) ;
		}
		return true;
	}
	return false;
}
string CAN_TP_DATA::get_detail()const{
	ostringstream oss;
	oss<< "当前数据包位多帧传输第"<<(UINT)packet<<"包.";
	return oss.str();
}
BOOL CAN_TP_DATA::operator==(Basic_CAN_frame& cf)const{
	if(cf.getCANID() != canid)return false;
	CAN_TP_DATA& tmp=dynamic_cast<CAN_TP_DATA&>(cf);
	for(UINT i = 0 ; i < 7 ; i++)
		if(tmp.data[ i ] != data[ i ])
			return false ;
	return tmp.packet==packet;
}
Basic_CAN_frame& CAN_TP_DATA::operator=(Basic_CAN_frame& can){
	if(getCANID() != can.getCANID()){
		return *this;
	}
	CAN_TP_DATA & tp = dynamic_cast<CAN_TP_DATA&> (can);
	index =tp.index;
	packet = tp.packet;
	for(UINT i = 0 ; i < 7 ; i++){
		data[i] = tp.data[i];
	}
	return *this;
}
//=============================== Mulit_frame ================//
BOOL Multi_frame::operator==(Multi_frame & mf)const{
	if(mf.PGN != PGN || frames.size()!= mf.frames.size() ){
		return false;
	}
	for(UINT i = 0 ; i < frames.size(); i++){
		if(!((*frames[ i ]) == (*mf.frames[ i ])))
			return false;
	}
	return true;
}
Multi_frame& Multi_frame::operator=(const Multi_frame&mf){
	PGN = mf.PGN;
	state = mf.state;
	frames.resize(mf.frames.size());
	for(UINT i = 0 ; i < mf.frames.size() ; i++){
		frames[i] = mf.frames[i];
	}
	return *this;
}
BYTE Multi_frame::getByte(UINT i)const{
	UINT r = i / 7 + 2, c = i % 7 ;
	if( frames.size() <= r )
		return 0xff;
	return dynamic_cast<CAN_TP_DATA*>(frames[ r ])->data[c];
}
WORD Multi_frame::getWord(UINT i)const{
	UINT r = (i + 1) / 7 + 2;
	if(frames.size() <= r){
		return 0xffff;
	}
	return getByte( i ) + (getByte( i + 1 )<<8);
}
DWORD Multi_frame::getDWord(UINT i)const{
	UINT r = (i + 3) / 7 + 2;
	if(frames.size() <= r){
			return 0xffffffff;
	}
	return getWord(i) + (getWord( i + 2) << 16 );
}
