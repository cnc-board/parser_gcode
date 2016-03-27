/*
 * SignalHandler.cpp
 *
 *  Created on: Mar 27, 2016
 *      Author: julien
 */

#include "../headers/SignalHandler.h"

SignalHandler::SignalHandler() {
	signal((int) SIGINT, SignalHandler::exitSignalHandler);
}

SignalHandler::~SignalHandler() {
	// TODO Auto-generated destructor stub
}

void SignalHandler::exitSignalHandler(int _ignored) {
	std::cerr << "emergency exit ..." << std::endl;

	exit(-1);
}
