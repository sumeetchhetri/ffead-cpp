/*
 * ConfiguartionData.h
 *
 *  Created on: 19-Jun-2012
 *      Author: sumeetc
 */

#ifndef CONFIGURATIONDATA_H_
#define CONFIGURATIONDATA_H_
#include "string"
#include "ControllerHandler.h"
#include "PropFileReader.h"
#include "SecurityHandler.h"
#include "WsUtil.h"
using namespace std;

class ConfigurationData {
public:
	string key_file,dh_file,ca_list,rand_file,sec_password,srv_auth_prvd,srv_auth_mode,srv_auth_file,ip_address,resourcePath;
	int client_auth;
	map<string, vector<string> > filterMap;
	resFuncMap rstCntMap;
	map<string, string> handoffs;
	map<string, Security> securityObjectMap;
	map<string, Element> formMap;
	strVec dcpsss,cmpnames;
	propMap props,lprops,urlpattMap,urlMap,tmplMap,vwMap,appMap,cntMap,pubMap,mapMap,mappattMap,autMap,autpattMap,wsdlmap,fviewmap;
	long sessionTimeout;
	bool sessatserv;
	ConfigurationData();
	virtual ~ConfigurationData();
};

#endif /* CONFIGURATIONDATA_H_ */
