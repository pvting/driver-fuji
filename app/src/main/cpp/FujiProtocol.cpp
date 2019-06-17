/*
 * FujiProtocol.cpp
 *
 *  Created on: Jan 3, 2017
 *      Author: bl
 */


#include "FujiProtocol.h"
#include "xcept.h"


FujiProtocol fp;

FujiProtocol::FujiProtocol() {
    frame_len = 0;  // 数据buf length
    data_receive_len = 0;  // 数据DATA receive length
    data_send_len = 0;  // 数据DATA send length
    frame_send_len = 0;
    inputCash = 0;
    int_fcc = 0;  //vaule of FCC, type int
    int_len = 0;
    int_charge = 0;
    sales_maxnum_toBeSet = 0;
    cmd = 0;
    getMachineIdTag = false;
    getVmcStatusTag = false;
    getVmcColumnStatusTag = false;
    isVendingMachineAlive = false;
    sales_maxnum = 1;        //连续购买个数，默认为1
    vmcSoftVerRpt_len = 0;
    vmcErrorRpt_len = 0;
    vmcMasterVerRpt_len = 0;
    vmcSoldoutRpt_len = 0;
    vmcGoodsSpidRpt_len = 0;
    columnPrice_len = 0;
    eachColumnSalesNumRpt_len = 0;
    vmcClmsalestatusRpt_len = 0;
    vmcHcModeRpt_len = 0;
    eachColumnSalesMoneyRpt_len = 0;
    Standby = 0;
    columnNum = 60;          //默认值改为60？
    sn = 0;
    getStatusType = 0;
    getInfoType = 0;
    PollTag = 0;
    eventID = 0;  //eventID
    selectedColumnId = 0;  //货道ID
    payment_method = 0;  //支付方式:  1.现金   2.非现金
    sem_init(&event_sem_id, 0, 0);

    memset(data_send, 0, BUF_SZ);
    int fd;
    char c;
    char portName[40];
    char buf[40];
    int baudRate = 0;
    int i;
    memcpy(portName, "/dev/", 5);
    if (-1 == (fd = open("/sdcard/VMC/config.ini", O_RDONLY))) {  // 读取配置文件，获取串口号，波特率
        throw vendingMachineError("open config error");
    }
    read(fd, &c, 1);
    while (c != 10) {
        i = 0;
        while (c != '=') {
            buf[i++] = c;
            read(fd, &c, 1);
        }
        i = 0;
        if (!memcmp("commport", buf, 8)) {
            read(fd, &c, 1);
            while (c != 10) {
                portName[5 + i] = c;
                i++;
                read(fd, &c, 1);
            }
            portName[5 + i] = 0;  // 串口号
        } else if (!memcmp("baudrate", buf, 8)) {
            read(fd, &c, 1);
            while (c != 10) {
                baudRate = baudRate * 10 + c - 48;  // 波特率
                read(fd, &c, 1);
            }
        } else if (!memcmp("debug", buf, 5)) {
            read(fd, &c, 1);
            i = 0;
            while (c != 10) {
                buf[i++] = c;
                read(fd, &c, 1);
            }
            if (!memcmp("true", buf, 4))
                debug = true;
            else
                debug = false;
        }
        read(fd, &c, 1);
    }
    if (-1 == close(fd)) {
        throw vendingMachineError("close config error");
    }
    serialInitialization(portName, baudRate); // 初始化串口
}

FujiProtocol::~FujiProtocol() {
}

