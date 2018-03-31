#ifndef CANDATA_H
#define CANDATA_H

#include"table field.h"
#include"datatype.h"
#define CHM_CANID 0x1826f456
#define BHM_CANID 0x182756F4
#define CRM_CANID 0x1801F456
#define CTS_CANID 0x1807F456
#define CML_CANID 0x1808F456
#define BRO_CANID 0x100956F4
#define CRO_CANID 0x100AF456
#define BCL_CANID 0x181056F4
#define CCS_CANID 0x1812F456
#define BSM_CANID 0x181356F4
#define BST_CANID 0x101956F4
#define CST_CANID 0x101AF456
#define BSD_CANID 0x181C56F4
#define CSD_CANID 0x181DF456
#define BEM_CANID 0x081E56F4
#define CEM_CANID 0x081FF456
#define DM1_CANID 0x1820F456
#define DM3_CANID 0x1822F456
#define TP_START_CANID 0x1CEC56F4
#define TP_REPLY_CANID 0x1CECF456
#define TP_DATA_CANID  0x1CEB56F4



#define BRM_PGN	0x02
#define BCP_PGN 0x06
#define BCS_PGN 0x11
#define BMV_PGN 0x15
#define BMT_PGN 0x16
#define BSP_PGN 0x17

#define STATE_HAND_SHK 		0
#define STATE_REC			1
#define STATE_CONFIGURE 	2
#define STATE_CHARGE		3
#define STATE_CHARGE_END	4
#define STATE_FAULT			5
#define STATE_TP			6

extern UINT DATA_BASE, CANID_BASE,LENGTH_BASE;
class Basic_CAN_frame{
	friend class Multi_frame;
	friend class CAN_BRM;
	friend class CAN_BCS;
	friend class CAN_BCP;
protected:
	DWORD canid;
	UINT index;
	UINT state;
	static CONST_CHARPTR states[7];
public:
	CONST_CHARPTR getState(){return states[state];}
	DWORD getCANID(){return canid;}
	static Basic_CAN_frame *create_frame(DWORD canid);
	virtual bool direct()const=0;
	VOID setIndex(UINT i){ index = i;}
	UINT getIndex(){ return index; }
	Basic_CAN_frame(DWORD id , UINT s):canid(id),index(0),state(s){}
	BOOL operator!=(Basic_CAN_frame&cf){return !(*this==cf);}
	virtual string typeName()const=0;
	virtual Basic_CAN_frame& operator=(Basic_CAN_frame&)=0;
	virtual BYTE getPGN(){return 0xff;}
	virtual BYTE getConsole(){return 0xff;}
	virtual BOOL get_data(Table_field &)=0;
	virtual string get_detail()const=0;
	virtual CONST_CHARPTR get_frame_name()const=0;
	virtual BOOL operator==(Basic_CAN_frame& cf)const{ return canid == cf.canid;}
	virtual ~Basic_CAN_frame(){}
};
//--------- 握手阶段 ----------//
class CAN_CHM: public Basic_CAN_frame{
	BYTE vmajor;
	WORD vminor;
public:
	virtual CONST_CHARPTR get_frame_name()const{return "充电桩握手报文";}
	virtual bool direct()const{return true;}
	CAN_CHM():Basic_CAN_frame(CHM_CANID,STATE_HAND_SHK),vmajor(0xff),vminor(0xffff){}
	virtual BOOL get_data(Table_field &);
	virtual string typeName()const{return "CHM";}
	virtual string get_detail()const;
	virtual BOOL operator==(Basic_CAN_frame& cf)const;
	virtual Basic_CAN_frame& operator=(Basic_CAN_frame&);
	~CAN_CHM(){}

};

