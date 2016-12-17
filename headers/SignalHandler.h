/*
 * SignalHandler.h
 *
 *  Created on: Mar 27, 2016
 *      Author: julien
 */

#ifndef SIGNALHANDLER_H_
#define SIGNALHANDLER_H_
#include <signal.h>
#include <iostream>
#include "../headers/Spicomm.h"
#include "../headers/Zynqcomm.hpp"
class SignalHandler {
public:
	SignalHandler();
	virtual ~SignalHandler();
	void register_exitSignalHandler(Zynq_comm * spi);
	static void exitSignalHandler(int _ignored);
private:
	static Zynq_comm * comm_channel;
};


#endif /* SIGNALHANDLER_H_ */