void FujiProtocol::startProtocol() {
    pthread_t pth_daemon;       //创建监测串口心跳线程
    int ret = pthread_create(&pth_daemon, NULL, threadDaemon, NULL);
    if (ret != 0) {
        cout << "pthread_create error: error_code=" << ret << endl;
    }
    char type;
    if (debug) {
        char filename[60] = "/sdcard/VMC/log";    //新建一个log.txt
        int f_original_len = strlen(filename);
        getCurrentTime();
        memcpy(filename + f_original_len, timebuf, 19);
        memcpy(filename + f_original_len + 19, ".txt", 4);
        freopen(filename, "w", stdout);    //将cout输出重定向到此文件中
    }
    while (1) {
        readData(frame_receive, 1);  // 读串口
        if (STX == frame_receive[0]) {  // 是否是起始字节
            readData(frame_receive + 1, 2);    //LEN
            int_len = (unsigned int) frame_receive[1] * 256 +
                      (unsigned int) frame_receive[2];    //cal int_value of LEN
            readData(frame_receive + 3, int_len);    //read the left
            frame_len = int_len + 3;
            data_receive_len = int_len - 4;
            printLog(frame_receive, frame_len, 0);
            cmd = frame_receive[3];   //CMD
            sn = frame_receive[4];   //SN
            memcpy(data_receive, frame_receive + 5, data_receive_len); //get data in receive
            if (!checkInstructions()) {
                setEvent(INSTRUCTIONS_ERROR);        //指令错误事件
                returnNak();
                printLog(frame_send, frame_send_len, 1);
                clearData();
            } else {
                type = data_receive[0];
                switch (cmd) {  // 解析CMD
                    case GET_UPDATA:  // 收到Get_Updata指令
                        if (1 == type)            //type=1
                        {
                            returnUpdataDat(1);
                            printLog(frame_send, frame_send_len, 1);
                            clearData();
                            break;
                        } else                    //type=2
                        {
                            //
                            break;
                        }
                    case SERVICE_RPT:
                        if ((1 == type) && (1 == data_receive[1]))
                            setEvent(DOOR_OPENED);        //门开事件
                        else if ((1 == type) && (0 == data_receive[1]))
                            setEvent(DOOR_CLOSED);        //门关事件
                        returnAck();
                        printLog(frame_send, frame_send_len, 1);
                        clearData();
                        break;
                    case ACTION_RPT:
                        Standby = data_receive[1];
                        if (1 == type) {
                            if (1 == Standby) {
                                getMachineIdTag = true;        //VMC启动完成，主动申请一次获取机器编号、VMC系统状态、货道可销售状态。货道数量，为后续的设置价格等做准备
                            }
                        } else
                            //type=2
                            setEvent(ACTION_RPT_VENDOUT);        //出货完成事件
                        returnAck();
                        printLog(frame_send, frame_send_len, 1);
                        clearData();
                        break;
                    case INFO_RPT:
                        if (1 == type) {
                            memset(vmcConfigRpt, 0, sizeof(vmcConfigRpt));
                            memcpy(vmcConfigRpt, data_receive, data_receive_len);
                            setEvent(VMC_CONFIG_RPT);        //系统配置报告收到此事件后，PC端可以调用getRpt(1)
                        }
                        else if (2 == type) {
                            memset(vmcPowerRpt, 0, sizeof(vmcPowerRpt));
                            memcpy(vmcPowerRpt, data_receive, data_receive_len);
                            setEvent(VMC_POWER_RPT);        //VMC能力报告,收到此事件后，PC端可以调用getRpt(2)
                        }
                        else if (3 == type) {
                            memset(vmcTimeRpt, 0, sizeof(vmcTimeRpt));
                            memcpy(vmcTimeRpt, data_receive, data_receive_len);
                            setEvent(VMC_TIME_RPT);    //VMC当前时间,收到此事件后，PC端可以调用getRpt(3)
                        }
                        else if (4 == type) {
                            memset(vmcHcModeRpt, 0, sizeof(vmcHcModeRpt));
                            memcpy(vmcHcModeRpt, data_receive, data_receive_len);
                            vmcHcModeRpt_len = data_receive_len;
                            setEvent(VMC_HCMODE_RPT); //货道冷热模式,收到此事件后，PC端可以调用getRpt(4)
                        }
                        else if (5 == type) {
                            memset(columnPrice_cash, 0, sizeof(columnPrice_cash));
                            memcpy(columnPrice_cash, data_receive + 2, data_receive_len - 2);
                            columnPrice_len = data_receive_len - 2;
                            setEvent(
                                    VMC_COLUMNPRICE_CASH_RPT);    //货道现金单价报告,收到此事件后，PC端可以调用getVmcColumnPrice_Cash()
                        }
                        else if (6 == type) {
                            memset(columnPrice_notCash, 0, sizeof(columnPrice_notCash));
                            memcpy(columnPrice_notCash, data_receive + 2, data_receive_len - 2);
                            columnPrice_len = data_receive_len - 2;
                            setEvent(
                                    VMC_COLUMNPRICE_NOTCASH_RPT);    //货道非现金单价报告,收到此事件后，PC端可以调用getVmcColumnPrice_notCash()
                        }
                        else if (7 == type) {
                            memset(vmcGoodsSpidRpt, 0, sizeof(vmcGoodsSpidRpt));
                            memcpy(vmcGoodsSpidRpt, data_receive, data_receive_len);
                            vmcGoodsSpidRpt_len = data_receive_len;
                            setEvent(
                                    VMC_GOODS_SPID_RPT);    //货道商品编码报告,收到此事件后，PC端可以调用getRpt(7)
                        }
                        else if (8 == type) {
                            sales_maxnum = (int) data_receive[1];
                            setEvent(
                                    VMC_SALES_MAXNUM_RPT);    //连续购买个数报告,收到此事件后，PC端可以调用getSalesMaxNum()
                        }
                        else if (10 == type) {
                            memset(vmcSoftVerRpt, 0, sizeof(vmcSoftVerRpt));
                            memcpy(vmcSoftVerRpt, data_receive, data_receive_len);
                            vmcSoftVerRpt_len = data_receive_len;
                            setEvent(
                                    VMC_SOFT_VER_RPT);    //智能控制盒软件版本报告,收到此事件后，PC端可以调用getRpt(10)
                        }
                        else if (13 == type) {
                            memset(machineId, 0, sizeof(machineId));
                            memcpy(machineId, data_receive + 1, sizeof(machineId));
                            setEvent(VMC_MACHINEID_RPT);    //VMC机器编号报告,收到此事件后，PC端可以调用getMachineId()
                        }
                        else if (19 == type) {
                            memset(vmcRoomHcmodeRpt, 0, sizeof(vmcRoomHcmodeRpt));
                            memcpy(vmcRoomHcmodeRpt, data_receive, data_receive_len);
                            setEvent(
                                    VMC_ROOM_HCMODE_RPT);    //库内冷热模式报告,收到此事件后，PC端可以调用getRpt(19)
                        }
                        else if (20 == type) {
                            memset(totalSalesInformationRpt, 0, sizeof(totalSalesInformationRpt));
                            memcpy(totalSalesInformationRpt, data_receive, data_receive_len);
                            setEvent(
                                    VMC_TOTALSALESINFORMATION_RPT);    //累计销售信息,收到此事件后，PC端可以调用getRpt(20)
                        }
                        else if (21 == type) {
                            memset(eachColumnSalesNumRpt, 0, sizeof(eachColumnSalesNumRpt));
                            memcpy(eachColumnSalesNumRpt, data_receive, data_receive_len);
                            eachColumnSalesNumRpt_len = data_receive_len;
                            setEvent(
                                    VMC_EACHCOLUMN_SALES_NUM_RPT);    //各货道总计销售个数,收到此事件后，PC端可以调用getRpt(21)
                        }
                        else if (22 == type) {
                            memset(eachColumnSalesMoneyRpt, 0, sizeof(eachColumnSalesMoneyRpt));
                            memcpy(eachColumnSalesMoneyRpt, data_receive, data_receive_len);
                            eachColumnSalesMoneyRpt_len = data_receive_len;
                            setEvent(
                                    VMC_EACHCOLUMN_SALES_MONEY_RPT);//各货道总计销售金额,收到此事件后，PC端可以调用getRpt(22)
                        }
                        else if (29 == type) {
                            memset(vmcMasterVerRpt, 0, sizeof(vmcMasterVerRpt));
                            memcpy(vmcMasterVerRpt, data_receive, data_receive_len);
                            vmcMasterVerRpt_len = data_receive_len;
                            setEvent(
                                    VMC_MASTER_VER_RPT);    //主控软件版本报告,收到此事件后，PC端可以调用getRpt(29)
                        }
                        else if (30 == type) {
                            memset(lampmodeRpt, 0, sizeof(lampmodeRpt));
                            memcpy(lampmodeRpt, data_receive, data_receive_len);
                            setEvent(
                                    VMC_LAMP_MODE_RPT);    //照明动作模式报告,收到此事件后，PC端可以调用getRpt(30)
                        }
                        else if (34 == type) {
                            memset(vmcHeaterTimeRpt, 0, sizeof(vmcHeaterTimeRpt));
                            memcpy(vmcHeaterTimeRpt, data_receive, data_receive_len);
                            setEvent(
                                    VMC_HEATER_TIME_RPT);    //加热节电时间带报告,收到此事件后，PC端可以调用getRpt(34)
                        }
                        else if (35 == type) {
                            memset(vmcCompressorTimeRpt, 0, sizeof(vmcCompressorTimeRpt));
                            memcpy(vmcCompressorTimeRpt, data_receive, data_receive_len);
                            setEvent(
                                    VMC_COMPRESSOR_TIME_RPT);    //制冷节电时间带报告,收到此事件后，PC端可以调用getRpt(35)
                        }
                        else if (37 == type) {
                            memset(vmcHeaterOnRpt, 0, sizeof(vmcHeaterOnRpt));
                            memcpy(vmcHeaterOnRpt, data_receive, data_receive_len);
                            setEvent(
                                    VMC_HEATER_ON_RPT);    //加热库内ON/OFF温度报告,收到此事件后，PC端可以调用getRpt(37)
                        }
                        else if (38 == type) {
                            memset(vmcCompressorOnRpt, 0, sizeof(vmcCompressorOnRpt));
                            memcpy(vmcCompressorOnRpt, data_receive, data_receive_len);
                            setEvent(
                                    VMC_COMPRESSOR_ON_RPT);    //制冷库内ON/OFF温度报告,收到此事件后，PC端可以调用getRpt(38)
                        }
                        returnAck();
                        printLog(frame_send, frame_send_len, 1);
                        clearData();
                        break;
                    case STATUS_RPT:
                        if (1 == type) {
                            memset(vmcStatusRpt, 0, sizeof(vmcStatusRpt));
                            memcpy(vmcStatusRpt, data_receive, data_receive_len);
                            setEvent(VMC_STATUS_RPT);    //VMC系统状态报告,收到此报告后，调用getVmcStatusRpt()
                        }
                        else if (2 == type) {
                            memset(vmcErrorRpt, 0, sizeof(vmcErrorRpt));
                            memcpy(vmcErrorRpt, data_receive, data_receive_len);
                            vmcErrorRpt_len = data_receive_len;
                            setEvent(VMC_ERROR_RPT);    //VMC故障状态,收到此报告后，调用getVmcErrorRpt()
                        }
                        else if (3 == type) {
                            memset(vmcSoldoutRpt, 0, sizeof(vmcSoldoutRpt));
                            memcpy(vmcStatusRpt, data_receive, data_receive_len);
                            vmcSoldoutRpt_len = data_receive_len;
                            setEvent(VMC_SOLDOUT_RPT);    //货道售空状态,收到此报告后，调用getVmcSoldoutRpt()
                        }
                        else if (4 == type) {
                            inputCash = (int) data_receive[1] * 256 +
                                        (int) data_receive[2];//cal int_value of cash
                            setEvent(VMC_INPUTCASH_RPT);    //用户投币余额,收到此报告后，调用getInputCash()
                        }
                        else if (5 == type) {
                            memset(vmcTempRpt, 0, sizeof(vmcTempRpt));
                            memcpy(vmcTempRpt, data_receive, data_receive_len);
                            setEvent(VMC_TEMP_RPT);    //库内温度,收到此报告后，调用getRpt(45)
                        }
                        else if (6 == type) {
                            columnNum = data_receive[1];     //存储货道数量
                            memset(vmcClmsalestatusRpt, 0, sizeof(vmcClmsalestatusRpt));
                            memcpy(vmcClmsalestatusRpt, data_receive, data_receive_len);
                            vmcClmsalestatusRpt_len = data_receive_len;
                            setEvent(
                                    VMC_CLMSALESTATUS_RPT);    //货道可销售状态,收到此报告后，调用getVmcClmsalestatusRpt()
                        }
                        returnAck();
                        printLog(frame_send, frame_send_len, 1);
                        clearData();
                        break;
                    case BUTTON_RPT:
                        if (1 == type) {
                            selectedColumnId = (int) data_receive[1];    //存储货道号
                            setEvent(
                                    BUTTON_RPT_SELECTGOODS);    //用户按下按钮,收到此事件后，可以调用getSelectedColumnId()
                        } else if (2 == type) {
                            setEvent(BUTTON_RPT_RETURNCOIN);    //退币杆操作
                        }
                        returnAck();
                        printLog(frame_send, frame_send_len, 1);
                        clearData();
                        break;
                    case VENDOUT_RPT:
                        memset(vendoutRptReceived, 0, sizeof(vendoutRptReceived));
                        memcpy(vendoutRptReceived, data_receive, data_receive_len);
                        setEvent(VENDOUT_RPT);    //出货报告,收到此事件后，PC端可以调用getVendoutRpt()
                        returnAck();
                        printLog(frame_send, frame_send_len, 1);
                        clearData();
                        break;
                    case COST_RPT:
                        if (int_charge ==
                            (int) data_receive[3] * 256 + (int) data_receive[4])
                            setEvent(COST_RPT_SUCCEED);            //扣费成功事件
                        else
                            setEvent(COST_RPT_FAILED);            //扣费失败事件
                        returnAck();
                        printLog(frame_send, frame_send_len, 1);
                        clearData();
                        int_charge = 0;
                        break;
                    case POLL:
                        isVendingMachineAlive = true;
                        if (getMachineIdTag)        //VMC启动完成，主动申请一次机器编号
                        {
                            getInfoType = 13;
                            returnGetInfo();
                            printLog(frame_send, frame_send_len, 1);
                            clearData();
                            getInfoType = 0;
                            getMachineIdTag = false;
                            getVmcStatusTag = true;
                            break;
                        }
                        if (getVmcStatusTag)        //主动申请一次机器编号后,再申请一次VMC系统状态
                        {
                            getStatusType = 1;
                            returnGetStatus();
                            printLog(frame_send, frame_send_len, 1);
                            clearData();
                            getStatusType = 0;
                            getVmcStatusTag = false;
                            getVmcColumnStatusTag = true;
                            break;
                        }
                        if (getVmcColumnStatusTag)        //申请一次VMC系统状态,再申请一次货道可销售状态，至此VMC底层初始化完成
                        {
                            getStatusType = 6;
                            returnGetStatus();
                            printLog(frame_send, frame_send_len, 1);
                            clearData();
                            getStatusType = 0;
                            getVmcColumnStatusTag = false;
                            setEvent(
                                    ACTION_RPT_VMCSTARTED);        //VMC启动完成,收到此事件后，PC端可以发起VMC配置,申请VMC信息等
                            break;
                        }
                        if (1 == PollTag)        //点亮按钮
                        {
                            returnControlInd(6);
                            printLog(frame_send, frame_send_len, 1);
                            clearData();
                            PollTag = 0;
                            break;
                        }
                        else if (2 == PollTag)        //出货查询模式
                        {
                            returnVendoutIndRequest();
                            printLog(frame_send, frame_send_len, 1);
                            clearData();
                            PollTag = 50;
                            break;
                        }
                        else if (3 == PollTag)        //出货动作模式
                        {
                            returnVendoutIndAction();
                            printLog(frame_send, frame_send_len, 1);
                            clearData();
                            PollTag = 0;
                            selectedColumnId = 0;        //
                            payment_method = 0;                //一次出货完成后重置payment
                            break;
                        }
                        else if (4 == PollTag)        //退币
                        {
                            returnControlInd(2);
                            printLog(frame_send, frame_send_len, 1);
                            clearData();
                            PollTag = 0;
                            break;
                        }
                        else if (5 == PollTag)        //PC指示扣费
                        {
                            returnCostInd();
                            printLog(frame_send, frame_send_len, 1);
                            clearData();
                            PollTag = 0;
                            break;
                        }
                        else if (6 == PollTag)        //PC请求get_status
                        {
                            returnGetStatus();
                            printLog(frame_send, frame_send_len, 1);
                            clearData();
                            PollTag = 0;
                            getStatusType = 0;
                            break;
                        }
                        else if (7 == PollTag)        //PC请求get_info
                        {
                            returnGetInfo();
                            printLog(frame_send, frame_send_len, 1);
                            clearData();
                            PollTag = 0;
                            getInfoType = 0;
                            break;
                        }
                        else if (8 == PollTag)        //PC请求设置机器编号
                        {
                            returnConfigIndSetMachineId();
                            printLog(frame_send, frame_send_len, 1);
                            clearData();
                            PollTag = 0;
                            break;
                        }
                        else if (9 == PollTag)        //PC请求设置设置货道现金单价
                        {
                            returnConfigIndSetColumnPrice_Cash();
                            printLog(frame_send, frame_send_len, 1);
                            clearData();
                            PollTag = 0;
                            break;
                        }
                        else if (10 == PollTag)        //PC请求设置设置货道非现金单价
                        {
                            returnConfigIndSetColumnPrice_notCash();
                            printLog(frame_send, frame_send_len, 1);
                            clearData();
                            PollTag = 0;
                            break;
                        }
                        else if (11 == PollTag)        //PC设置连续购买个数
                        {
                            returnConfigIndSetSalesMaxNum();
                            printLog(frame_send, frame_send_len, 1);
                            clearData();
                            PollTag = 0;
                            break;
                        }
                        else {
                            returnAck();
                            printLog(frame_send, frame_send_len, 1);
                            clearData();
                            break;
                        }
                    case ACK:
                        if (50 == PollTag) {
                            setEvent(
                                    VENDOUT_READY);        //出货查询结果：可以出货.收到此事件后，可以调用vendoutIndAction()
                            PollTag = 0;
                        }
                        break;
                    case NAK:
                        if (50 == PollTag)        //触屏选货后，收到NAK，中止交易
                        {
                            setEvent(VENDOUT_NOTREADY);        //出货查询结果：不可以出货
                            selectedColumnId = 0;
                            PollTag = 0;
                        } else
                            setEvent(NAK);        //收到NAK事件
                        break;
                }
            }
        }
    }
    fclose(stdout);
}