class CAN_BHM:public Basic_CAN_frame{
	WORD hvol ; //最高允许电压：[1,2]0.1V/位，基数为0V
public:
	virtual CONST_CHARPTR get_frame_name()const{return "BMS握手报文";}
	CAN_BHM():Basic_CAN_frame(BHM_CANID,STATE_HAND_SHK),hvol(0xff){}
	virtual bool direct()const{return false;}
	virtual BOOL get_data(Table_field &);
	virtual string get_detail()const;
	virtual string typeName()const{return "BHM";}
	virtual BOOL operator==(Basic_CAN_frame& cf)const;
	virtual Basic_CAN_frame& operator=(Basic_CAN_frame&);
	~CAN_BHM(){}
};
//--------- 辨识阶段 ------------//
class CAN_CRM:public Basic_CAN_frame{
	BYTE rec; //辨识结果
	DWORD mnum; //充电机编号
	DWORD mloc ; //充电机地区编号
public:
	virtual CONST_CHARPTR get_frame_name()const{return "充电桩辨识报文";}
	virtual bool direct()const{return true;}
	virtual string typeName()const{return "CRM";}
	CAN_CRM():Basic_CAN_frame(CRM_CANID,STATE_REC),rec(0xff),mnum(0xffffffff),mloc(0xffff){}
	virtual BOOL get_data(Table_field &);
	virtual string get_detail()const;
	virtual BOOL operator==(Basic_CAN_frame& cf)const;
	virtual Basic_CAN_frame& operator=(Basic_CAN_frame&);
	virtual ~CAN_CRM(){}
};

