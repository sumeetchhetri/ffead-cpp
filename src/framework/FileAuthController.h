/*
	Copyright 2009-2020, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
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
