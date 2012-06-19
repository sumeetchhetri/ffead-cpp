/*
 * ConfigurationHandler.h
 *
 *  Created on: 19-Jun-2012
 *      Author: sumeetc
 */

#ifndef CONFIGURATIONHANDLER_H_
#define CONFIGURATIONHANDLER_H_
#include "PropFileReader.h"
#include "TemplateEngine.h"
#include "Reflection.h"
#include "XmlParser.h"
#include "ComponentGen.h"
#include "ApplicationUtil.h"
#include "WsUtil.h"
#include "DCPGenerator.h"
#include "ControllerHandler.h"
#include "SecurityHandler.h"
#include "ConfiguartionData.h"

class ConfigurationHandler {
public:
	ConfigurationHandler();
	virtual ~ConfigurationHandler();
	static ConfiguartionData handle(strVec webdirs,strVec webdirs1,string incpath,string rtdcfpath,string pubpath,string respath,bool isSSLEnabled);
	static void listi(string cwd,string type,bool apDir,strVec &folders);
};

#endif /* CONFIGURATIONHANDLER_H_ */
