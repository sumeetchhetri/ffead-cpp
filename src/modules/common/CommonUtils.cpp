/*
	Copyright 2009-2020, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (const the& "License");
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
 * CommonUtils.cpp
 *
 *  Created on: 10-Dec-2014
 *      Author: sumeetc
 */

#include "CommonUtils.h"

CommonUtils* CommonUtils::instance = NULL;
const std::string CommonUtils::BLANK = "";
std::atomic<long long> CommonUtils::tsPoll = 0;
std::atomic<long long> CommonUtils::tsPoll1 = 0;
std::atomic<long long> CommonUtils::tsProcess = 0;
std::atomic<long long> CommonUtils::tsReqSockRead = 0;
std::atomic<long long> CommonUtils::tsReqParse = 0;
std::atomic<long long> CommonUtils::tsReqPrsSrvc = 0;
std::atomic<long long> CommonUtils::tsReqTotal = 0;
std::atomic<long long> CommonUtils::tsResSockWrite = 0;
std::atomic<long long> CommonUtils::tsResTotal = 0;
std::atomic<long long> CommonUtils::tsService = 0;
std::atomic<long long> CommonUtils::tsServicePre = 0;
std::atomic<long long> CommonUtils::tsServiceCors = 0;
std::atomic<long long> CommonUtils::tsServiceSec = 0;
std::atomic<long long> CommonUtils::tsServiceFlt = 0;
std::atomic<long long> CommonUtils::tsServiceCnt = 0;
std::atomic<long long> CommonUtils::tsServiceExt = 0;
std::atomic<long long> CommonUtils::tsServicePost = 0;
std::atomic<long long> CommonUtils::tsContMpg = 0;
std::atomic<long long> CommonUtils::tsContPath = 0;
std::atomic<long long> CommonUtils::tsContExt = 0;
std::atomic<long long> CommonUtils::tsContExec = 0;
std::atomic<long long> CommonUtils::tsContRstLkp = 0;
std::atomic<long long> CommonUtils::tsContRstCsiLkp = 0;
std::atomic<long long> CommonUtils::tsContRstInsLkp = 0;
std::atomic<long long> CommonUtils::tsContRstPrsArgs = 0;
std::atomic<long long> CommonUtils::tsContRstExec = 0;
std::atomic<long long> CommonUtils::tsContRstSer = 0;
std::atomic<long long> CommonUtils::cSocks = 0;
std::atomic<long long> CommonUtils::cReqs = 0;
std::atomic<long long> CommonUtils::cResps = 0;
int CommonUtils::g_seed = 0;

const char* CommonUtils::dateStr;

static const char* get_date() {
	time_t t;
	struct tm tm;
	static const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	static __thread char date[52] = "Date: Thu, 01 Jan 1970 00:00:00 GMT\r\nServer: fcpv\r\n";

	time(&t);
	gmtime_r(&t, &tm);
	strftime(date, 51, "Date: ---, %d --- %Y %H:%M:%S GMT\r\nServer: fcpv\r\n", &tm);
	memcpy(date + 6, days[tm.tm_wday], 3);
	memcpy(date + 14, months[tm.tm_mon], 3);

	return date;
}

void CommonUtils::setDate() {
	dateStr = get_date();
}

CommonUtils::CommonUtils() {
}

CommonUtils::~CommonUtils() {
}

CommonUtils* CommonUtils::getInstance() {
	if(instance==NULL) {
		instance = new CommonUtils;
	}
	return instance;
}

void CommonUtils::clearInstance() {
	if(instance!=NULL) {
		delete instance;
	}
}

int CommonUtils::getProcessorCount() {
#if defined(OS_MINGW)
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#elif defined(OS_DARWIN)
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
	getInstance()->appNames.push_back(appName);
}

void CommonUtils::setAppName(const std::string& appName)
{
	for(int i=0;i<(int)getInstance()->appNames.size();i++) {
		if(appName==getInstance()->appNames.at(i)) {
			getInstance()->contextName.reset(&getInstance()->appNames.at(i));
			return;
		}
	}
}

