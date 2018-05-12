/*
 * CommonUtils.cpp
 *
 *  Created on: 10-Dec-2014
 *      Author: sumeetc
 */

#include "CommonUtils.h"

ThreadLocal CommonUtils::contextName;
std::map<std::string, std::string> CommonUtils::mimeTypes;
std::map<std::string, std::string> CommonUtils::locales;
std::string CommonUtils::BLANK = "";
long long CommonUtils::tsPoll = 0;
long long CommonUtils::tsPoll1 = 0;
long long CommonUtils::tsProcess = 0;
long long CommonUtils::tsRead = 0;
long long CommonUtils::tsService = 0;
long long CommonUtils::tsWrite = 0;
long long CommonUtils::tsService1 = 0;
long long CommonUtils::tsService2 = 0;
long long CommonUtils::tsService3 = 0;
long long CommonUtils::tsService4 = 0;
long long CommonUtils::tsService5 = 0;
long long CommonUtils::tsService6 = 0;
long long CommonUtils::tsService7 = 0;
long long CommonUtils::tsService8 = 0;
long long CommonUtils::tsService9 = 0;
long long CommonUtils::tsService10 = 0;
long long CommonUtils::tsService11 = 0;
long long CommonUtils::tsService12 = 0;
long long CommonUtils::cSocks = 0;
long long CommonUtils::cReqs = 0;
long long CommonUtils::cResps = 0;
std::vector<std::string> CommonUtils::appNames;

int CommonUtils::getProcessorCount() {
#if defined(OS_MINGW)
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#elif defined(OS_DARWIN) || defined(OS_BSD)
    int nm[2];
    size_t len = 4;
    uint32_t count;

    nm[0] = CTL_HW; nm[1] = HW_AVAILCPU;
    sysctl(nm, 2, &count, &len, NULL, 0);

    if(count < 1) {
        nm[1] = HW_NCPU;
        sysctl(nm, 2, &count, &len, NULL, 0);
        if(count < 1) { count = 1; }
    }
    return count;
#elif defined(OS_HPUX)
    return  mpctl(MPC_GETNUMSPUS, NULL, NULL);
#elif defined(OS_IRIX)
    return sysconf(_SC_NPROC_ONLN);
#else
    return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

void CommonUtils::addContext(std::string appName) {
	appNames.push_back(appName);
}

void CommonUtils::setAppName(const std::string& appName)
{
	for(int i=0;i<(int)appNames.size();i++) {
		if(appName==appNames.at(i)) {
			contextName.set(&appNames.at(i));
			return;
		}
	}
}

void CommonUtils::loadMimeTypes(const std::string& file)
{
	if(mimeTypes.size()>0)return;
	PropFileReader pread;
	mimeTypes = pread.getProperties(file);
}

const std::string& CommonUtils::getMimeType(const std::string& extension)
{
	if(mimeTypes.find(extension)!=mimeTypes.end())
	{
		return mimeTypes[extension];
	}
	return BLANK;
}

void CommonUtils::loadLocales(const std::string& file)
{
	if(locales.size()>0)return;
	PropFileReader pread;
	locales = pread.getProperties(file);
}

const std::string& CommonUtils::getLocale(const std::string& abbrev)
{
	if(locales.find(abbrev)!=locales.end())
	{
		return locales[abbrev];
	}
	return BLANK;
}

std::string CommonUtils::getAppName(const std::string& appName)
{
	std::string appn = appName;
	if(appn=="" && contextName.get()!=NULL)
	{
		appn = *(std::string*)contextName.get();
	}
	StringUtil::replaceAll(appn, "-", "_");
	RegexUtil::replace(appn, "[^a-zA-Z0-9_]+", "");
	return appn;
}

unsigned long long CommonUtils::charArrayToULongLong(const std::string& l)
{
	unsigned long long t = 0;
	int ind = l.length();
	for (int i = 0; i < ind; i++)
	{
		t = (t << 8) + (l.at(i) & 0xff);
	}
	return t;
}

unsigned long long CommonUtils::charArrayToULongLong(const std::string& l, int ind)
{
	unsigned long long t = 0;
	for (int i = 0; i < ind; i++)
	{
		t = (t << 8) + (l.at(i) & 0xff);
	}
	return t;
}

unsigned long long CommonUtils::charArrayToULongLong(const std::vector<unsigned char>& l)
{
	unsigned long long t = 0;
	int ind = l.size();
	for (int i = 0; i < ind; i++)
	{
		t = (t << 8) + (l.at(i) & 0xff);
	}
	return t;
}

std::string CommonUtils::ulonglongTocharArray(const unsigned long long& lon, const int& provind)
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

	std::string result;
	for (int i = 0; i<ind; i++)
	{
		int offset = (ind - 1 - i) * 8;
		result.push_back((char) ((lon >> offset) & 0xFF));
	}
	return result;
}

