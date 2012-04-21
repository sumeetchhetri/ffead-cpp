/*
 * DefTemp.h
 *
 *  Created on: Sep 12, 2009
 *      Author: sumeet
 */

#ifndef DEFTEMP_H_
#define DEFTEMP_H_
#include "TemplateHandler.h"

class DefTemp : public TemplateHandler{
public:
	DefTemp();
	virtual ~DefTemp();
	Context getContext();
};

#endif /* DEFTEMP_H_ */