void FujiProtocol::frameSendSolve(unsigned char char_cmd, unsigned char *data,
                                  unsigned int datasize) {
    int_fcc = 0;
    frame_send[0] = STX;
    int_len = datasize + 4;
    len[0] = int_len >> 8;
    len[1] = int_len & 0xFF;
    frame_send[1] = len[0];
    frame_send[2] = len[1];
    frame_send[3] = char_cmd;
    frame_send[4] = sn;
    memcpy(frame_send + 5, data, datasize);
    for (unsigned int i = 0; i < int_len; i++) {
        int_fcc += frame_send[i + 1];
    }                        //cal value of FCC
    int_fcc = int_fcc & 0xFFFF;
    fcc[0] = int_fcc >> 8;
    fcc[1] = int_fcc & 0xFF;
    frame_send[int_len + 1] = fcc[0];
    frame_send[int_len + 2] = fcc[1];
    frame_send_len = datasize + 7;
}

bool const FujiProtocol::checkInstructions() {
    bool b = false;
    int_fcc = 0;
    for (unsigned int i = 0; i < int_len; i++) {
        int_fcc += frame_receive[i + 1];
    }
    if (int_fcc ==
        (unsigned int) frame_receive[int_len + 1] * 256 +
        (unsigned int) frame_receive[int_len + 2])
        b = true;
    return b;
}

