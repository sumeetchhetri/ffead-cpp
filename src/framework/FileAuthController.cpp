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

bool FileAuthController::authenticate(std::string_view username, std::string_view password)
{
	std::string userstamp = (std::string(username) + delimiter + std::string(password));
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

std::string FileAuthController::getUserRole(std::string_view username)
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
