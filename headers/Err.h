/*
 * Err.h
 *
 *  Created on: 22 févr. 2016
 *      Author: maxime
 */

#ifndef HEADERS_ERR_H_
#define HEADERS_ERR_H_



enum errors
{
	NoErr, CanNotOpenFile, geneBin, CmdInconnue, MissingParam, ModeDistanceNonDef, MvSur3ou4AxesNonPrisEnCharge, DeplacementDeZero
};


class Err : std::exception
{
public:
	Err(errors x);
	errors _err;
};



#endif /* HEADERS_ERR_H_ */