//---------- 配置阶段 -----------//
class CAN_CTS:public Basic_CAN_frame{
	BYTE	second,minute,hour,day,month;
	WORD year;//BCD码
public:
	virtual CONST_CHARPTR get_frame_name()const{return "充电桩最大输出能力报文";}
	virtual string typeName()const{return "CTS";}
	virtual bool direct()const{return true;}
	CAN_CTS():Basic_CAN_frame(CTS_CANID,STATE_CONFIGURE),second(0xff),minute(0xff),hour(0xff),day(0xff),month(0xff),year(0xffff){}
	virtual BOOL get_data(Table_field &);
	virtual string get_detail()const;
	virtual BOOL operator==(Basic_CAN_frame& cf)const;
	virtual Basic_CAN_frame& operator=(Basic_CAN_frame&);
	~CAN_CTS(){}
};
class CAN_CML:public Basic_CAN_frame{
	WORD houtvol,loutvol,houtcur,loutcur;
public:
	virtual CONST_CHARPTR get_frame_name()const{return "充电桩时间同步报文";}
	virtual bool direct()const{return true;}
	virtual string typeName()const{return "CML";}
	CAN_CML():Basic_CAN_frame(CML_CANID,STATE_CONFIGURE),houtvol(0xffff),loutvol(0xffff),houtcur(0xffff),loutcur(0xffff){}
	virtual BOOL get_data(Table_field &);
	virtual string get_detail()const;
	virtual BOOL operator==(Basic_CAN_frame& cf)const;
	virtual Basic_CAN_frame& operator=(Basic_CAN_frame&);
	~CAN_CML(){}
};
class CAN_BRO:public Basic_CAN_frame{
	BYTE rec;
public:
	virtual CONST_CHARPTR get_frame_name()const{return "BMS准备就绪报文";}
	virtual string typeName()const{return "BRO";}
	CAN_BRO():Basic_CAN_frame(BRO_CANID,STATE_CONFIGURE),rec(0xff){}
	virtual BOOL get_data(Table_field &);
	virtual string get_detail()const;
	virtual bool direct()const{return false;}
	virtual BOOL operator==(Basic_CAN_frame& cf)const;
	virtual Basic_CAN_frame& operator=(Basic_CAN_frame&);
	~CAN_BRO(){}
};
class CAN_CRO:public Basic_CAN_frame{
	BYTE rec;
public:
	virtual CONST_CHARPTR get_frame_name()const{return "充电桩准备就绪报文";}
	CAN_CRO():Basic_CAN_frame(CRO_CANID,STATE_CONFIGURE),rec(0xff){}
	virtual BOOL get_data(Table_field &);
	virtual bool direct()const{return true;}
	virtual string get_detail()const;
	virtual string typeName()const{return "CRO";}
	virtual BOOL operator==(Basic_CAN_frame& cf)const;
	virtual Basic_CAN_frame& operator=(Basic_CAN_frame&);
	~CAN_CRO(){}
};
//---------- 充电阶段 ----------//
class CAN_BCL:public Basic_CAN_frame{
	WORD nvol,ncur;
	BYTE cmod;
public:
	virtual CONST_CHARPTR get_frame_name()const{return "BMS充电需求报文";}
	virtual string typeName()const{return "BCL";}
	CAN_BCL():Basic_CAN_frame(BCL_CANID,STATE_CHARGE),nvol(0xffff),ncur(0xffff),cmod(0xff){}
	virtual BOOL get_data(Table_field &);
	virtual bool direct()const{return false;}
	virtual string get_detail()const;
	virtual BOOL operator==(Basic_CAN_frame& cf)const;
	virtual Basic_CAN_frame& operator=(Basic_CAN_frame&);
	~CAN_BCL(){}
};
class CAN_CCS:public Basic_CAN_frame{
	WORD ovol,ocur,chargetime;
	BYTE chargeallow;
public:
	virtual CONST_CHARPTR get_frame_name()const{return "充电桩充电状态报文";}
	virtual string typeName()const{return "CCS";}
	virtual bool direct()const{return true;}
	CAN_CCS():Basic_CAN_frame(CCS_CANID,STATE_CHARGE),ovol(0xffff),ocur(0xffff),chargetime(0xffff),chargeallow(0xff){}
	virtual BOOL get_data(Table_field &);
	virtual string get_detail()const;
	virtual BOOL operator==(Basic_CAN_frame& cf)const;
	virtual Basic_CAN_frame& operator=(Basic_CAN_frame&);
	~CAN_CCS(){}
};
class CAN_BSM:public Basic_CAN_frame{
	BYTE maxvolid,maxtem,maxtemid,mintem,mintemid;
	union{
		struct{
			WORD volstate   		:2; //电压状态
			WORD chargestate 		:2; //荷电状态
			WORD curstate			:2; //过流状态
			WORD temstate			:2; //温度状态
			WORD insultstate		:2; //绝缘状态
			WORD OCS				:2; //输出连接器状态
			WORD CA					:2; //充电允许
		}compts;
		WORD data;
	}chargestate;
public:
	virtual CONST_CHARPTR get_frame_name()const{return "蓄电池状态信息报文";}
	virtual string typeName()const{return "BSM";}
	CAN_BSM():Basic_CAN_frame(BSM_CANID,STATE_CHARGE),maxvolid(0xff),maxtem(0xff),maxtemid(0xff),mintem(0xff),mintemid(0xff){chargestate.data=0xffff;}
	virtual BOOL get_data(Table_field &);
	virtual bool direct()const{return false;}
	virtual string get_detail()const;
	virtual BOOL operator==(Basic_CAN_frame& cf)const;
	virtual Basic_CAN_frame& operator=(Basic_CAN_frame&);
	~CAN_BSM(){}
};
class CAN_BST:public Basic_CAN_frame{
	union{
		struct{
			BYTE stopbySOC  		:2;//达到所需SOC值
			BYTE stopbyVT		:2;//总电压达到设定值
			BYTE stopbyVN		:2;//电压达到当前设定值
			BYTE stopbyCM			:2;// 充电机主动中止
		} compts;
		BYTE data;
	}stopinfo;
	union{
		struct{
			WORD insultFault		:2;//绝缘故障
			WORD OCTFault			:2;//输出连接器过温
			WORD BMStemFault		:2;//BMS、输出连接器过温
			WORD CCFault			:2;//充电连接器故障
			WORD BTFault			:2;//电池组温度过高
			WORD HVolFault			:2;//高压继电器故障
			WORD T2VolFault			:2;//检测点2电压检测故障
			WORD OtherFault			:2;//其他故障
		}compt;
		BYTE data[2];
	}faultinfo;
	union{
		struct{
			BYTE curfault			:2; //电流过大
			BYTE volfault			:2; //电压异常
			BYTE					:4;
		}compt;
		BYTE data;
	}stopfaultinfo;
public:
	virtual CONST_CHARPTR get_frame_name()const{return "BMS中止充电报文";}
	virtual string typeName()const{return "BST";}
	virtual bool direct()const{return false;}
	CAN_BST():Basic_CAN_frame(BST_CANID,STATE_CHARGE){stopinfo.data=0xff;faultinfo.data[0]=0xff;faultinfo.data[1]=0xff;stopfaultinfo.data=0xff;}
	virtual BOOL get_data(Table_field &);
	virtual string get_detail()const;
	virtual BOOL operator==(Basic_CAN_frame& cf)const;
	virtual Basic_CAN_frame& operator=(Basic_CAN_frame&);
	~CAN_BST(){}
};

