/*
 * Test4.h
 *
 *  Created on: Jun 9, 2010
 *      Author: sumeet
 */

#ifndef TEST4_H_
#define TEST4_H_
#include "Date.h"
#include "BinaryData.h"

class Test4 {
	Date date;
	Date datt;
	Date dattm;
	BinaryData binar;
public:
	Test4();
	virtual ~Test4();
	Date getDate() const;
    void setDate(Date date);
    Date getDatt() const;
    void setDatt(Date datt);
    Date getDattm() const;
    void setDattm(Date dattm);
    BinaryData getBinar() const;
    void setBinar(BinaryData binar);
};

#endif /* TEST4_H_ */