void CommonUtils::loadMimeTypes(const std::string& file)
{
	if(getInstance()->mimeTypes.size()>0)return;
	PropFileReader pread;
	getInstance()->mimeTypes = pread.getProperties(file);
}

const std::string& CommonUtils::getMimeType(const std::string& extension)
{
	if(getInstance()->mimeTypes.find(extension)!=getInstance()->mimeTypes.end())
	{
		return getInstance()->mimeTypes[extension];
	}
	return BLANK;
}

void CommonUtils::loadLocales(const std::string& file)
{
	if(getInstance()->locales.size()>0)return;
	PropFileReader pread;
	getInstance()->locales = pread.getProperties(file);
}

const std::string& CommonUtils::getLocale(const std::string& abbrev)
{
	if(getInstance()->locales.find(abbrev)!=getInstance()->locales.end())
	{
		return getInstance()->locales[abbrev];
	}
	return BLANK;
}

std::string CommonUtils::getAppName(const std::string& appName)
{
	void* tlcn = getInstance()->contextName.get();
	if(tlcn!=NULL)
	{
		return *(std::string*)tlcn;
	}
	return appName;
}

std::string CommonUtils::getAppName(std::string_view appName)
{
	void* tlcn = getInstance()->contextName.get();
	if(tlcn!=NULL)
	{
		return *(std::string*)tlcn;
	}
	return std::string(appName);
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

unsigned long long CommonUtils::btn(char* buf, const int& ind)
{
    unsigned long long t = 0;
    for (int i = 0; i < ind; i++)
    {
        t = (t << 8) + ((unsigned char)buf[i] & 0xff);
    }
    return t;
}

void CommonUtils::ntb(std::string& result, const unsigned long long& lon, int ind)
{
    for (int i = 0; i<ind; i++)
    {
        int offset = (ind - 1 - i) * 8;
        result.push_back((char) ((lon >> offset) & 0xFF));
    }
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

void CommonUtils::printStats() {
	/*Logger logger = LoggerFactory::getLogger("CommonUtils");
	std::string a = ("Connections (Sockets: "+CastUtil::fromNumber(cSocks) + ", Open Sockets: " + CastUtil::fromNumber(SocketInterface::openSocks)
			+", Requests: "+CastUtil::fromNumber(cReqs)+", Responses: "+CastUtil::fromNumber(cResps)+")\n");
	logger.info(a);
	std::string b = ("E-E Total (EL_Pre: "+CastUtil::fromNumber(tsPoll1)+", EL_Wait: "+CastUtil::fromNumber(tsPoll)+
			", EL_Process: "+CastUtil::fromNumber(tsProcess)+", Sock_Read: "+CastUtil::fromNumber(tsReqSockRead)+
			", Req_Parse: "+CastUtil::fromNumber(tsReqParse)+", Req_Prep: "+CastUtil::fromNumber(tsReqTotal-tsReqSockRead-tsReqPrsSrvc)+
			", Service: "+CastUtil::fromNumber(tsService)+", Sock_Write: "+CastUtil::fromNumber(tsResSockWrite)+
			", Res_Prep: "+CastUtil::fromNumber(tsResTotal-tsResSockWrite)+")\n");
	logger.info(b);
	if(cReqs>0) {
		std::string c = ("E-E Average (EL_Pre: "+CastUtil::fromNumber(tsPoll1/cReqs)+", EL_Wait: "+CastUtil::fromNumber(tsPoll/cReqs)+
				", EL_Process: "+CastUtil::fromNumber(tsProcess/cReqs)+", Sock_Read: "+CastUtil::fromNumber(tsReqSockRead/cReqs)+
				", Req_Parse: "+CastUtil::fromNumber(tsReqParse/cReqs)+", Req_Prep: "+CastUtil::fromNumber((tsReqTotal-tsReqSockRead-tsReqPrsSrvc)/cReqs)+
				", Service: "+CastUtil::fromNumber((tsService)/cReqs)+", Sock_Write: "+CastUtil::fromNumber(tsResSockWrite/cReqs)+
				", Res_Prep: "+CastUtil::fromNumber((tsResTotal-tsResSockWrite)/cReqs)+")\n");
		logger.info(c);
	}
	std::string d = ("Service Total (Pre: "+CastUtil::fromNumber(tsServicePre)+", Cors: "+CastUtil::fromNumber(tsServiceCors)+
			", Security: "+CastUtil::fromNumber(tsServiceSec) + ", Filter: "+CastUtil::fromNumber(tsServiceFlt)+
			", Controller: "+CastUtil::fromNumber(tsServiceCnt)+", Ext: "+CastUtil::fromNumber(tsServiceExt)+
			", Post: "+CastUtil::fromNumber(tsServicePost)+")\n");
	logger.info(d);
	if(cReqs>0) {
		std::string e = ("Service Average (Pre: "+CastUtil::fromNumber(tsServicePre/cReqs)+", Cors: "+CastUtil::fromNumber(tsServiceCors/cReqs)+
				", Security: "+ CastUtil::fromNumber(tsServiceSec/cReqs) + ", Filter: "+CastUtil::fromNumber(tsServiceFlt/cReqs)+
				", Controller: "+CastUtil::fromNumber(tsServiceCnt/cReqs)+ ", Ext: "+CastUtil::fromNumber(tsServiceExt/cReqs)+
				", Post: "+CastUtil::fromNumber(tsServicePost/cReqs)+")\n");
		logger.info(e);
	}
	std::string f = ("Controller Total (Cont_Cond: "+CastUtil::fromNumber(tsContMpg)+", Mapg_Cond: "+CastUtil::fromNumber(tsContPath)+
			", Ext_Cond: "+CastUtil::fromNumber(tsContExt) + ", Cont_Exec: "+CastUtil::fromNumber(tsContExec)+
			", Rest_Lkp: "+CastUtil::fromNumber(tsContRstLkp)+", Rest_CsiLkp: "+CastUtil::fromNumber(tsContRstCsiLkp)+
			", Rest_InsLkp: "+CastUtil::fromNumber(tsContRstInsLkp)+", Rest_PrsArgs: "+CastUtil::fromNumber(tsContRstPrsArgs)+
			", Rest_Exec: "+CastUtil::fromNumber(tsContRstExec)+ ", Rest_Ser: "+CastUtil::fromNumber(tsContRstSer)+")\n");
	logger.info(f);
	if(cReqs>0) {
		std::string g = ("Controller Average (Cont_Cond: "+CastUtil::fromNumber(tsContMpg/cReqs)+
				", Mapg_Cond: "+CastUtil::fromNumber(tsContPath/cReqs)+", Ext_Cond: "+CastUtil::fromNumber(tsContExt/cReqs) +
				", Cont_Exec: "+CastUtil::fromNumber(tsContExec/cReqs)+", Rest_Lkp: "+CastUtil::fromNumber(tsContRstLkp/cReqs)+
				", Rest_CsiLkp: "+CastUtil::fromNumber(tsContRstCsiLkp/cReqs)+ ", Rest_InsLkp: "+CastUtil::fromNumber(tsContRstInsLkp/cReqs)+
				", Rest_PrsArgs: "+CastUtil::fromNumber(tsContRstPrsArgs/cReqs)+", Rest_Exec: "+CastUtil::fromNumber(tsContRstExec/cReqs)+
				", Rest_Ser: "+CastUtil::fromNumber(tsContRstSer/cReqs)+")\n");
		logger.info(g);
	}*/
}

std::string CommonUtils::normalizeAppName(const std::string& appName) {
	std::string scappName = appName;
	StringUtil::replaceAll(scappName, "-", "_");
	RegexUtil::replace(scappName, "[^a-zA-Z0-9_]+", "");
	return scappName;
}

std::string CommonUtils::getTpeFnName(const std::string& tpe, const std::string& appName, bool fast) {
	std::string stpe = appName + tpe;
	RegexUtil::replace(stpe,"[/]+","/");
	RegexUtil::replace(stpe,"[^a-zA-Z0-9_]+","");
	return "_" + stpe + "emittTemplateHTML" + (fast?"_s":"");
}

void CommonUtils::getDateStr(std::string& resp) {
	resp.append(dateStr);
}

std::string CommonUtils::getDateStr() {
	return std::string(dateStr);
}

const char* CommonUtils::getDateStrP() {
	return dateStr;
}
