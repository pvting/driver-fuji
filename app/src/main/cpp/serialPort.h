/*
 * serialPort.h
 *
 *  Created on: Dec 22, 2016
 *      Author: bl
 */

#ifndef _serialPort_h
#define _serialPort_h

#include <fcntl.h>
#include <unistd.h>


class serialPort
{
public:
	serialPort();
	virtual ~serialPort();
	void serialInitialization(char const* portName, unsigned int baudRate);
	void readData(unsigned char* buf, unsigned int bufSize);
	void writeData(unsigned char const* buf, unsigned int bufSize);
protected:
	int fd;
};

#endif
