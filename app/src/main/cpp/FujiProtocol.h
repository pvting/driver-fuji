/*
 * FujiProtocol.h
 *
 *  Created on: Jan 3, 2017
 *      Author: bl
 */

#ifndef FUJIPROTOCOL_H_
#define FUJIPROTOCOL_H_

#include "serialPort.h"
#include <iostream>		//引用相关输出头文件
#include <iomanip> //引用格式控制头文件
#include <string.h>		//strlen
#include <sys/time.h>		//gettimeofday
#include <stdio.h>		//fprintf  freopen
#include <pthread.h>
#include <unistd.h>  	//	sleep
#include <semaphore.h>

#define STX 0x02  // 起始字符
#define BUF_SZ 800
#define UPDATE_INI_SIZE 103
#define ACK 0xA0    // ACK cmd
#define NAK 0xA1    // NAK cmd				& NAK事件
#define POLL 0xA2    // POLL cmd
#define SERVICE_RPT 0xE0    // Service_RPT cmd
#define ACTION_RPT 0xE1    // Action_RPT cmd
#define INFO_RPT 0xE2    // Info_RPT cmd
#define STATUS_RPT 0xE3    // Status_RPT cmd
#define BUTTON_RPT 0xE4    // Button_RPT cmd
#define VENDOUT_RPT 0xE5   // Vendout cmd		& 出货报告事件
#define GET_UPDATA 0xE7    // Get_Updata cmd
#define COST_RPT 0xEC   // Cost_RPT cmd
#define CONFIG_IND 0xF0   // Config_IND cmd
#define CONTROL_IND 0xF1    // Control_IND cmd
#define GET_INFO 0xF2    // Get_Info cmd
#define GET_STATUS 0xF3    // Get_Status cmd
#define VENDOUT_IND 0xF4    // Vendout_IND cmd
#define UPDATA_DAT 0xF5    // Updata_DAT cmd
#define COST_IND 0xFB    // Cost_IND cmd

#define ACTION_RPT_VMCSTARTED 11 	//VMC初始化完成事件
#define ACTION_RPT_VENDOUT 12 	//出货完成事件
#define BUTTON_RPT_SELECTGOODS 13 	//用户按钮选货报告
#define BUTTON_RPT_RETURNCOIN 14 	//退币杆
#define DOOR_OPENED 15 	//门开事件
#define VENDOUT_READY 16 	//出货查询结果：可以出货
#define VENDOUT_NOTREADY 17	//出货查询结果：不可以出货
#define COST_RPT_SUCCEED 18 	//扣费成功事件
#define COST_RPT_FAILED 19 	//扣费失败事件
#define INSTRUCTIONS_ERROR 20 	//指令错误
#define VMC_STATUS_RPT 21		//VMC系统状态报告
#define VMC_ERROR_RPT 22		//VMC故障状态
#define VMC_SOLDOUT_RPT 23	//货道售空状态
#define VMC_INPUTCASH_RPT 24 	//用户投币余额
#define VMC_TEMP_RPT 25 	//库内温度
#define VMC_CLMSALESTATUS_RPT 26	//货道可销售状态
#define VMC_CONFIG_RPT 27		//系统配置报告
#define VMC_POWER_RPT 28		//VMC能力报告
#define VMC_TIME_RPT 29	//VMC当前时间
#define VMC_HCMODE_RPT 30	//货道冷热模式
#define VMC_COLUMNPRICE_CASH_RPT 31	//货道现金单价报告
#define VMC_COLUMNPRICE_NOTCASH_RPT 32	//货道非现金单价报告
#define VMC_GOODS_SPID_RPT 33	//货道商品编码报告
#define VMC_SALES_MAXNUM_RPT 34	//连续购买个数报告
#define VMC_SOFT_VER_RPT 35	//智能控制盒软件版本报告
#define VMC_MACHINEID_RPT 36	//机器编号报告
#define VMC_ROOM_HCMODE_RPT 37	//库内冷热模式
#define VMC_TOTALSALESINFORMATION_RPT 38		//累计销售信息
#define VMC_EACHCOLUMN_SALES_NUM_RPT 39   //各货道总计销售个数
#define VMC_EACHCOLUMN_SALES_MONEY_RPT 40   //各货道总计销售金额
#define VMC_MASTER_VER_RPT 41		//主控软件版本报告
#define VMC_LAMP_MODE_RPT 42		//照明动作模式报告
#define VMC_HEATER_TIME_RPT 43		//加热节电时间带报告
#define VMC_COMPRESSOR_TIME_RPT 44  //制冷节电时间带报告
#define VMC_HEATER_ON_RPT 45		//加热库内ON/OFF温度报告
#define VMC_COMPRESSOR_ON_RPT	46	//制冷库内ON/OFF温度报告
#define DOOR_CLOSED 47 	//门关事件
#define SERIAL_ERROR_EVENT 48 	//串口通信异常事件
#define SERIAL_ALIVE_EVENT 49	//串口通信心跳正常
#define ALIVE_OVERTIME 20		//判断心跳间隔20秒