void CommonUtils::printBinary(const std::string& encv, const bool& isNL)
{
	for (int var = 0; var < (int)encv.size(); ++var) {
		std::bitset<8> bits((unsigned char)encv.at(var));
		std::cout << bits.to_string();
		if(isNL)
		{
			std::cout << std::endl;
		}
	}
}

std::string CommonUtils::toBinary(const std::string& encv)
{
	std::string decv;
	for (int var = 0; var < (int)encv.size(); ++var) {
		std::bitset<8> bits((unsigned char)encv.at(var));
		decv.append(bits.to_string());
	}
	return decv;
}

void CommonUtils::printMap(const std::map<std::string, std::string>& mp)
{
	std::map<std::string, std::string>::const_iterator it;
	for (it=mp.begin();it!=mp.end();++it) {
		std::cout << it->first << " = " << it->second << std::endl;
	}
}

void CommonUtils::printMap(const std::map<int, std::map<std::string, std::string> >& mp)
{
	std::map<int, std::map<std::string, std::string> >::const_iterator it;
	for (it=mp.begin();it!=mp.end();++it) {
		std::cout << it->first << " = " << it->second.begin()->first << ":" << it->second.begin()->second << std::endl;
	}
}

void CommonUtils::printMap(const std::map<std::string, int>& mp)
{
	std::map<std::string, int>::const_iterator it;
	for (it=mp.begin();it!=mp.end();++it) {
		std::cout << it->first << " = " << it->second << std::endl;
	}
}

void CommonUtils::printHEX(const std::string& encv)
{
	for (int var = 0; var < (int)encv.size(); ++var) {
		char hexVal[20];
		memset(hexVal, 0, sizeof(hexVal));
		sprintf(hexVal, "%x", (unsigned char)encv.at(var));
		std::string hexstr(hexVal);
		if(hexstr.length()==1)
			hexstr = "0" + hexstr;
		std::cout << hexstr;
		if(var%2==1)
			std::cout << " ";
	}
	std::cout << std::endl;
}

std::string CommonUtils::toHEX(const uint32_t& num)
{
	char hexVal[20];
	memset(hexVal, 0, sizeof(hexVal));
	sprintf(hexVal, "%x", num);
	std::string hexstr("0x");
	hexstr.append(hexVal);
	if(hexstr.length()==1)
		hexstr = "0" + hexstr;
	hexstr += "u";
	return hexstr;
}

std::string CommonUtils::xorEncryptDecrypt(const std::string& toEncrypt, const uint32_t& maskingKey) {
	std::string output = toEncrypt;
	std::string maskKeystr = ulonglongTocharArray(maskingKey, 4);

    for (int i = 0; i < (int)toEncrypt.size(); i++)
    {
    	output[i] = toEncrypt[i] ^ maskKeystr[i%4];
    }
    return output;
}

void CommonUtils::listFiles(std::vector<std::string>& files, const std::string& cwd, const std::string& suffix, const bool& isAbs)
{
	files.clear();
	struct dirent *entry;
	DIR *dir;
	dir = opendir(cwd.c_str());
	if(dir==NULL)return;

	while ((entry = readdir (dir)) != NULL) {
		std::string file = isAbs?(cwd+"/"):"";
		file.append(entry->d_name);
		std::string f = cwd + "/" + std::string(entry->d_name);
		struct stat sb;
		stat (f.c_str(), &sb);
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
	closedir(dir);
}

std::vector<std::string> CommonUtils::getFiles(const std::string& cwd, const std::string& suffix, const bool& isAbs)
{
	std::vector<std::string> files;
	listFiles(files, cwd, suffix, isAbs);
	return files;
}
