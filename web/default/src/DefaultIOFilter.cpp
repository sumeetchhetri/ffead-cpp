/*
 * DefaultIOFilter.cpp
 *
 *  Created on: Apr 7, 2011
 *      Author: sumeet
 */

#include "DefaultIOFilter.h"

DefaultIOFilter::DefaultIOFilter() {
	// TODO Auto-generated constructor stub

}

DefaultIOFilter::~DefaultIOFilter() {
	// TODO Auto-generated destructor stub
}

void DefaultIOFilter::doInputFilter(HttpRequest *req)
{
	//Process the input request here...
	cout << "Processed input request" << endl;
}

void DefaultIOFilter::doOutputFilter(HttpResponse *res)
{
	//Process the output response here...
	cout << "Processed output response" << endl;
}
