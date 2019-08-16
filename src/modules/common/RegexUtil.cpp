/*
	Copyright 2009-2012, Sumeet Chhetri

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
 * RegexUtil.cpp
 *
 *  Created on: 14-Aug-2012
 *      Author: sumeetc
 */

#include "RegexUtil.h"

cuckoohash_map<std::string_view, regex_t*> RegexUtil::patterns;
cuckoohash_map<std::string_view, regex_t*> RegexUtil::nlpatterns;
bool RegexUtil::cacheRegexes = true;

void RegexUtil::flushCache() {
	if(patterns.size()>0) {
		auto lt = patterns.lock_table();
		cuckoohash_map<std::string_view, regex_t*>::locked_table::iterator it;
		for(it=lt.begin();it!=lt.end();++it) {
			regfree(it->second);
			delete it->second;
		}
	}
	if(nlpatterns.size()>0) {
		auto lt = nlpatterns.lock_table();
		cuckoohash_map<std::string_view, regex_t*>::locked_table::iterator it;
		for(it=lt.begin();it!=lt.end();++it) {
			regfree(it->second);
			delete it->second;
		}
	}
}

bool RegexUtil::isValidRegex(std::string_view pattern) {
	return getRegex(pattern, false)!=NULL;
}

regex_t* RegexUtil::getRegex(std::string_view pattern, const bool& matchNewLine) {
	regex_t* regex = NULL;
	bool found = false;
	if(cacheRegexes) {
		if(!matchNewLine && patterns.contains(pattern)) {
			return patterns.find(pattern);
		} else if(nlpatterns.contains(pattern)) {
			return nlpatterns.find(pattern);
		}
	}
	if(!found) {
		int cflags = REG_EXTENDED;
		if(matchNewLine)
			cflags = REG_NEWLINE | REG_EXTENDED;

		if(cacheRegexes)
		{
			if(!matchNewLine) {
				regex = new regex_t;
				patterns.insert(pattern, regex);
			} else {
				regex = new regex_t;
				nlpatterns.insert(pattern, regex);
			}
			int reti = regcomp(regex, &pattern[0], cflags);
			if(reti!=0)
			{
				std::cout << ("Could not compile regex - "+std::string(pattern) + " failed with error ") << reti << std::endl;
			}
		}
		else
		{
			regex = new regex_t;
			int reti = regcomp(regex, &pattern[0], cflags);
			if(reti!=0)
			{
				std::cout << ("Could not compile regex - "+std::string(pattern) + " failed with error ") << reti << std::endl;
			}
		}
	}
	return regex;
}

void RegexUtil::find(std::string_view text, std::string_view pattern, int &spos, int &epos, const bool& matchNewLine/* = false*/)
{
	std::string ttext(text);
	regex_t* regex = getRegex(pattern, matchNewLine);
	spos = -1;
	epos = -1;
	regmatch_t pm;
	int reti = regexec(regex, ttext.c_str(), 1, &pm, 0);
	if(!cacheRegexes) {
		regfree(regex);
		delete regex;
	}
	if (reti == 0) {    /* while matches found */
		/* substring found between pm.rm_so and pm.rm_eo */
		/* This call to regexec() finds the next match */
		spos = pm.rm_so;
		epos = pm.rm_eo;
	}
}

bool RegexUtil::matches(std::string_view text, std::string_view pattern, const bool& matchNewLine/* = false*/)
{
	std::string ttext(text);
	regex_t* regex = getRegex(pattern, matchNewLine);
	regmatch_t pm;
	int reti = regexec(regex, ttext.c_str(), 1, &pm, 0);
	if(!cacheRegexes) {
		regfree(regex);
		delete regex;
	}
	if (reti == 0) {    /* while matches found */
		return true;
	}
	return false;
}

int RegexUtil::find(std::string_view text, std::string_view pattern, const bool& matchNewLine/* = false*/)
{
	std::string ttext(text);
	regex_t* regex = getRegex(pattern, matchNewLine);
	regmatch_t pm;
	int reti = regexec(regex, ttext.c_str(), 1, &pm, 0);
	if(!cacheRegexes) {
		regfree(regex);
		delete regex;
	}
	if (reti == 0) {    /* while matches found */
		/* substring found between pm.rm_so and pm.rm_eo */
		/* This call to regexec() finds the next match */
		return pm.rm_so;
	}
	return -1;
}