class CAN_CST:public Basic_CAN_frame{
	union{
		struct{
			BYTE stopbyCM  			:2;//达到充电机设定条件中止
			BYTE stopbyMan			:2;//人工中止
			BYTE stopbyFault		:2;//故障中止
			BYTE stopbyBMS			:2;// BMS主动中止
		} compts;
		BYTE data;
	}stopinfo;
	union{
		struct{
			WORD HTempFault			:2;//过温故障
			WORD CCFault			:2;//充电连接器故障
			WORD CMtemFault			:2;//充电机内部过温
			WORD CurFault			:2;//电流不能传送
			WORD CSFault			:2;//充电机急停故障
			WORD OtherFault			:2;//其他故障
			WORD 					:4;
		}compt;
		BYTE data[2];
	}faultinfo;
	union{
		struct{
			BYTE curfault			:2; //电流不匹配
			BYTE volfault			:2; //电压异常
			BYTE 					:4;
		}compt;
		BYTE data;
	}stopfaultinfo;
public:
	virtual CONST_CHARPTR get_frame_name()const{return "充电桩中止充电报文";}
	virtual string typeName()const{return "CST";}
	virtual bool direct()const{return true;}
	CAN_CST():Basic_CAN_frame(CST_CANID,STATE_CHARGE){stopinfo.data=0xff;faultinfo.data[1]=0xff;faultinfo.data[0]=0xff;stopfaultinfo.data=0xff;}
	virtual BOOL get_data(Table_field &);
	virtual string get_detail()const;
	virtual BOOL operator==(Basic_CAN_frame& cf)const;
	virtual Basic_CAN_frame& operator=(Basic_CAN_frame&);
	~CAN_CST(){}
};
//----------- 充电结束阶段	-------------//
class CAN_BSD:public Basic_CAN_frame{
	BYTE SOC; //中止充电荷电状态
	WORD LVol,HVol;//电池单体最低和最高电压
	BYTE LTem,HTem;//电池单体最低和最高温度
public:
	virtual CONST_CHARPTR get_frame_name()const{return "BMS统计报文";}
	CAN_BSD():Basic_CAN_frame(BSD_CANID,STATE_CHARGE_END),SOC(0xff),LVol(0xffff),HVol(0xffff),LTem(0xff),HTem(0xff){}
	virtual BOOL get_data(Table_field &);
	virtual string get_detail()const;
	virtual BOOL operator==(Basic_CAN_frame& cf)const;
	virtual string typeName()const{return "BSD";}
	virtual Basic_CAN_frame& operator=(Basic_CAN_frame&);
	virtual bool direct()const{return false;}
	~CAN_BSD(){}
};
class CAN_CSD:public Basic_CAN_frame{
	WORD chargetime,outenergy; //累计充电时间,输出能量
	DWORD	CM_id;//充电机编号
public:
	virtual CONST_CHARPTR get_frame_name()const{return "充电桩统计报文";}
	virtual string typeName()const{return "CSD";}
	CAN_CSD():Basic_CAN_frame(CSD_CANID,STATE_CHARGE),chargetime(0xffff),outenergy(0xffff),CM_id(0xffffffff){}
	virtual BOOL get_data(Table_field &);
	virtual string get_detail()const;
	virtual BOOL operator==(Basic_CAN_frame& cf)const;
	virtual bool direct()const{return true;}
	virtual Basic_CAN_frame& operator=(Basic_CAN_frame&);
	~CAN_CSD(){}
};
//------------- 错误报文 ------------//
class CAN_BEM:public Basic_CAN_frame{
	union{
		struct{
			DWORD CRM00_OV  		:2;//CRM=0x00超时
			DWORD CRMAA_OV  		:2;//CRM=0xAA超时
			DWORD					:4;
			DWORD CML_OV	  		:2;//CML超时
			DWORD CRO_OV	  		:2;//CRO超时
			DWORD					:4;
			DWORD CCS_OV			:2;//CCS超时
			DWORD CST_OV			:2;//CST超时
			DWORD					:4;
			DWORD CSD_OV			:2;//CSD超时
			DWORD 					:6;
		} compts;
		DWORD data;
	}errinfo;
public:
	virtual CONST_CHARPTR get_frame_name()const{return "BMS错误报文";}
	virtual string typeName()const{return "BEM";}
	CAN_BEM():Basic_CAN_frame(BEM_CANID,STATE_FAULT){errinfo.data=0xffffffff;}
	virtual BOOL get_data(Table_field &);
	virtual bool direct()const{return false;}
	virtual string get_detail()const;
	virtual BOOL operator==(Basic_CAN_frame& cf)const;
	virtual Basic_CAN_frame& operator=(Basic_CAN_frame&);
	~CAN_BEM(){}
};
class CAN_CEM:public Basic_CAN_frame{
	union{
		struct{
			DWORD BRM_OV  		:2;//BRM超时
			DWORD					:6;
			DWORD BCP_OV	  		:2;//BCP超时
			DWORD BRO_OV	  		:2;//BRO超时
			DWORD					:4;
			DWORD BCS_OV			:2;//BCS超时
			DWORD BCL_OV			:2;//BCL超时
			DWORD BST_OV			:2;//CST超时
			DWORD 					:2;
			DWORD BSD_OV			:2;//BSD超时
			DWORD 					:6;
		} compts;
		DWORD data;
	}errinfo;
public:
	virtual CONST_CHARPTR get_frame_name()const{return "充电桩错误报文";}
	virtual string typeName()const{return "CEM";}
	virtual bool direct()const{return true;}
	CAN_CEM():Basic_CAN_frame(CEM_CANID,STATE_FAULT){errinfo.data=0xffffffff;}
	virtual BOOL get_data(Table_field &);
	virtual string get_detail()const;
	virtual BOOL operator==(Basic_CAN_frame& cf)const;
	virtual Basic_CAN_frame& operator=(Basic_CAN_frame&);
	~CAN_CEM(){}
};
//--------------- 多帧传输 ---------------//
#define TP_START_CONSOLE 	0x10
#define TP_REPLY_CONSOLE 	0x11
#define TP_END_CONSOLE		0x13
class CAN_TP_START:public Basic_CAN_frame{
	friend class Multi_frame;
	friend class CAN_BRM;
	friend class CAN_BCS;
	friend class CAN_BCP;
		BYTE console;
		WORD length;
		BYTE packets;
		BYTE reserve;
		BYTE PGN;
public:
	virtual CONST_CHARPTR get_frame_name()const{return "多帧传输请求帧";}
	virtual string typeName()const{return "TP_START";}
	virtual bool direct()const{return false;}
	CAN_TP_START():Basic_CAN_frame(TP_START_CANID,STATE_TP),console(0xff),length(0xffff),packets(0xff),reserve(0xff),PGN(0xff){}
	virtual BOOL get_data(Table_field &);
	virtual string get_detail()const;
	virtual BOOL operator==(Basic_CAN_frame& cf)const;
	virtual Basic_CAN_frame& operator=(Basic_CAN_frame&);
	virtual BYTE getPGN(){return PGN;}
	virtual BYTE getConsole(){return console;}
	~CAN_TP_START(){}
};
class CAN_TP_REPLY:public Basic_CAN_frame{
	friend class Multi_frame;
	friend class CAN_BRM;
	friend class CAN_BCS;
	friend class CAN_BCP;
	union{
		struct{
			BYTE console;
			BYTE packets;
			BYTE nextpacket;
			WORD reserve;
			BYTE PGN;
		}reply;
		struct{
			BYTE console;
			WORD length;
			BYTE packets;
			BYTE reserve;
			BYTE PGN;
		}end;
		BYTE data[ 6 ];
	}compts;
public:
	virtual CONST_CHARPTR get_frame_name()const{return "多帧传输回复帧";}
	virtual string typeName()const{return "TP_REPLY(TP_END)";}
	virtual bool direct()const{return true;}
	CAN_TP_REPLY():Basic_CAN_frame(TP_REPLY_CANID,STATE_TP){for(UINT i = 0; i < 6 ; i ++ )compts.data[i]=0xff;}
	virtual BOOL get_data(Table_field &);
	virtual BYTE getPGN(){return compts.data[6];}
	virtual BYTE getConsole(){return compts.data[0];}
	virtual string get_detail()const;
	virtual BOOL operator==(Basic_CAN_frame& cf)const;
	virtual Basic_CAN_frame& operator=(Basic_CAN_frame&);
	~CAN_TP_REPLY(){}
};
class CAN_TP_DATA:public Basic_CAN_frame{
	friend class Multi_frame;
	friend class CAN_BRM;
	friend class CAN_BCS;
	friend class CAN_BCP;
protected:
		BYTE packet;
		BYTE data[7];
public:
	virtual CONST_CHARPTR get_frame_name()const{return "多帧传输数据帧";}
	virtual string typeName()const{return "TP_DATA";}
	CAN_TP_DATA():Basic_CAN_frame(TP_DATA_CANID,STATE_TP),packet(0xff){}
	virtual BOOL get_data(Table_field &);
	virtual bool direct()const{return false;}
	virtual string get_detail()const;
	virtual BOOL operator==(Basic_CAN_frame& cf)const;
	virtual Basic_CAN_frame& operator=(Basic_CAN_frame&);
	~CAN_TP_DATA(){}
};
//===================== 多帧传输数据 ===================//

