package com.want.vmc_impl_fuji;

import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.RadioGroup;
import android.widget.TextView;



public class FujiImpl extends AppCompatActivity {

    int column;
    int payMent;
    EditText et;
    TextView tv;


    //     Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("fujiProtocol");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        et = (EditText) findViewById(R.id.editText);
        et.setOnFocusChangeListener(new android.view.View.OnFocusChangeListener() {

            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if (hasFocus) {
                    et.setText("");
                } else {
                    et.setText("请输入type类型");
                }
            }
        });
        tv = (TextView) findViewById(R.id.textview);
        tv.setText("富士售货机");
        tv.append("\n");

        // Example of a call to a native method

        Thread thread_protocol = new Thread(new Runnable() {
            @Override
            public void run() {
                startProtocol();
            }
        });
        thread_protocol.start();

        final Handler mHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 11:
                        tv.append("VMC启动完成!");
                        tv.append("\n");
                        break;
                    case 12:
                        tv.append("出货完成!");
                        tv.append("\n");
                        break;
                    case 13:
                        tv.append("用户选择按键：" + getSelectedColumnId());
                        tv.append("\n");
                        break;
                    case 14:
                        tv.append("退币杆操作!");
                        tv.append("\n");
                        break;
                    case 15:
                        tv.append("门开状态");
                        tv.append("\n");
                        break;
                    case 16:
                        tv.append("出货查询结果：可以出货");
                        tv.append("\n");
                        break;
                    case 17:
                        tv.append("出货查询结果：不可以出货");
                        tv.append("\n");
                        break;
                    case 18:
                        tv.append("扣费成功");
                        tv.append("\n");
                        break;
                    case 19:
                        tv.append("扣费失败");
                        tv.append("\n");
                        break;
                    case 21:
                        tv.append("VMC系统状态报告:" + bytes2HexString(getVmcStatusRpt()));
                        tv.append("\n");
                        break;
                    case 22:
                        tv.append("VMC故障状态报告:" + bytes2HexString(getVmcErrorRpt()));
                        tv.append("\n");
                        break;
                    case 23:
                        tv.append("货道售空状态报告:" + bytes2HexString(getVmcSoldoutRpt()));
                        tv.append("\n");
                        break;
                    case 24:
                        tv.append("用户投币余额：" + getInputCash());
                        tv.append("\n");
                        break;
                    case 25:
                        tv.append("库内温度状态报告:" + bytes2HexString(getRpt(45)));
                        tv.append("\n");
                        break;
                    case 26:
                        tv.append("货道可销售状态报告:" + bytes2HexString(getVmcClmsalestatusRpt()));
                        tv.append("\n");
                        break;
                    case 27:
                        tv.append("系统配置报告:" + bytes2HexString(getRpt(1)));
                        tv.append("\n");
                        break;
                    case 28:
                        tv.append("VMC能力报告:" + bytes2HexString(getRpt(2)));
                        tv.append("\n");
                        break;
                    case 29:
                        tv.append("VMC当前时间:" + bytes2HexString(getRpt(3)));
                        tv.append("\n");
                        break;
                    case 30:
                        tv.append("货道冷热模式报告:" + bytes2HexString(getRpt(4)));
                        tv.append("\n");
                        break;
                    case 31:
                        tv.append("货道现金单价报告:" + bytes2HexString(getVmcColumnPrice_Cash()));
                        tv.append("\n");
                        break;
                    case 32:
                        tv.append("货道非现金单价报告:" + bytes2HexString(getVmcColumnPrice_notCash()));
                        tv.append("\n");
                        break;
                    case 33:
                        tv.append("货道商品编码报告:" + bytes2HexString(getRpt(7)));
                        tv.append("\n");
                        break;
                    case 34:
                        tv.append("最大连续购买个数：" + getSalesMaxNum());
                        tv.append("\n");
                        break;
                    case 35:
                        tv.append("智能控制盒软件版本:" + bytes2HexString(getRpt(10)));
                        tv.append("\n");
                        break;
                    case 36:
                        tv.append("机器编号" + bytes2HexString(getMachineId()));
                        tv.append("\n");
                        break;
                    case 37:
                        tv.append("库内冷热模式报告:" + bytes2HexString(getRpt(19)));
                        tv.append("\n");
                        break;
                    case 38:
                        tv.append("累计销售信息:" + bytes2HexString(getRpt(20)));
                        tv.append("\n");
                        break;
                    case 39:
                        tv.append("各货道总计销售个数:" + bytes2HexString(getRpt(21)));
                        tv.append("\n");
                        break;
                    case 40:
                        tv.append("各货道总计销售金额:" + bytes2HexString(getRpt(22)));
                        tv.append("\n");
                        break;
                    case 41:
                        tv.append("主控软件版本报告:" + bytes2HexString(getRpt(29)));
                        tv.append("\n");
                        break;
                    case 42:
                        tv.append("照明动作模式报告:" + bytes2HexString(getRpt(30)));
                        tv.append("\n");
                        break;
                    case 43:
                        tv.append("加热节电时间带报告:" + bytes2HexString(getRpt(34)));
                        tv.append("\n");
                        break;
                    case 44:
                        tv.append("制冷节电时间带报告:" + bytes2HexString(getRpt(35)));
                        tv.append("\n");
                        break;
                    case 45:
                        tv.append("加热库内ON/OFF温度报告:" + bytes2HexString(getRpt(37)));
                        tv.append("\n");
                        break;
                    case 46:
                        tv.append("制冷库内ON/OFF温度报告:" + bytes2HexString(getRpt(38)));
                        tv.append("\n");
                        break;
                    case 47:
                        tv.append("门关状态");
                        tv.append("\n");
                        break;
                    case 48:
                        tv.append("串口通信异常");
                        tv.append("\n");
                        break;
                    case 49:
                        tv.append("串口通信正常");
                        tv.append("\n");
                        break;
                    case 161:
                        tv.append("收到NAK");
                        tv.append("\n");
                        break;
                    case 229:
                        tv.append("出货报告:" + bytes2HexString(getVendoutRpt()));
                        tv.append("\n");
                        break;
                }
            }
        };

        Thread thread_event = new Thread(new Runnable() {

            @Override
            public void run() {
                while (true) {
                    int i;
                    i = getEvent();
                    mHandler.sendEmptyMessage(i);
                }
            }
        });
        thread_event.start();

    }

    public static String bytes2HexString(byte[] b) {        //byte转16进制字符串函数
        String ret = "";
        for (int i = 0; i < b.length; i++) {
            String hex = Integer.toHexString(b[i] & 0xFF);
            if (hex.length() == 1) {
                hex = '0' + hex;
            }
            ret += hex.toUpperCase();
        }
        return ret;
    }

    public void chargeButtonClicked(View view) {
        charge(20);
        tv.append("PC指示扣费");
        tv.append("\n");
    }

    public void lightButtonClicked(View view) {
        RadioGroup radioGroupColumn = (RadioGroup) findViewById(R.id.radioGroupColumn);
        switch (radioGroupColumn.getCheckedRadioButtonId()) {
            case R.id.radioFontA:
                column = 13;
                break;
            case R.id.radioFontB:
                column = 14;
                break;
        }
        lightButton(column);
        tv.append("PC指示点亮按钮");
        tv.append("\n");
    }

    public void getColumnNumButtonClicked(View view) {
        tv.append("货道总数:" + getColumnNum());
        tv.append("\n");
    }

    public void returnCoinButtonClicked(View view) {
        returnCoin();
        tv.append("PC指示退币");
        tv.append("\n");
    }

    public void vendoutIndRequestButtonClicked(View view) {
//        RadioGroup radioGroupColumn = (RadioGroup) findViewById(R.id.radioGroupColumn);
//        switch (radioGroupColumn.getCheckedRadioButtonId()) {
//            case R.id.radioFontA:
//                column = 13;
//                break;
//            case R.id.radioFontB:
//                column = 14;
//                break;
//        }
        column = Integer.parseInt(et.getText().toString());
        RadioGroup radioGroupPayment = (RadioGroup) findViewById(R.id.radioGroupPayment);
        switch (radioGroupPayment.getCheckedRadioButtonId()) {
            case R.id.radioFontC:
                payMent = 1;
                break;
            case R.id.radioFontD:
                payMent = 2;
                break;
        }
        vendoutIndRequest(payMent, column);
        tv.append("出货查询模式,货道号" + column);
        tv.append("\n");
    }

    public void vendoutIndActionButtonClicked(View view) {
//        RadioGroup radioGroupColumn = (RadioGroup) findViewById(R.id.radioGroupColumn);
//        switch (radioGroupColumn.getCheckedRadioButtonId()) {
//            case R.id.radioFontA:
//                column = 13;
//                break;
//            case R.id.radioFontB:
//                column = 14;
//                break;
//        }
        column = Integer.parseInt(et.getText().toString());
        RadioGroup radioGroupPayment = (RadioGroup) findViewById(R.id.radioGroupPayment);
        switch (radioGroupPayment.getCheckedRadioButtonId()) {
            case R.id.radioFontC:
                payMent = 1;
                break;
            case R.id.radioFontD:
                payMent = 2;
                break;
        }
        vendoutIndAction(payMent, column);
        tv.append("出货动作模式，货道号" + column);
        tv.append("\n");
    }

    public void getStatusButtonClicked(View view) {
        int type = Integer.parseInt(et.getText().toString());
        getStatus(type);
        tv.append("PC请求getStatus，类型" + type);
        tv.append("\n");
    }

    public void getInfoButtonClicked(View view) {
        int type = Integer.parseInt(et.getText().toString());
        getInfo(type);
        tv.append("PC请求getInfo，类型" + type);
        tv.append("\n");
    }
    

    public void setMachineIdButtonClicked(View view) {
        byte machineId[] = {'7', '3', '1', '6', '5', '5', '9', '4'};
        setMachineId(machineId);
        tv.append("PC请求设置机器编号");       //设置之后可调用getInfo(13)来请求机器编号信息,收到rpt后可调用getMachineId()查看
        tv.append("\n");
    }

    public void setColumnPrice_CashButtonClicked(View view) {
//        byte ColumnPrice_Cash[] = {0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14};
        byte ColumnPrice_Cash[] = {0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x1E, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x1E, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x1E, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x1E, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x1E, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x1E};
        setColumnPrice_Cash(ColumnPrice_Cash);
        tv.append("PC请求设置货道现金单价");       //设置之后,收到rpt后可调用getVmcColumnPrice_Cash()查看
        tv.append("\n");
    }

    public void setColumnPrice_notCashButtonClicked(View view) {
//        byte ColumnPrice_NotCash[] = {0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x1E, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14};
        byte ColumnPrice_NotCash[] = {0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x1E, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x1E, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x1E, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x1E, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x1E, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x14, 0x00, 0x1E};
        setColumnPrice_notCash(ColumnPrice_NotCash);
        tv.append("PC请求设置货道非现金单价");       //设置之后,收到rpt后可调用getVmcColumnPrice_notCash()查看
        tv.append("\n");
    }


    public void setSalesMaxNumButtonClicked(View view) {
        int type = Integer.parseInt(et.getText().toString());
        setSalesMaxNum(type);
        tv.append("PC请求设置连续购买个数为" + type);//设置之后可调用getInfo(8)来请求最大连续购买个数信息,收到rpt后可调用getSalesMaxNum()查看
        tv.append("\n");
    }


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */

    public native void startProtocol();

    public native int getEvent();

    public native int getSelectedColumnId();

    public native short getInputCash();

    public native void charge(int charge_cost);

    public native void lightButton(int columnNo);

    public native short getColumnNum();

    public native void returnCoin();

    public native void vendoutIndRequest(int payment, int columnID);

    public native void vendoutIndAction(int payment, int columnID);

    public native byte[] getVendoutRpt();

    public native void getStatus(int m_type);

    public native void getInfo(int m_type);

    public native byte[] getVmcStatusRpt();

    public native byte[] getVmcErrorRpt();

    public native byte[] getVmcSoldoutRpt();

    public native byte[] getVmcClmsalestatusRpt();

    public native void setMachineId(byte[] machine_id);

    public native byte[] getMachineId();

    public native void setColumnPrice_Cash(byte[] Column_Price_Cash);

    public native byte[] getVmcColumnPrice_Cash();

    public native void setColumnPrice_notCash(byte[] Column_Price_notCash);

    public native byte[] getVmcColumnPrice_notCash();

    public native void setSalesMaxNum(int m_type);

    public native int getSalesMaxNum();

    public native byte[] getRpt(int rpt_type);

}
