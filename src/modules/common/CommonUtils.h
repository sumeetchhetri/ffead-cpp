/*
 * CommonUtils.h
 *
 *  Created on: 10-Dec-2014
 *      Author: sumeetc
 */

#ifndef COMMONUTILS_H_
#define COMMONUTILS_H_

#include <dirent.h>
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
#elif defined(OS_DARWIN) || defined(OS_BSD)
#include <sys/param.h>
#include <sys/sysctl.h>
#elif defined(OS_HPUX)
#include <sys/mpctl.h>
#else
#include <unistd.h>
#endif


class CommonUtils {
	static std::string BLANK;
	static ThreadLocal contextName;
	static std::map<std::string, std::string> mimeTypes;
	static std::map<std::string, std::string> locales;
	static std::vector<std::string> appNames;
	friend class ConfigurationHandler;
	static void addContext(std::string appName);
public:
	static void clear();
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
	static const std::string& getMimeType(const std::string& extension);
	static const std::string& getLocale(const std::string& abbrev);
	static std::vector<std::string> getFiles(const std::string& cwd, const std::string& suffix, const bool& isAbs = true);
	static void listFiles(std::vector<std::string>& files, const std::string& cwd, const std::string& suffix, const bool& isAbs = true);
	static long long tsPoll;
	static long long tsPoll1;
	static long long tsProcess;
	static long long tsRead;
	static long long tsService;
	static long long tsService1;
	static long long tsService2;
	static long long tsService3;
	static long long tsService4;
	static long long tsService5;
	static long long tsService6;
	static long long tsService7;
	static long long tsService8;
	static long long tsService9;
	static long long tsService10;
	static long long tsService11;
	static long long tsService12;
	static long long tsWrite;
	static long long cSocks;
	static long long cReqs;
	static long long cResps;
};

#endif /* COMMONUTILS_H_ */