class Multi_frame{		//多帧传输
protected:
	BYTE PGN;
	UINT state;
	vector<Basic_CAN_frame*> frames;
public:
#define BRM_TP_SIZE 10
#define BCP_TP_SIZE 5
#define BCS_TP_SIZE 5
	CONST_CHARPTR getState(){return Basic_CAN_frame::states[state];}
	Multi_frame(CAN_TP_START* tp,UINT state):PGN(tp->PGN),state(state){frames.push_back(tp);}
	Multi_frame(BYTE pgn,UINT state):PGN(pgn),state(state){}
	BYTE getPGN()const{return PGN;}
	virtual string typeName()const=0;
	BYTE getByte(UINT i)const;
	WORD getWord(UINT i)const;
	DWORD getDWord(UINT i)const;
	VOID delLastFrame(){frames.pop_back();}
	vector<UINT> getIndexs(){vector<UINT> indexs;for(UINT i = 0 ; i < frames.size();i++) indexs.push_back(frames[i]->getIndex());return indexs;}
	VOID addFrame(Basic_CAN_frame* f){frames.push_back(f);}
	VOID clear(){ PGN = 0; state = 0xFFFFFFFF;frames.clear(); }
	virtual CONST_CHARPTR get_frame_name()const=0;
	virtual string get_detail()const=0;
	virtual BOOL rec_finish()const=0;
	virtual BOOL valid()const=0;
	BOOL operator==(Multi_frame & mf)const;
	Multi_frame& operator=(const Multi_frame&);
	virtual ~Multi_frame(){}
};

