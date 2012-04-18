/*
 * DefTemp.cpp
 *
 *  Created on: Sep 12, 2009
 *      Author: sumeet
 */

#include "DefTemp.h"

DefTemp::DefTemp() {
	// TODO Auto-generated constructor stub

}

DefTemp::~DefTemp() {
	// TODO Auto-generated destructor stub
}

Context DefTemp::getContext()
{
	Context cnt;
	cnt["dat"] = "1.js";
	return cnt;
}
