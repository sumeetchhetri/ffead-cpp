/*
 * CommonUtils.cpp
 *
 *  Created on: 10-Dec-2014
 *      Author: sumeetc
 */

#include "CommonUtils.h"

ThreadLocal CommonUtils::contextName;
map<string, string> CommonUtils::mimeTypes;
map<string, string> CommonUtils::locales;
string CommonUtils::BLANK = "";

void CommonUtils::setAppName(const string& appName)
{
	if(contextName.get()==NULL)
	{
		contextName.set(new string(appName));
	}
	else
	{
		string* stv = (string*)contextName.get();
		*stv = appName;
		contextName.set(stv);
	}
}

void CommonUtils::loadMimeTypes(const string& file)
{
	if(mimeTypes.size()>0)return;
	PropFileReader pread;
	mimeTypes = pread.getProperties(file);
}

const string& CommonUtils::getMimeType(const string& extension)
{
	if(mimeTypes.find(extension)!=mimeTypes.end())
	{
		return mimeTypes[extension];
	}
	return BLANK;
}

void CommonUtils::loadLocales(const string& file)
{
	if(locales.size()>0)return;
	PropFileReader pread;
	locales = pread.getProperties(file);
}

const string& CommonUtils::getLocale(const string& abbrev)
{
	if(locales.find(abbrev)!=locales.end())
	{
		return locales[abbrev];
	}
	return BLANK;
}

string CommonUtils::getAppName(const string& appName)
{
	string appn = appName;
	if(appn=="" && contextName.get()!=NULL)
	{
		appn = *(string*)contextName.get();
	}
	StringUtil::replaceAll(appn, "-", "_");
	RegexUtil::replace(appn, "[^a-zA-Z0-9_]+", "");
	return appn;
}

unsigned long long CommonUtils::charArrayToULongLong(const string& l)
{
	unsigned long long t = 0;
	int ind = l.length();
	for (int i = 0; i < ind; i++)
	{
		t = (t << 8) + (l.at(i) & 0xff);
	}
	return t;
}

unsigned long long CommonUtils::charArrayToULongLong(const string& l, int ind)
{
	unsigned long long t = 0;
	for (int i = 0; i < ind; i++)
	{
		t = (t << 8) + (l.at(i) & 0xff);
	}
	return t;
}

unsigned long long CommonUtils::charArrayToULongLong(const vector<unsigned char>& l)
{
	unsigned long long t = 0;
	int ind = l.size();
	for (int i = 0; i < ind; i++)
	{
		t = (t << 8) + (l.at(i) & 0xff);
	}
	return t;
}

string CommonUtils::ulonglongTocharArray(const unsigned long long& lon, const int& provind)
{
	int ind;
	if(lon<256)
	{
		ind = 1;
	}
	else if(lon<65536)
	{
		ind = 2;
	}
	else if(lon<16777216)
	{
		ind = 3;
	}
	else if(lon<4294967296ULL)
	{
		ind = 4;
	}
	else if(lon<1099511627776ULL)
	{
		ind = 5;
	}
	else if(lon<281474976710656ULL)
	{
		ind = 6;
	}
	else if(lon<72057594037927936ULL)
	{
		ind = 7;
	}
	else
	{
		ind = 8;
	}

	if(provind!=-1 && ind<provind) {
		ind = provind;
	}

	string result;
	for (int i = 0; i<ind; i++)
	{
		int offset = (ind - 1 - i) * 8;
		result.push_back((char) ((lon >> offset) & 0xFF));
	}
	return result;
}

void CommonUtils::printBinary(const string& encv, const bool& isNL)
{
	for (int var = 0; var < (int)encv.size(); ++var) {
		bitset<8> bits((unsigned char)encv.at(var));
		cout << bits.to_string();
		if(isNL)
		{
			cout << endl;
		}
	}
}

string CommonUtils::toBinary(const string& encv)
{
	string decv;
	for (int var = 0; var < (int)encv.size(); ++var) {
		bitset<8> bits((unsigned char)encv.at(var));
		decv.append(bits.to_string());
	}
	return decv;
}

void CommonUtils::printMap(const map<string, string>& mp)
{
	map<string, string>::const_iterator it;
	for (it=mp.begin();it!=mp.end();++it) {
		cout << it->first << " = " << it->second << endl;
	}
}

void CommonUtils::printMap(const map<int, map<string, string> >& mp)
{
	map<int, map<string, string> >::const_iterator it;
	for (it=mp.begin();it!=mp.end();++it) {
		cout << it->first << " = " << it->second.begin()->first << ":" << it->second.begin()->second << endl;
	}
}

void CommonUtils::printMap(const map<string, int>& mp)
{
	map<string, int>::const_iterator it;
	for (it=mp.begin();it!=mp.end();++it) {
		cout << it->first << " = " << it->second << endl;
	}
}

void CommonUtils::printHEX(const string& encv)
{
	for (int var = 0; var < (int)encv.size(); ++var) {
		char hexVal[20];
		memset(hexVal, 0, sizeof(hexVal));
		sprintf(hexVal, "%x", (unsigned char)encv.at(var));
		string hexstr(hexVal);
		if(hexstr.length()==1)
			hexstr = "0" + hexstr;
		cout << hexstr;
		if(var%2==1)
			cout << " ";
	}
	cout << endl;
}

string CommonUtils::toHEX(const uint32_t& num)
{
	char hexVal[20];
	memset(hexVal, 0, sizeof(hexVal));
	sprintf(hexVal, "%x", num);
	string hexstr("0x");
	hexstr.append(hexVal);
	if(hexstr.length()==1)
		hexstr = "0" + hexstr;
	hexstr += "u";
	return hexstr;
}

string CommonUtils::xorEncryptDecrypt(const string& toEncrypt, const uint32_t& maskingKey) {
	string output = toEncrypt;
	string maskKeystr = ulonglongTocharArray(maskingKey, 4);

    for (int i = 0; i < (int)toEncrypt.size(); i++)
    {
    	output[i] = toEncrypt[i] ^ maskKeystr[i%4];
    }
    return output;
}

void CommonUtils::listFiles(vector<string>& files, const string& cwd, const string& suffix, const bool& isAbs)
{
	files.clear();
	struct dirent *entry;
	DIR *dir;
	dir = opendir(cwd.c_str());
	if(dir==NULL)return;

	while ((entry = readdir (dir)) != NULL) {
		string file = isAbs?(cwd+"/"):"";
		file.append(entry->d_name);
		struct stat sb;
		stat (file.c_str(), &sb);
		if(S_ISREG(sb.st_mode) && StringUtil::endsWith(file, suffix)) {
			RegexUtil::replace(file,"[/]+","/");
			files.push_back(file);
		} else if(S_ISDIR(sb.st_mode) && suffix=="/") {
			if(isAbs && !StringUtil::endsWith(file, "/.") && !StringUtil::endsWith(file, "/..")) {
				file += "/";
				RegexUtil::replace(file,"[/]+","/");
				files.push_back(file);
			} else if(!isAbs && file!="." && file!="..") {
				files.push_back(file);
			}
		}
	}
}

vector<string> CommonUtils::getFiles(const string& cwd, const string& suffix, const bool& isAbs)
{
	vector<string> files;
	listFiles(files, cwd, suffix, isAbs);
	return files;
}