using namespace std;
void* threadDaemon(void* arg);

class FujiProtocol: private serialPort
{
public:
	FujiProtocol();
	virtual ~FujiProtocol();
	void startProtocol();
	bool isVendingMachineAlive;
	int getEvent();
	void getVendoutRpt(unsigned char * gVR);
	int getSelectedColumnId();
	void vendoutIndRequest(int payment,int columnID);
	void vendoutIndAction(int payment,int columnID);
	void lightButton(int columnID);
	void returnCoin();
	void charge(int price);
	void getStatus(int i_type);
	void getVmcStatusRpt(unsigned char * gVSR);
	int getVmcErrorRpt(unsigned char * gVER);
	int getVmcSoldoutRpt(unsigned char * gVSOR);
	short getInputCash();
	short getColumnNum();
	int getVmcClmsalestatusRpt(unsigned char * gVCR);
	int getVmcColumnPrice_Cash(unsigned char * gVCC);
	int getVmcColumnPrice_notCash(unsigned char * gVCNC);
	int getSalesMaxNum();
	void getInfo(int i_type);
	void setSalesMaxNum(int maxnum);
	void setMachineId(unsigned char const* vmcId);
	void getMachineId(unsigned char * gMI);
	void setColumnPrice_Cash(unsigned char const* colprice_cash);
	void setColumnPrice_notCash(unsigned char const* colprice_notcash);
	int getRpt(int m_type, unsigned char *gR);
	void setEvent(int Id);

private:
	bool debug;
	unsigned char frame_receive[BUF_SZ];  // 数据接收buf
	unsigned char frame_send[BUF_SZ];  // 数据send buf
	unsigned char data_receive[BUF_SZ];   //value of DATA
	unsigned char data_send[BUF_SZ];   //value of DATA
	unsigned int frame_len;  // 数据buf length
	unsigned int data_receive_len;  // 数据DATA receive length
	unsigned int data_send_len;  // 数据DATA send length
	unsigned int frame_send_len;  // 数据frame send length
	unsigned char sn;	//vaule of SN
	unsigned char len[2];	//vaule of LEN,  type char
	unsigned short int_len;	//vaule of LEN,  type int
	unsigned char cmd;	//vaule of CMD
	unsigned char fcc[2];	//vaule of FCC, type char
	unsigned char Standby;	//
	unsigned int int_fcc;  //vaule of FCC, type int

	bool getMachineIdTag;
	bool getVmcStatusTag;
	bool getVmcColumnStatusTag;

