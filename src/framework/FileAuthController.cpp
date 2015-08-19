/*
 * FileAuthController.cpp
 *
 *  Created on: Nov 23, 2010
 *      Author: sumeet
 */

#include "FileAuthController.h"

FileAuthController::FileAuthController(const string& filename, const string& delimiter) {
	this->filename = filename;
	this->delimiter = delimiter;
}

FileAuthController::~FileAuthController() {
	// TODO Auto-generated destructor stub
}

string FileAuthController::treat_password(const string& password)
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

bool FileAuthController::authenticate(const string& username, const string& password)
{
	string userstamp = (username + delimiter + treat_password(password));
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

bool FileAuthController::getPassword(const string& username, string &passwd)
{
	bool passwdf = false;
	ifstream ifs(this->filename.c_str());
	if(ifs.is_open() && username!="")
	{
		string temp;
		while(getline(ifs, temp))
		{
			vector<string> tempv;
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

string FileAuthController::getUserRole(const string& username)
{
	//bool passwdf = false;
	ifstream ifs(this->filename.c_str());
	if(ifs.is_open() && username!="")
	{
		string temp;
		while(getline(ifs, temp))
		{
			vector<string> tempv;
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
	string blnk;
	return blnk;
}

string FileAuthController::get(const string& username, const int& pos)
{
	//bool passwdf = false;
	ifstream ifs(this->filename.c_str());
	if(ifs.is_open() && username!="")
	{
		string temp;
		while(getline(ifs, temp))
		{
			vector<string> tempv;
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
	string blnk;
	return blnk;
}