void FujiProtocol::returnUpdataDat(unsigned char char_type) {
    data_send[0] = char_type;
    for (unsigned int i = 0; i < UPDATE_INI_SIZE; i++) {
        data_send[i + 1] = 0x00;
    }
    data_send_len = 104;
    frameSendSolve(UPDATA_DAT, data_send, data_send_len);
    writeData(frame_send, frame_send_len);
}

void FujiProtocol::returnAck() {
    frameSendSolve(ACK, data_send, data_send_len);    //ACK
    writeData(frame_send, frame_send_len);
}

void FujiProtocol::returnNak() {
    frameSendSolve(NAK, data_send, data_send_len);    //NAK
    writeData(frame_send, frame_send_len);
}

void FujiProtocol::returnControlInd(unsigned char char_type) {
    if (6 == char_type) {
        data_send[0] = 0x06;            //指示VMC选货
        data_send[1] = 0x02;            //通过货道ID方式
        data_send[2] = selectedColumnId & 0xFF;    //货道号
        data_send[3] = 0x00;
        data_send[4] = 0x00;
        data_send_len = 5;
        frameSendSolve(CONTROL_IND, data_send, data_send_len);
        writeData(frame_send, frame_send_len);
    } else if (2 == char_type) {
        data_send[0] = 0x02;            //退币操作
        data_send_len = 1;
        frameSendSolve(CONTROL_IND, data_send, data_send_len);    //ACK
        writeData(frame_send, frame_send_len);
    }
}