//------------- 辨识阶段 --------------//
class CAN_BRM:public Multi_frame{
public:
	CAN_BRM(CAN_TP_START*tp):Multi_frame(tp,STATE_REC){}
	CAN_BRM():Multi_frame(BRM_PGN,STATE_REC){}
	virtual CONST_CHARPTR get_frame_name()const{ return "BMS辨识报文"; }
	Multi_frame& operator=(const Multi_frame&mf){return Multi_frame::operator=(mf);}
	virtual string typeName()const{return "BRM";}
	virtual string get_detail()const;
	virtual BOOL rec_finish()const;
	virtual BOOL valid()const;
	virtual ~CAN_BRM(){}
};
//------------- 配置阶段 -------------//
class CAN_BCP:public Multi_frame{
public:
	CAN_BCP(CAN_TP_START*tp):Multi_frame(tp,STATE_CONFIGURE){}
	CAN_BCP():Multi_frame(BCP_PGN,STATE_CONFIGURE){}
	virtual CONST_CHARPTR get_frame_name()const{ return "电池充电参数报文"; }
	Multi_frame& operator=(const Multi_frame&mf){return Multi_frame::operator=(mf);}
	virtual string typeName()const{return "BCP";}
	virtual string get_detail()const;
	virtual BOOL rec_finish()const;
	virtual BOOL valid()const;
	virtual ~CAN_BCP(){}
};
class CAN_BCS:public Multi_frame{
public:
	CAN_BCS(CAN_TP_START*tp):Multi_frame(tp,STATE_CHARGE){}
	CAN_BCS():Multi_frame(BCS_PGN,STATE_CHARGE){}
	virtual CONST_CHARPTR get_frame_name()const{ return "电池充电总状态报文"; }
	Multi_frame& operator=(const Multi_frame&mf){return Multi_frame::operator=(mf);}
	virtual string typeName()const{return "BCS";}
	virtual string get_detail()const;
	virtual BOOL rec_finish()const;
	virtual BOOL valid()const;
	virtual ~CAN_BCS(){}
};
#endif//CANDATA_H
