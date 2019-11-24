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
	std::string filename;
	std::string delimiter;
public:
	std::string treat_password(const std::string&);
	FileAuthController(const std::string&, const std::string&);
	virtual ~FileAuthController();
	virtual bool authenticate(const std::string&, const std::string&);
	virtual bool isInitialized();
	virtual bool getPassword(const std::string& username, std::string &passwd);
	virtual std::string getUserRole(const std::string&);
	virtual std::string get(const std::string& username, const int& pos);
};

#endif /* FILEAUTHCONTROLLER_H_ */