void FujiProtocol::printLog(unsigned char *data, unsigned int data_size, int tag) {
    if (debug) {
        getCurrentTime();
        cout << timebuf << ".";
        cout.setf(ios::left);      //设置对齐方式为左对齐
        cout.fill('0');             //设置填充方式,不足位补0
        cout.width(3);
        cout << dec << (int) (tv.tv_usec / 1000) << "  ";
        cout.setf(ios::right);      //设置对齐方式为右对齐
        cout.fill('0');             //设置填充方式,不足位补0
        if (0 == tag)
            cout << "Read :";
        else
            cout << "Write:";
        for (unsigned int i = 0; i < data_size; i++) {
            cout.width(2);              //设置宽度为2，只对下条输出有用
            cout << hex << (unsigned int) data[i];
        }
        cout << endl;
    }
}

void FujiProtocol::clearData() {
    memset(frame_receive, 0, sizeof(frame_receive));
    memset(frame_send, 0, sizeof(frame_receive));
    memset(data_receive, 0, sizeof(data_receive));
    memset(data_send, 0, sizeof(data_send));
    data_send_len = 0;
}

void FujiProtocol::setEvent(int Id) {
    eventID = Id;
    sem_post(&event_sem_id);
}

int FujiProtocol::getEvent() {
    sem_wait(&event_sem_id);  // 等待事件发生
    return eventID;
}

