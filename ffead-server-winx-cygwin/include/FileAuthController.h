/*
 * FileAuthController.h
 *
 *  Created on: Nov 23, 2010
 *      Author: sumeet
 */

#ifndef FILEAUTHCONTROLLER_H_
#define FILEAUTHCONTROLLER_H_
#include "AuthController.h"
#include "fstream"

class FileAuthController :public AuthController
{
	string filename;
	string delimiter;
public:
	string treat_password(string);
	FileAuthController(string,string);
	virtual ~FileAuthController();
	bool authenticate(string,string);
	bool isInitialized();
	bool authenticate(map<string,string>){return false;};
	bool handle(HttpRequest* req,HttpResponse* res){return false;};
	bool getPassword(string username,string &passwd);
	string getUserRole(string);
};

#endif /* FILEAUTHCONTROLLER_H_ */