std::vector<std::string> RegexUtil::search(std::string_view text, std::string_view pattern, const bool& matchNewLine/* = false*/) {
	std::vector<std::string> vec;
	std::string ttext(text);
	regex_t* regex = getRegex(pattern, matchNewLine);
	regmatch_t pm;
	int reti = regexec(regex, ttext.c_str(), 1, &pm, 0);
	while (reti == 0) {    /* while matches found */
		/* substring found between pm.rm_so and pm.rm_eo */
		/* This call to regexec() finds the next match */
		if(!reti) {
			std::string match;
			match = ttext.substr(pm.rm_so, pm.rm_eo-pm.rm_so);
			vec.push_back(match);
		} else {
			break;
		}
		ttext = ttext.substr(pm.rm_eo);
		pm.rm_eo = -1;
		pm.rm_so = -1;
		reti = regexec (regex, ttext.c_str(), 1, &pm, 0);
	}
	if(!cacheRegexes) {
		regfree(regex);
		delete regex;
	}
	return vec;
}

std::string RegexUtil::replaceCopy(std::string_view text, std::string_view pattern, std::string_view with, const bool& matchNewLine/* = false*/) {
	std::string ttext(text);
	std::string rettxt;
	regex_t* regex = getRegex(pattern, matchNewLine);
	regmatch_t pm;
	int reti = regexec(regex, ttext.c_str(), 1, &pm, 0);
	while (reti == 0) {    /* while matches found */
		/* substring found between pm.rm_so and pm.rm_eo */
		/* This call to regexec() finds the next match */
		if(!reti) {
			std::string match;
			match = ttext.substr(pm.rm_so, pm.rm_eo-pm.rm_so);
			rettxt += ttext.substr(0, pm.rm_so) + std::string(with);
		} else {
			rettxt += ttext;
			break;
		}
		ttext = ttext.substr(pm.rm_eo);
		pm.rm_eo = -1;
		pm.rm_so = -1;
		reti = regexec (regex, ttext.c_str(), 1, &pm, 0);
	}
	if(!cacheRegexes) {
		regfree(regex);
		delete regex;
	}
	if(ttext!="")rettxt += ttext;

	if(text!=rettxt)
	{
		while(replace(rettxt, pattern, with, matchNewLine)) {}
	}
	return rettxt;
}

bool RegexUtil::replace(std::string& text, std::string_view pattern, std::string_view with, const bool& matchNewLine/* = false*/) {
	std::string ttext(text);
	std::string rettxt;
	regex_t* regex = getRegex(pattern, matchNewLine);
	regmatch_t pm;
	int reti = regexec(regex, ttext.c_str(), 1, &pm, 0);
	while (reti == 0) {    /* while matches found */
		/* substring found between pm.rm_so and pm.rm_eo */
		/* This call to regexec() finds the next match */
		if(!reti) {
			std::string match;
			match = ttext.substr(pm.rm_so, pm.rm_eo-pm.rm_so);
			rettxt += ttext.substr(0, pm.rm_so) + std::string(with);
		} else {
			rettxt += ttext;
			break;
		}
		ttext = ttext.substr(pm.rm_eo);
		pm.rm_eo = -1;
		pm.rm_so = -1;
		reti = regexec (regex, ttext.c_str(), 1, &pm, 0);
	}
	if(!cacheRegexes) {
		regfree(regex);
		delete regex;
	}
	if(ttext!="")rettxt += ttext;
	if(text==rettxt)
		return false;
	text = rettxt;
	while(replace(text, pattern, with, matchNewLine)) {}
	return true;
}

std::vector<std::string> RegexUtil::findWithGroups(std::string_view text, std::string_view pattern, const bool& matchNewLine) {
	std::vector<std::string> data;
	std::string ttext(text);
	regex_t* regex = getRegex(pattern, matchNewLine);
	regmatch_t pm[10];
	int reti = regexec(regex, ttext.c_str(), 10, pm, 0);
	if(!cacheRegexes) {
		regfree(regex);
		delete regex;
	}
	if (reti == 0) {    /* while matches found */
		for (int i = 0; pm[i].rm_so != -1; i++)
		{
			std::string co = ttext.substr(pm[i].rm_so, pm[i].rm_eo-pm[i].rm_so);
			data.push_back(co);
		}
	}
	return data;
}

std::vector<std::string> RegexUtil::findWithGroups(std::string_view text, std::string_view pattern, const int& groupCount, const bool& matchNewLine /*= false*/) {
	std::vector<std::string> data;
	std::string ttext(text);
	regex_t* regex = getRegex(pattern, matchNewLine);
	regmatch_t pm[groupCount];
	int reti = regexec(regex, ttext.c_str(), groupCount, pm, 0);
	if(!cacheRegexes) {
		regfree(regex);
		delete regex;
	}
	if (reti == 0) {    /* while matches found */
		for (int i = 0; pm[i].rm_so != -1; i++)
		{
			std::string co = ttext.substr(pm[i].rm_so, pm[i].rm_eo-pm[i].rm_so);
			data.push_back(co);
		}
	}
	return data;
}