void FujiProtocol::getStatus(int i_type)        //PC请求get_status
{
    getStatusType = i_type;
    PollTag = 6;
}

void FujiProtocol::getInfo(int i_type)        //PC请求get_info
{
    getInfoType = i_type;
    PollTag = 7;
}

void FujiProtocol::returnGetStatus() {
    data_send[0] = getStatusType & 0xFF;
    data_send_len = 1;
    frameSendSolve(GET_STATUS, data_send, data_send_len);
    writeData(frame_send, frame_send_len);
}

void FujiProtocol::returnGetInfo() {
    data_send[0] = getInfoType & 0xFF;
    data_send_len = 1;
    frameSendSolve(GET_INFO, data_send, data_send_len);
    writeData(frame_send, frame_send_len);
}

void FujiProtocol::getVendoutRpt(unsigned char *gVR) {
    memcpy(gVR, vendoutRptReceived, sizeof(vendoutRptReceived));
}

void FujiProtocol::getVmcStatusRpt(unsigned char *gVSR) {
    memcpy(gVSR, vmcStatusRpt, sizeof(vmcStatusRpt));
}

int FujiProtocol::getVmcErrorRpt(unsigned char *gVER) {
    memcpy(gVER, vmcErrorRpt, vmcErrorRpt_len);
    return vmcErrorRpt_len;
}

int FujiProtocol::getVmcSoldoutRpt(unsigned char *gVSOR) {
    memcpy(gVSOR, vmcSoldoutRpt, vmcSoldoutRpt_len);
    return vmcSoldoutRpt_len;
}

short FujiProtocol::getInputCash() {
    return inputCash;
}

int FujiProtocol::getVmcClmsalestatusRpt(unsigned char *gVCR) {
    memcpy(gVCR, vmcClmsalestatusRpt, vmcClmsalestatusRpt_len);
    return vmcClmsalestatusRpt_len;
}