	unsigned char vendoutRptReceived[49];   //
	unsigned char vmcStatusRpt[33];		//VMC系统状态报告
	unsigned char vmcErrorRpt[40];	//VMC故障状态
	int vmcErrorRpt_len;  //
	unsigned char vmcSoldoutRpt[80];	//货道售空状态
	int vmcSoldoutRpt_len;  //
	int columnPrice_len;	//货道价格长度
	unsigned char vmcTempRpt[5];	//库内温度
	unsigned char vmcClmsalestatusRpt[80];	//货道可销售状态
	int vmcClmsalestatusRpt_len;  //
	unsigned char vmcConfigRpt[47];	//系统配置报告
	unsigned char vmcPowerRpt[5];	//VMC能力报告
	unsigned char vmcTimeRpt[17];	//VMC当前时间
	unsigned char vmcHcModeRpt[80];	//货道冷热模式
	int vmcHcModeRpt_len;  //
	unsigned char vmcGoodsSpidRpt[80];	//货道商品编码报告
	int vmcGoodsSpidRpt_len;  //
	unsigned char totalSalesInformationRpt[41];	//累计销售信息
	unsigned char eachColumnSalesNumRpt[200];	//各货道总计销售个数
	int eachColumnSalesNumRpt_len;  //
	unsigned char eachColumnSalesMoneyRpt[200];	//各货道总计销售金额
	int eachColumnSalesMoneyRpt_len;  //
	unsigned char vmcSoftVerRpt[40];	//智能控制盒软件版本报告
	unsigned int vmcSoftVerRpt_len;  //
	unsigned char vmcRoomHcmodeRpt[5];	//库内冷热模式报告
	unsigned char vmcMasterVerRpt[10];	//主控软件版本报告
	unsigned int vmcMasterVerRpt_len;  //
	unsigned char lampmodeRpt[2];	//照明动作模式报告
	unsigned char vmcHeaterTimeRpt[21];		//加热节电时间带报告
	unsigned char vmcCompressorTimeRpt[21];		//制冷节电时间带报告
	unsigned char vmcHeaterOnRpt[4];	//加热库内ON/OFF温度报告
	unsigned char vmcCompressorOnRpt[4];	//制冷库内ON/OFF温度报告
	int sales_maxnum;  //	连续购买个数
	int sales_maxnum_toBeSet;
	short inputCash;//input cash	投币信息保存在VMC内存中，即使串口通信断掉余额依然记忆，除非VMC重启会清零)
	short columnNum;  // 货道数量
	unsigned char columnPrice_cash[150];  // 货道现金单价,设置货道现金单价时，非现金单价也会跟着变,机器重启不会恢复默认
	unsigned char columnPrice_cash_toBeSet[150];
	unsigned char columnPrice_notCash[150];  // 货道非现金单价
	unsigned char columnPrice_notCash_toBeSet[150];
	unsigned char machineId[8];  // 机器编号
	unsigned char machineId_toBeSet[8];
	int eventID;		//eventID
	int selectedColumnId;		//货道ID
	int payment_method;			//支付方式:  1.现金   2.非现金
	char timebuf[64];		//存储当前时间 2017-01-09 18:25:20
	int int_charge;  //PC指示扣费金额
	int getStatusType;
	int getInfoType;
	int PollTag;
	sem_t event_sem_id;  // 事件信号量
	struct timeval tv;

	void frameSendSolve(unsigned char cmd, unsigned char*data,
			unsigned int datasize);
	bool const checkInstructions();

	void printLog(unsigned char * data, unsigned int data_size, int tag);
	void getCurrentTime();
	void returnUpdataDat(unsigned char char_type);
	void returnControlInd(unsigned char char_type);
	void returnVendoutIndRequest();
	void returnVendoutIndAction();
	void returnCostInd();
	void returnGetStatus();
	void returnGetInfo();
	void returnConfigIndSetMachineId();
	void returnConfigIndSetColumnPrice_Cash();
	void returnConfigIndSetColumnPrice_notCash();
	void returnConfigIndSetSalesMaxNum();
	void returnAck();
	void returnNak();
	void clearData();
};

extern FujiProtocol fp;

#endif /* FUJIPROTOCOL_H_ */
