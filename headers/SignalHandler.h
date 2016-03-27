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

class SignalHandler {
public:
	SignalHandler();
	virtual ~SignalHandler();
	static void exitSignalHandler(int _ignored);
};

#endif /* SIGNALHANDLER_H_ */