int FujiProtocol::getRpt(int m_type, unsigned char *gR) {
    if (1 == m_type) {
        memcpy(gR, vmcConfigRpt, sizeof(vmcConfigRpt));
        return sizeof(vmcConfigRpt);
    }
    else if (2 == m_type) {
        memcpy(gR, vmcPowerRpt, sizeof(vmcPowerRpt));
        return sizeof(vmcPowerRpt);
    }
    else if (3 == m_type) {
        memcpy(gR, vmcTimeRpt, sizeof(vmcTimeRpt));
        return sizeof(vmcTimeRpt);
    }
    else if (4 == m_type) {
        memcpy(gR, vmcHcModeRpt, vmcHcModeRpt_len);
        return vmcHcModeRpt_len;
    }
    else if (7 == m_type) {
        memcpy(gR, vmcGoodsSpidRpt, vmcGoodsSpidRpt_len);
        return vmcGoodsSpidRpt_len;
    }
    else if (10 == m_type) {
        memcpy(gR, vmcSoftVerRpt, vmcSoftVerRpt_len);
        return vmcSoftVerRpt_len;
    }
    else if (19 == m_type) {
        memcpy(gR, vmcRoomHcmodeRpt, sizeof(vmcRoomHcmodeRpt));
        return sizeof(vmcRoomHcmodeRpt);
    }
    else if (20 == m_type) {
        memcpy(gR, totalSalesInformationRpt, sizeof(totalSalesInformationRpt));
        return sizeof(totalSalesInformationRpt);
    }
    else if (21 == m_type) {
        memcpy(gR, eachColumnSalesNumRpt, eachColumnSalesNumRpt_len);
        return eachColumnSalesNumRpt_len;
    }
    else if (22 == m_type) {
        memcpy(gR, eachColumnSalesMoneyRpt, eachColumnSalesMoneyRpt_len);
        return eachColumnSalesMoneyRpt_len;
    }
    else if (29 == m_type) {
        memcpy(gR, vmcMasterVerRpt, vmcMasterVerRpt_len);
        return vmcMasterVerRpt_len;
    }
    else if (30 == m_type) {
        memcpy(gR, lampmodeRpt, sizeof(lampmodeRpt));
        return sizeof(lampmodeRpt);
    }
    else if (34 == m_type) {
        memcpy(gR, vmcHeaterTimeRpt, sizeof(vmcHeaterTimeRpt));
        return sizeof(vmcHeaterTimeRpt);
    }
    else if (35 == m_type) {
        memcpy(gR, vmcCompressorTimeRpt, sizeof(vmcCompressorTimeRpt));
        return sizeof(vmcCompressorTimeRpt);
    }
    else if (37 == m_type) {
        memcpy(gR, vmcHeaterOnRpt, sizeof(vmcHeaterOnRpt));
        return sizeof(vmcHeaterOnRpt);
    }
    else if (38 == m_type) {
        memcpy(gR, vmcCompressorOnRpt, sizeof(vmcCompressorOnRpt));
        return sizeof(vmcCompressorOnRpt);
    }
    else if (45 == m_type) {
        memcpy(gR, vmcTempRpt, sizeof(vmcTempRpt));
        return sizeof(vmcTempRpt);
    }
}

int FujiProtocol::getVmcColumnPrice_Cash(unsigned char *gVCC) {
    memcpy(gVCC, columnPrice_cash, columnPrice_len);
    return columnPrice_len;
}

int FujiProtocol::getVmcColumnPrice_notCash(unsigned char *gVCNC) {
    memcpy(gVCNC, columnPrice_notCash, columnPrice_len);
    return columnPrice_len;
}

void FujiProtocol::getMachineId(unsigned char *gMI) {
    memcpy(gMI, machineId, sizeof(machineId));
}

int FujiProtocol::getSelectedColumnId() {
    return selectedColumnId;
}

short FujiProtocol::getColumnNum()        //获取货道数量
{
    return columnNum;
}

int FujiProtocol::getSalesMaxNum()        //获取连续购买个数
{
    return sales_maxnum;
}

void FujiProtocol::setSalesMaxNum(int maxnum)        //PC设置连续购买个数
{
    sales_maxnum_toBeSet = maxnum;
    PollTag = 11;
}

void FujiProtocol::returnConfigIndSetSalesMaxNum() {
    data_send[0] = 0x06;
    data_send[1] = sales_maxnum_toBeSet & 0xFF;
    data_send_len = 2;
    frameSendSolve(CONFIG_IND, data_send, data_send_len);
    writeData(frame_send, frame_send_len);
}

void FujiProtocol::vendoutIndRequest(int payment, int columnID) {
    selectedColumnId = columnID;
    payment_method = payment;
    PollTag = 2;
}

void FujiProtocol::returnVendoutIndRequest() {
    data_send[0] = 0x01;        //mode=1
    data_send[1] = 0x02;        //method=2,货道ID出货
    if (1 == payment_method)
        data_send[2] = 0x01;        //type=0x01,现金
    else if (2 == payment_method)
        data_send[2] = 0x31;        //type=0x31,非现金
    data_send[3] = selectedColumnId;        //column
    data_send[4] = 0x00;        //SPID
//    data_send[5] = selectedColumnId;
    data_send[5] = 0x00;
    data_send[6] = 0x00;        //COST
    data_send[7] = 0x00;        //COST
    for (unsigned int i = 0; i < 30; i++) {
        data_send[i + 8] = 0x00;
    }
    data_send_len = 38;
    frameSendSolve(VENDOUT_IND, data_send, data_send_len);        //Vendout_IND
    writeData(frame_send, frame_send_len);
}

