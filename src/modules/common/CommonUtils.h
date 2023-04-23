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
 * CommonUtils.h
 *
 *  Created on: 10-Dec-2014
 *      Author: sumeetc
 */

#ifndef COMMONUTILS_H_
#define COMMONUTILS_H_

#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include "vector"
#include "string"
#include "map"
#include "iostream"
#include "bitset"
#include "cstring"
#include <stdio.h>
#include <stdint.h>
#include "ThreadLocal.h"
#include "PropFileReader.h"
#include "RegexUtil.h"
#if defined(OS_MINGW)
#include <windows.h>
#elif defined(OS_DARWIN)
#include <sys/param.h>
#include <sys/sysctl.h>
#elif defined(OS_HPUX)
#include <sys/mpctl.h>
#else
#include <unistd.h>
#endif
#include "CastUtil.h"
#include "LoggerFactory.h"
#include "HTTPResponseStatus.h"
#include "ClassInfo.h"

struct ClassBeanIns {
	ClassInfo* clas;
	void* instance;
	bool cleanUp;
};

typedef void (*GetClassBeanIns) (std::string& clsn, std::string appn, ClassBeanIns* cbi);

class CommonUtils {
	CommonUtils();
	static CommonUtils* instance;
	ThreadLocal contextName;
	std::map<std::string, std::string> mimeTypes;
	std::map<std::string, std::string> locales;
	std::vector<std::string> appNames;
	static const char* dateStr;
	friend class ConfigurationHandler;
	friend class CHServer;
	friend class ServiceHandler;
	friend class RequestHandler2;
	friend class RequestReaderHandler;
	static void setDate();
	static void addContext(std::string appName);
	static int g_seed;
public:
	static const std::string BLANK;
	static std::string getDateStr();
	static const char* getDateStrP();
	static void getDateStr(std::string&);
	static void clearInstance();
	static int getProcessorCount();
	static unsigned long long charArrayToULongLong(const std::string& l, int ind);
	static unsigned long long btn(char* buf, const int& ind);
	static void ntb(std::string& result, const unsigned long long& lon, int ind);
	static unsigned long long charArrayToULongLong(const std::string& l);
	static std::string ulonglongTocharArray(const unsigned long long& lon, const int& provind= -1);
	static unsigned long long charArrayToULongLong(const std::vector<unsigned char>& l);
	static std::string xorEncryptDecrypt(const std::string& toEncrypt, const uint32_t& maskingKey);
	static void printBinary(const std::string& encv, const bool& isNL= true);
	static std::string toBinary(const std::string& encv);
	static void printMap(const std::map<std::string, std::string>& mp);
	static void printMap(const std::map<int, std::map<std::string, std::string> >& mp);
	static void printMap(const std::map<std::string, int>& mp);
	static void printHEX(const std::string& encv);
	static std::string toHEX(const uint32_t& num);
	static void setAppName(const std::string& appName);
	static std::string getAppName(const std::string& appName = "");
	static std::string getAppName(std::string_view);
	static void loadMimeTypes(const std::string& file);
	static void loadLocales(const std::string& file);
	static const std::string& getMimeType(const std::string& extension);
	static const std::string& getLocale(const std::string& abbrev);
	static std::vector<std::string> getFiles(const std::string& cwd, const std::string& suffix, const bool& isAbs = true);
	static void listFiles(std::vector<std::string>& files, const std::string& cwd, const std::string& suffix, const bool& isAbs = true);
	static std::atomic<long long> tsPoll;
	static std::atomic<long long> tsPoll1;
	static std::atomic<long long> tsProcess;
	static std::atomic<long long> cSocks;
	static std::atomic<long long> cReqs;
	static std::atomic<long long> cResps;

	static std::atomic<long long> tsReqSockRead;
	static std::atomic<long long> tsReqParse;
	static std::atomic<long long> tsReqPrsSrvc;
	static std::atomic<long long> tsReqTotal;

	static std::atomic<long long> tsResSockWrite;
	static std::atomic<long long> tsResTotal;

	static std::atomic<long long> tsService;
	static std::atomic<long long> tsServicePre;
	static std::atomic<long long> tsServiceCors;
	static std::atomic<long long> tsServiceSec;
	static std::atomic<long long> tsServiceFlt;
	static std::atomic<long long> tsServiceCnt;
	static std::atomic<long long> tsServiceExt;
	static std::atomic<long long> tsServicePost;
	static std::atomic<long long> tsContMpg;
	static std::atomic<long long> tsContPath;
	static std::atomic<long long> tsContExt;
	static std::atomic<long long> tsContExec;
	static std::atomic<long long> tsContRstLkp;
	static std::atomic<long long> tsContRstCsiLkp;
	static std::atomic<long long> tsContRstInsLkp;
	static std::atomic<long long> tsContRstPrsArgs;
	static std::atomic<long long> tsContRstExec;
	static std::atomic<long long> tsContRstSer;

	static CommonUtils* getInstance();
	virtual ~CommonUtils();
	static void printStats();
	static std::string normalizeAppName(const std::string& appName);
	static std::string getTpeFnName(const std::string& tpe, const std::string& appName, bool fast = true);
	//Copied from https://stackoverflow.com/questions/1640258/need-a-fast-random-generator-for-c
	static inline int fastrand() {
		g_seed = (214013*g_seed+2531011);
		return (g_seed>>16)&0x7FFF;
	}
	static inline int fastrand(int& g_seed) {
		g_seed = (214013*g_seed+2531011);
		return (g_seed>>16)&0x7FFF;
	}
	//https://stackoverflow.com/questions/9631225/convert-strings-specified-by-length-not-nul-terminated-to-int-float
	static inline bool fastStrToNum(const char* str, int len, int& ret) {
	    ret = 0;
	    for(int i = 0; i < len; ++i)
	    {
	    	if(!isdigit(str[i])) return false;
	        ret = ret * 10 + (str[i] - '0');
	    }
	    return true;
	}
	//https://tinodidriksen.com/2010/02/cpp-convert-string-to-int-speed/
	static inline bool naiveStrToNum(const char* p, int len, int& ret) {
	    ret = 0;
	    int c = 0;
	    while (*p >= '0' && *p <= '9') {
	        ret = (ret*10) + (*p - '0');
	        ++p;
	        if(++c==len) break;
	    }
	    if (c!=len) {
	        return false;
	    }
	    return true;
	}
};

#endif /* COMMONUTILS_H_ */
