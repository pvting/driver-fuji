/*
 * serialPort.cpp
 *
 *  Created on: Dec 22, 2016
 *      Author: bl
 */
#include <termios.h>
#include <sstream>
#include "serialPort.h"
#include "xcept.h"

using namespace std;

serialPort::serialPort() {
    fd = 0;
}

serialPort::~serialPort() {
    if (-1 == close(fd)) {
        throw vendingMachineError("serial close error");
    }
}

void serialPort::serialInitialization(char const *portName,
                                      unsigned int baudRate) {
    struct termios Opt;
    speed_t s;

    fd = open(portName, O_RDWR | O_NOCTTY | O_NDELAY);
    if (-1 == fd) {
        throw vendingMachineError("serial open error");
    }
    if (-1 == fcntl(fd, F_SETFL, 0)) {
        throw vendingMachineError("serial fcntl error");
    }
    if (tcgetattr(fd, &Opt) != 0) {
        throw vendingMachineError("serial tcgetattr error");
    }
    switch (baudRate) {
        case 115200:
            s = B115200;
            break;
        case 38400:
            s = B38400;
            break;
        case 19200:
            s = B19200;
            break;
        case 9600:
            s = B9600;
            break;
        default:
            throw vendingMachineError("serial baudRate error");
    }
    cfsetispeed(&Opt, s);
    cfsetospeed(&Opt, s);

    Opt.c_cflag &= ~CSTOPB;  // one stop bit
    Opt.c_cflag |= (CLOCAL | CREAD);

    cfmakeraw(&Opt);  // 初始化串口设置，（无校验，8位停止位）no parity, Mask the character size bits, 8 bit
//    Opt.c_cflag |= PARENB;
//    Opt.c_cflag &= ~PARODD;
//    Opt.c_iflag |= (INPCK /*| ISTRIP*/);
    Opt.c_cc[VMIN] = 1;
    Opt.c_cc[VTIME] = 0;
    tcflush(fd, TCIOFLUSH);
    if (tcsetattr(fd, TCSANOW, &Opt) != 0) {
        throw vendingMachineError("serial tcsetattr error");
    }
}

void serialPort::readData(unsigned char *buf, unsigned int bufSize) {
    int len = 0;
    int len1;

    while (len < bufSize) {
        len1 = read(fd, (char *) buf + len, bufSize - len);
        if (-1 == len1) {
            throw vendingMachineError("serial read error");
        }
        len += len1;
    }
}

void serialPort::writeData(unsigned char const *buf, unsigned int bufSize) {
    int len = 0;
    int len1;

    while (len < bufSize) {
        len1 = write(fd, (char *) buf + len, bufSize - len);
        if (-1 == len1) {
            throw vendingMachineError("serial write error");
        }
        len += len1;
    }
}