void FujiProtocol::vendoutIndAction(int payment, int columnID) {
    selectedColumnId = columnID;
    payment_method = payment;
    PollTag = 3;
}

void FujiProtocol::returnVendoutIndAction() {
    data_send[0] = 0x02;        //mode=2
    data_send[1] = 0x02;
    if (1 == payment_method)
        data_send[2] = 0x01;        //type=0x01,现金
    else if (2 == payment_method)
        data_send[2] = 0x31;        //type=0x31,非现金
    data_send[3] = selectedColumnId;        //column
    data_send[4] = 0x00;        //SPID
//    data_send[5] = selectedColumnId;
    data_send[5] = 0x00;
    data_send[6] = 0x00;        //COST
    data_send[7] = 0x00;            //COST
    for (unsigned int i = 0; i < 30; i++) {
        data_send[i + 8] = 0x00;
    }
    data_send_len = 38;
    frameSendSolve(VENDOUT_IND, data_send, data_send_len);        //Vendout_IND
    writeData(frame_send, frame_send_len);
}

void FujiProtocol::returnCostInd() {
    data_send[0] = 0x00;
    data_send[1] = 0x00;
    data_send[2] = 0x00;
    data_send[3] = int_charge >> 8;
    data_send[4] = int_charge & 0xFF;
    for (int i = 0; i < 6; i++) {
        data_send[5 + i] = 0x00;
    }
    data_send_len = 11;
    frameSendSolve(COST_IND, data_send, data_send_len);
    writeData(frame_send, frame_send_len);
}

void FujiProtocol::lightButton(int columnID) {
    selectedColumnId = columnID;
    PollTag = 1;
}

void FujiProtocol::returnCoin()        //PC指示退币
{
    PollTag = 4;
}

void FujiProtocol::charge(int price)        //PC指示扣费金额
{
    int_charge = price;
    PollTag = 5;
}

void FujiProtocol::setMachineId(unsigned char const *vmcId)        //设置机器编号
{
    memset(machineId_toBeSet, 0, sizeof(machineId_toBeSet));
    memcpy(machineId_toBeSet, vmcId, 8);
    PollTag = 8;
}

void FujiProtocol::returnConfigIndSetMachineId() {
    data_send[0] = 0x08;
    memcpy(data_send + 1, machineId_toBeSet, 8);
    for (int i = 0; i < 4; i++) {
        data_send[9 + i] = 0x00;
    }
    data_send_len = 13;
    frameSendSolve(CONFIG_IND, data_send, data_send_len);
    writeData(frame_send, frame_send_len);
}

void FujiProtocol::setColumnPrice_Cash(unsigned char const *colprice_cash) {
    memset(columnPrice_cash_toBeSet, 0, columnNum * 2);
    memcpy(columnPrice_cash_toBeSet, colprice_cash, columnNum * 2);
    PollTag = 9;
}

void FujiProtocol::returnConfigIndSetColumnPrice_Cash() {
    data_send[0] = 0x03;
    data_send[1] = ((unsigned short) columnNum) & 0xFF;
    memcpy(data_send + 2, columnPrice_cash_toBeSet, 2 * columnNum);
    data_send_len = 2 * columnNum + 2;
    frameSendSolve(CONFIG_IND, data_send, data_send_len);
    writeData(frame_send, frame_send_len);
}

void FujiProtocol::setColumnPrice_notCash(unsigned char const *colprice_notcash) {
    memset(columnPrice_notCash_toBeSet, 0, columnNum * 2);
    memcpy(columnPrice_notCash_toBeSet, colprice_notcash, columnNum * 2);
    PollTag = 10;
}

void FujiProtocol::returnConfigIndSetColumnPrice_notCash() {
    data_send[0] = 0x04;
    data_send[1] = ((unsigned short) columnNum) & 0xFF;
    memcpy(data_send + 2, columnPrice_notCash_toBeSet, 2 * columnNum);
    data_send_len = 2 * columnNum + 2;
    frameSendSolve(CONFIG_IND, data_send, data_send_len);
    writeData(frame_send, frame_send_len);
}

void FujiProtocol::getCurrentTime() {
    gettimeofday(&tv, NULL);
    strftime(timebuf, sizeof(timebuf) - 1, "%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec));
}

void *threadDaemon(void *arg) {
    while (true) {
        sleep(ALIVE_OVERTIME);          //等待20秒建立通信
        if (!fp.isVendingMachineAlive) {
            fp.setEvent(SERIAL_ERROR_EVENT);
        }
        else
            fp.setEvent(SERIAL_ALIVE_EVENT);
        fp.isVendingMachineAlive = false;
    }
    return NULL;
}