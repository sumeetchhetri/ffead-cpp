/*
 * ConfiguartionData.h
 *
 *  Created on: 19-Jun-2012
 *      Author: sumeetc
 */

#ifndef CONFIGUARTIONDATA_H_
#define CONFIGUARTIONDATA_H_
#include "string"
using namespace std;

class ConfiguartionData {
public:
	string key_file,dh_file,ca_list,rand_file,sec_password,srv_auth_prvd,srv_auth_mode,srv_auth_file;
	int client_auth;
	map<string, vector<string> > filterMap;
	resFuncMap rstCntMap;
	map<string, string> handoffs;
	map<string, Security> securityObjectMap;
	map<string, Element> formMap;
	strVec dcpsss,cmpnames;
	propMap props,lprops,urlpattMap,urlMap,tmplMap,vwMap,appMap,cntMap,pubMap,mapMap,mappattMap,autMap,autpattMap,wsdlmap,fviewmap;
	ConfiguartionData();
	virtual ~ConfiguartionData();
};

#endif /* CONFIGUARTIONDATA_H_ */
