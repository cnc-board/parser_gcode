/*
 * SignalHandler.cpp
 *
 *  Created on: Mar 27, 2016
 *      Author: julien
 */

#include "../headers/SignalHandler.h"

Zynq_comm * SignalHandler::comm_channel;

SignalHandler::SignalHandler() {
}

SignalHandler::~SignalHandler() {
}

void SignalHandler::register_exitSignalHandler(Zynq_comm * spi) {
	signal((int) SIGINT, SignalHandler::exitSignalHandler);
	comm_channel=spi;
}

void SignalHandler::exitSignalHandler(int _ignored) {
	std::cerr << "emergency exit ..." << std::endl;
	comm_channel->send_reset_FPGA();
	exit(-1);
}
