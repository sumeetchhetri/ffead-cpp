/*
 * TemplateGenerator.h
 *
 *  Created on: 15-Feb-2013
 *      Author: sumeetc
 */

#ifndef TEMPLATEGENERATOR_H_
#define TEMPLATEGENERATOR_H_
#include "string"
#include "vector"
#include "fstream"
#include "StringUtil.h"
#include "map"
#include "ConfigurationData.h"



class TemplateGenerator {
public:
	TemplateGenerator();
	virtual ~TemplateGenerator();
	static std::string generateTempCd(const std::string&, std::string &, std::string &, const std::string& app);
	static std::string generateTempCdAll(const std::string& serverRootDirectory);
};

#endif /* TEMPLATEGENERATOR_H_ */
