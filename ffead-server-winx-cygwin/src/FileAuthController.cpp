/*
 * FileAuthController.cpp
 *
 *  Created on: Nov 23, 2010
 *      Author: sumeet
 */

#include "FileAuthController.h"

FileAuthController::FileAuthController(string filename,string delimiter) {
	this->filename = filename;
	this->delimiter = delimiter;
}

FileAuthController::~FileAuthController() {
	// TODO Auto-generated destructor stub
}

string FileAuthController::treat_password(string password)
{
	return password;
}

bool FileAuthController::isInitialized()
{
	ifstream ifs(this->filename.c_str());
	bool fl = ifs.is_open();
	ifs.close();
	return fl;
}

bool FileAuthController::authenticate(string username,string password)
{
	password = treat_password(password);
	string userstamp = (username + delimiter + password);
	ifstream ifs(this->filename.c_str());
	if(ifs.is_open())
	{
		string temp;
		while(getline(ifs, temp))
		{
			if(temp.find(userstamp)!=string::npos)
			{
				ifs.close();
				return true;
			}
		}
		ifs.close();
	}
	return false;
}

bool FileAuthController::getPassword(string username,string &passwd)
{
	string userstamp = (username + delimiter);
	ifstream ifs(this->filename.c_str());
	bool passwdf = false;
	if(ifs.is_open() && username!="")
	{
		string temp;
		while(getline(ifs, temp))
		{
			if(temp.find(userstamp)!=string::npos)
			{
				passwdf = true;
				ifs.close();
				passwd = temp.substr(temp.find(delimiter)+1);
				break;
			}
		}
		ifs.close();
	}
	return passwdf;
}

string FileAuthController::getUserRole(string username)
{
	string userstamp = (username + delimiter);
	ifstream ifs(this->filename.c_str());
	if(ifs.is_open() && username!="")
	{
		string temp;
		while(getline(ifs, temp))
		{
			if(temp.find(userstamp)!=string::npos)
			{
				userstamp = userstamp.substr(userstamp.find_last_of(":")+1);
				ifs.close();
				break;
			}
		}
		ifs.close();
	}
	return userstamp;
}
