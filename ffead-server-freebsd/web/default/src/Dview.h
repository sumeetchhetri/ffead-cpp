/*
 * Dview.h
 *
 *  Created on: Sep 12, 2009
 *      Author: sumeet
 */

#ifndef DVIEW_H_
#define DVIEW_H_
#include "DynamicView.h"

class Dview : public DynamicView{
public:
	Dview();
	virtual ~Dview();
	Document getDocument();
};

#endif /* DVIEW_H_ */
