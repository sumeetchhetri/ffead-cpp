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
#include "SocketInterface.h"

class CommonUtils {
	CommonUtils();
	static CommonUtils* instance;
	static std::string_view BLANK;
	ThreadLocal contextName;
	std::map<std::string_view, std::string_view> mimeTypes;
	std::map<std::string_view, std::string_view> locales;
	std::vector<std::string> appNames;
	friend class ConfigurationHandler;
	friend class CHServer;
	static void addContext(std::string appName);
public:
	static void clearInstance();
	static int getProcessorCount();
	static unsigned long long charArrayToULongLong(const std::string& l, int ind);
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
	static void loadMimeTypes(const std::string& file);
	static void loadLocales(const std::string& file);
	static std::string_view getMimeType(std::string_view extension);
	static std::string_view getLocale(std::string_view abbrev);
	static std::vector<std::string> getFiles(const std::string& cwd, const std::string& suffix, const bool& isAbs = true);
	static void listFiles(std::vector<std::string>& files, const std::string& cwd, const std::string& suffix, const bool& isAbs = true);
	static std::atomic<long long> tsPoll;
	static std::atomic<long long> tsPoll1;
	static std::atomic<long long> tsProcess;
	static std::atomic<long long> tsRead;
	static std::atomic<long long> tsWrite;
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
	static std::atomic<long long> cSocks;
	static std::atomic<long long> cReqs;
	static std::atomic<long long> cResps;
	static CommonUtils* getInstance();
	virtual ~CommonUtils();
	static void printStats();
};

#endif /* COMMONUTILS_H_ */
