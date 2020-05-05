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
 * FileAuthController.cpp
 *
 *  Created on: Nov 23, 2010
 *      Author: sumeet
 */

#include "FileAuthController.h"

FileAuthController::FileAuthController(const std::string& filename, const std::string& delimiter) {
	this->filename = filename;
	this->delimiter = delimiter;
}

FileAuthController::~FileAuthController() {
	
}

std::string FileAuthController::treat_password(const std::string& password)
{
	return password;
}

bool FileAuthController::isInitialized()
{
	std::ifstream ifs(this->filename.c_str());
	bool fl = ifs.is_open();
	ifs.close();
	return fl;
}

bool FileAuthController::authenticate(const std::string& username, const std::string& password)
{
	std::string userstamp = (username + delimiter + treat_password(password));
	std::ifstream ifs(this->filename.c_str());
	if(ifs.is_open())
	{
		std::string temp;
		while(getline(ifs, temp))
		{
			if(temp.find(userstamp)!=std::string::npos)
			{
				ifs.close();
				return true;
			}
		}
		ifs.close();
	}
	return false;
}

bool FileAuthController::getPassword(const std::string& username, std::string &passwd)
{
	bool passwdf = false;
	std::ifstream ifs(this->filename.c_str());
	if(ifs.is_open() && username!="")
	{
		std::string temp;
		while(getline(ifs, temp))
		{
			std::vector<std::string> tempv;
			StringUtil::split(tempv, temp, (this->delimiter));
			if(tempv.size()>=2 && tempv.at(0)==username)
			{
				passwdf = true;
				ifs.close();
				passwd = tempv.at(1);
				break;
			}
		}
		ifs.close();
	}
	return passwdf;
}

std::string FileAuthController::getUserRole(const std::string& username)
{
	//bool passwdf = false;
	std::ifstream ifs(this->filename.c_str());
	if(ifs.is_open() && username!="")
	{
		std::string temp;
		while(getline(ifs, temp))
		{
			std::vector<std::string> tempv;
			StringUtil::split(tempv, temp, (this->delimiter));
			if(tempv.size()>=3 && tempv.at(0)==username)
			{
				//passwdf = true;
				ifs.close();
				return tempv.at(2);
			}
		}
		ifs.close();
	}
	std::string blnk;
	return blnk;
}

std::string FileAuthController::get(const std::string& username, const int& pos)
{
	//bool passwdf = false;
	std::ifstream ifs(this->filename.c_str());
	if(ifs.is_open() && username!="")
	{
		std::string temp;
		while(getline(ifs, temp))
		{
			std::vector<std::string> tempv;
			StringUtil::split(tempv, temp, (this->delimiter));
			if((int)tempv.size()>pos && tempv.at(0)==username)
			{
				//passwdf = true;
				ifs.close();
				return tempv.at(pos);
			}
		}
		ifs.close();
	}
	std::string blnk;
	return blnk;
}
