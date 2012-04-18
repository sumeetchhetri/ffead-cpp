/*
 * DefaultController.cpp
 *
 *  Created on: Aug 20, 2009
 *      Author: sumeet
 */

#include "DefaultController.h"
#include "Cibernate.h"
#include "Object.h"
#include "Test.h"
#include "Test2.h"
#include "Test3.h"
#include "Test4.h"


DefaultController::DefaultController() {
	// TODO Auto-generated constructor stub

}

DefaultController::~DefaultController() {
	// TODO Auto-generated destructor stub
}

HttpResponse DefaultController::service(HttpRequest req)
{
	//cout << "Called the Child class" << flush;
	HttpResponse res;
	Cibernate chib("default");
	vector<string> cols;
	cols.push_back("id");cols.push_back("name");
	string ns = "kriss";
	Object on;
	on << ns;
	chib.addParam("name",on);
	vector<Test2> tec = chib.getARAC<Test2>();
	cout << "\n Cibernate Query fetched " << tec.size() << " rows\n" << flush;
	vector<Test3> tec1 = chib.getARAC<Test3>();
	cout << "\n Cibernate Query fetched " << tec1.size() << " rows\n" << flush;
	vector<Test4> tec2 = chib.getARAC<Test4>();
	cout << "\n Cibernate Query fetched " << tec2.size() << " rows\n" << flush;
	//int *p = NULL;
		//throw 1;
	//int y = *p +1;
	return res;
}

