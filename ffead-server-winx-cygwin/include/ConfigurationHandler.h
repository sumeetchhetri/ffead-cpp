/*
 * ConfigurationHandler.h
 *
 *  Created on: 19-Jun-2012
 *      Author: sumeetc
 */

#ifndef CONFIGURATIONHANDLER_H_
#define CONFIGURATIONHANDLER_H_
#include "TemplateEngine.h"
#include "Reflection.h"
#include "XmlParser.h"
#include "ComponentGen.h"
#include "ApplicationUtil.h"
#include "DCPGenerator.h"
#include "ConfigurationData.h"

class ConfigurationHandler {
public:
	ConfigurationHandler();
	virtual ~ConfigurationHandler();
	static ConfigurationData handle(strVec webdirs,strVec webdirs1,string incpath,string rtdcfpath,string pubpath,string respath,bool isSSLEnabled);
	static void listi(string cwd,string type,bool apDir,strVec &folders);
};

#endif /* CONFIGURATIONHANDLER_H_ */
