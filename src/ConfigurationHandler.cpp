/*
	Copyright 2010, Sumeet Chhetri

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
 * ConfigurationHandler.cpp
 *
 *  Created on: 19-Jun-2012
 *      Author: sumeetc
 */

#include "ConfigurationHandler.h"

ConfigurationHandler::ConfigurationHandler() {
	// TODO Auto-generated constructor stub

}

ConfigurationHandler::~ConfigurationHandler() {
	// TODO Auto-generated destructor stub
}


void ConfigurationHandler::listi(string cwd,string type,bool apDir,strVec &folders)
{
	FILE *pipe_fp;
	string command;
	command = ("ls -F1 "+cwd+"|grep '"+type+"'");
	cout << "\nCommand:" << command << flush;
	if ((pipe_fp = popen(command.c_str(), "r")) == NULL)
	{
		printf("pipe open error in cmd_list\n");
	}
	int t_char;
	string folderName;
	while ((t_char = fgetc(pipe_fp)) != EOF)
	{
		if(t_char!='\n')
		{
			stringstream ss;
			ss << (char)t_char;
			string temp;
			ss >> temp;
			folderName.append(temp);
		}
		else if(folderName!="")
		{
			boost::replace_first(folderName,"*","");
			if(folderName.find("~")==string::npos)
			{
				cout << "\nlist for file" << (cwd+"/"+folderName) << "\n" << flush;
				if(apDir)
					folders.push_back(cwd+folderName);
				else
					folders.push_back(folderName);
			}
			folderName = "";
		}
	}
	pclose(pipe_fp);
}

ConfigurationData ConfigurationHandler::handle(strVec webdirs,strVec webdirs1,string incpath,string rtdcfpath,string pubpath,string respath,bool isSSLEnabled)
{
	ConfigurationData configurationData;
	configurationData.resourcePath = respath;
	strVec all,dcps,afcd,appf,wspath,compnts,handoffVec;
	string includeRef;
	TemplateEngine templ;
	Context cntxt;
	string libs,ilibs,isrcs,iobjs,ideps;
	Reflection ref;
	vector<bool> stat;
	strVec vecvp,pathvec;
	propMap srp;
	PropFileReader pread;
	XmlParser parser("Parser");
	ComponentGen gen;
	if(isSSLEnabled)
	{
		propMap sslsec = pread.getProperties(respath+"/security.prop");
		if(sslsec.size()>0)
		{
			configurationData.key_file = sslsec["KEYFILE"];
			configurationData.dh_file = sslsec["DHFILE"];
			configurationData.ca_list = sslsec["CA_LIST"];
			configurationData.rand_file = sslsec["RANDOM"];
			configurationData.sec_password = sslsec["PASSWORD"];
			string tempcl = sslsec["CLIENT_SEC_LEVEL"];
			configurationData.srv_auth_prvd = sslsec["SRV_AUTH_PRVD"];
			configurationData.srv_auth_mode = sslsec["SRV_AUTH_MODE"];
			configurationData.srv_auth_file = sslsec["SRV_AUTH_FILE"];
			if(tempcl!="")
			{
				try
				{
					configurationData.client_auth = boost::lexical_cast<int>(tempcl);
				}
				catch(...)
				{
					cout << "\nInvalid client auth level defined" << flush;
					configurationData.client_auth = 1;
				}
			}
		}
	}
	string rundyncontent;
	for(unsigned int var=0;var<webdirs.size();var++)
	{
		//cout <<  webdirs.at(0) << flush;
		string defpath = webdirs.at(var);
		string dcppath = defpath + "dcp/";
		string cmppath = defpath + "components/";
		string usrincludes = defpath + "include/";
		//propMap srp = pread.getProperties(defpath+"config/app.prop");

		string name = webdirs1.at(var);
		boost::replace_all(name,"/","");
		rundyncontent += "cp -Rf $FEAD_CPP_PATH/public/* $FEAD_CPP_PATH/web/"+name+"/public/\n";
		configurationData.cntMap[name] = "true";
		listi(dcppath,".dcp",true,dcps);
		listi(cmppath,".cmp",true,compnts);
		all.push_back(usrincludes);
		appf.push_back(defpath+"app.xml");

		libs += ("-l"+ name+" ");
		ilibs += ("-I" + usrincludes+" ");
		wspath.push_back(name);

		Element root = parser.getDocument(defpath+"config/application.xml").getRootElement();
		if(root.getTagName()=="app" && root.getChildElements().size()>0)
		{
			ElementList eles = root.getChildElements();
			for (unsigned int apps = 0; apps < eles.size(); apps++)
			{
				if(eles.at(apps).getTagName()=="controllers")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="controller")
						{
							string url = cntrls.at(cntn).getAttribute("url");
							string clas = cntrls.at(cntn).getAttribute("class");
							if(url!="" && clas!="")
							{
								if(cntrls.at(cntn).getAttribute("url").find("*")!=string::npos)
								{
									if(url=="*.*")
										configurationData.urlpattMap[name+url] = clas;
									else
									{
										url = url.substr(url.find("*")+1);
										configurationData.urlMap[name+url] = clas;
									}
								}
								else if(clas!="")
									configurationData.urlMap[name+url] = clas;
								cout << "adding controller => " << name << url << " :: " << clas << endl;
							}
							else
							{
								string from = cntrls.at(cntn).getAttribute("from");
								string to = cntrls.at(cntn).getAttribute("to");
								if(to.find("*")!=string::npos && to!="")
									to = to.substr(to.find("*")+1);
								if(from.find("*")!=string::npos && to!="")
								{
									if(from=="*.*")
										configurationData.mappattMap[name+from] = to;
									else
									{
										from = from.substr(from.find("*")+1);
										configurationData.mapMap[name+from] = to;
									}
								}
								else if(to!="")
								{
									configurationData.mapMap[name+from] = to;
								}
								cout << "adding mapping => " << name << from << " :: " << to << endl;
							}
						}
					}
				}
				else if(eles.at(apps).getTagName()=="authhandlers")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="authhandler")
						{
							string url = cntrls.at(cntn).getAttribute("url");
							string provider = cntrls.at(cntn).getAttribute("provider");
							if(url!="" && provider!="")
							{
								if(url.find("*")!=string::npos)
								{
									if(url=="*.*")
										configurationData.autpattMap[name+url] = provider;
									else
									{
										url = url.substr(url.find("*")+1);
										configurationData.autMap[name+url] = provider;
									}
								}
								else if(provider!="")
									configurationData.autMap[name+url] = provider;
								cout << "adding authhandler => " << name << url << " :: " << provider << endl;
							}
						}
					}
				}
				else if(eles.at(apps).getTagName()=="filters")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="filter")
						{
							string url = cntrls.at(cntn).getAttribute("url");
							string clas = cntrls.at(cntn).getAttribute("class");
							string type = cntrls.at(cntn).getAttribute("type");
							if(clas!="" && (type=="in" || type=="out"))
							{
								if(url=="")url="*.*";
								if(url.find("*")!=string::npos)
								{
									if(url=="*.*")
									{
										configurationData.filterMap[name+url+type].push_back(clas);
									}
									else
									{
										url = url.substr(url.find("*")+1);
										configurationData.filterMap[name+url+type].push_back(clas);
									}
								}
								cout << "adding filter => " << name << url << type << " :: " << clas << endl;
							}
						}
					}
				}
				else if(eles.at(apps).getTagName()=="templates")
				{
					ElementList tmplts = eles.at(apps).getChildElements();
					for (unsigned int tmpn = 0; tmpn < tmplts.size(); tmpn++)
					{
						if(tmplts.at(tmpn).getTagName()=="template")
						{
							configurationData.tmplMap[name+tmplts.at(tmpn).getAttribute("file")] = tmplts.at(tmpn).getAttribute("class");
							//cout << tmplts.at(tmpn).getAttribute("file") << " :: " << tmplts.at(tmpn).getAttribute("class") << flush;
						}
					}
				}
				else if(eles.at(apps).getTagName()=="dviews")
				{
					ElementList dvs = eles.at(apps).getChildElements();
					for (unsigned int dn = 0; dn < dvs.size(); dn++)
					{
						if(dvs.at(dn).getTagName()=="dview")
						{
							configurationData.vwMap[name+dvs.at(dn).getAttribute("path")] = dvs.at(dn).getAttribute("class");
							//cout << dvs.at(dn).getAttribute("path") << " :: " << dvs.at(dn).getAttribute("class") << flush;
						}
					}
				}
				else if(eles.at(apps).getTagName()=="restcontrollers")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="restcontroller")
						{
							string url = cntrls.at(cntn).getAttribute("urlpath");
							string clas = cntrls.at(cntn).getAttribute("class");
							string rname = cntrls.at(cntn).getAttribute("name");
							ElementList resfuncs = cntrls.at(cntn).getChildElements();
							for (unsigned int cntn1 = 0; cntn1 < resfuncs.size(); cntn1++)
							{
								if(resfuncs.at(cntn1).getTagName()=="restfunction")
								{
									RestFunction restfunction;
									restfunction.name = resfuncs.at(cntn1).getAttribute("name");
									restfunction.alias = resfuncs.at(cntn1).getAttribute("alias");
									restfunction.clas = clas;
									restfunction.meth = resfuncs.at(cntn1).getAttribute("meth");
									restfunction.baseUrl = resfuncs.at(cntn1).getAttribute("baseUrl");
									ElementList resfuncparams = resfuncs.at(cntn1).getChildElements();
									for (unsigned int cntn2 = 0; cntn2 < resfuncparams.size(); cntn2++)
									{
										if(resfuncparams.at(cntn2).getTagName()=="param")
										{
											RestFunctionParams param;
											/*try
											{
												param.pos = boost::lexical_cast<int>(resfuncparams.at(cntn2).getAttribute("pos"));
											} catch(...) {
												cout << "CONFIGURATION_ERROR-> Invalid pos attribute specified for function "
														<< restfunction.name << ",pos value should be an integer." << endl;
											}*/
											param.type = resfuncparams.at(cntn2).getAttribute("type");
											restfunction.params.push_back(param);
										}
									}
									//if(restfunction.params.size()==0)
									//	continue;
									if(clas!="")
									{
										if(url.find("*")==string::npos)
										{
											if(url=="")
												url = "/";
											string urlmpp;
											if(rname!="")
											{
												if(restfunction.alias!="")
												{
													if(restfunction.baseUrl=="")
														urlmpp = name+url+rname+"/"+restfunction.alias;
													else
														urlmpp = name+restfunction.baseUrl;
													configurationData.rstCntMap[urlmpp] = restfunction;
												}
												else
												{
													if(restfunction.baseUrl=="")
														urlmpp = name+url+rname+"/"+restfunction.name;
													else
														urlmpp = name+restfunction.baseUrl;
													configurationData.rstCntMap[name+url+rname+"/"+restfunction.name] = restfunction;
												}
											}
											else
											{
												if(restfunction.alias!="")
												{
													if(restfunction.baseUrl=="")
														urlmpp = name+url+clas+"/"+restfunction.alias;
													else
														urlmpp = name+restfunction.baseUrl;
													configurationData.rstCntMap[urlmpp] = restfunction;
												}
												else
												{
													if(restfunction.baseUrl=="")
														urlmpp = name+url+clas+"/"+restfunction.name;
													else
														urlmpp = name+restfunction.baseUrl;
													configurationData.rstCntMap[urlmpp] = restfunction;
												}
											}
											cout << "adding rest-controller => " << urlmpp  << " , class => " << clas << endl;
										}
									}
								}
							}
						}
					}
				}
				else if(eles.at(apps).getTagName()=="security")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="login-handler")
						{
							string provider = cntrls.at(cntn).getAttribute("provider");
							string url = cntrls.at(cntn).getAttribute("url");
							string sessionTimeoutV = cntrls.at(cntn).getAttribute("sessionTimeout");
							Security securityObject;
							securityObject.loginProvider = provider;
							securityObject.loginUrl = url;
							try {
								securityObject.sessTimeout = boost::lexical_cast<long>(sessionTimeoutV);
							} catch (...) {
								securityObject.sessTimeout = 3600;
								cout << "\nInvalid session timeout value defined, defaulting to 1hour/3600sec";
							}
							configurationData.securityObjectMap[name] = securityObject;
						}
						else if(cntrls.at(cntn).getTagName()=="secure")
						{
							if(configurationData.securityObjectMap.find(name)!=configurationData.securityObjectMap.end())
							{
								Security securityObject = configurationData.securityObjectMap[name];
								string path = cntrls.at(cntn).getAttribute("path");
								string role = cntrls.at(cntn).getAttribute("role");
								SecureAspect secureAspect;
								secureAspect.path = path;
								secureAspect.role = role;
								securityObject.secures.push_back(secureAspect);
								configurationData.securityObjectMap[name] = securityObject;
							}
						}
						else if(cntrls.at(cntn).getTagName()=="welcome")
						{
							string welcomeFile = cntrls.at(cntn).getAttribute("file");
							if(configurationData.securityObjectMap.find(name)!=configurationData.securityObjectMap.end())
							{
								Security securityObject = configurationData.securityObjectMap[name];
								securityObject.welocmeFile = welcomeFile;
								configurationData.securityObjectMap[name] = securityObject;
							}
						}
					}
				}
				else if(eles.at(apps).getTagName()=="handoffs")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="handoff")
						{
							string app = cntrls.at(cntn).getAttribute("app");
							string def = cntrls.at(cntn).getAttribute("default");
							string ext = cntrls.at(cntn).getAttribute("ext");
							configurationData.handoffs[app] = def;
							configurationData.handoffs[app+"extension"] = ext;
							handoffVec.push_back("-l"+ app+" ");
						}
					}
				}
			}
		}
		Mapping* mapping = new Mapping;
		smstrMap appTableColMapping;
		strMap maptc,maptcl;
		relMap appTableRelMapping;
		Element dbroot = parser.getDocument(defpath+"config/cibernate.xml").getRootElement();
		if(dbroot.getTagName()=="cibernate")
		{
			ElementList dbeles = dbroot.getChildElements();
			for (unsigned int dbs = 0; dbs < dbeles.size(); dbs++)
			{
				if(dbeles.at(dbs).getTagName()=="config")
				{
					ElementList confs = dbeles.at(dbs).getChildElements();
					string uid,pwd,dsn;
					int psize= 2;
					for (unsigned int cns = 0; cns < confs.size(); cns++)
					{
						if(confs.at(cns).getTagName()=="uid")
						{
							uid = confs.at(cns).getText();
						}
						else if(confs.at(cns).getTagName()=="pwd")
						{
							pwd = confs.at(cns).getText();
						}
						else if(confs.at(cns).getTagName()=="dsn")
						{
							dsn = confs.at(cns).getText();
						}
						else if(confs.at(cns).getTagName()=="pool-size")
						{
							if(confs.at(cns).getText()!="")
								psize = boost::lexical_cast<int>(confs.at(cns).getText());
						}
					}
					CibernateConnPools::addPool(psize,uid,pwd,dsn,name);
				}
				else if(dbeles.at(dbs).getTagName()=="tables")
				{
					ElementList tabs = dbeles.at(dbs).getChildElements();
					for (unsigned int dn = 0; dn < tabs.size(); dn++)
					{
						if(tabs.at(dn).getTagName()=="table")
						{
							vector<DBRel> relv;
							/*DBRel relation;
							relation.type = (tabs.at(dn).getAttribute("hasOne")!="")?1:((tabs.at(dn).getAttribute("hasMany")!="")?2:((tabs.at(dn).getAttribute("many")!="")?3:0));
							if(relation.type==1)
								relation.clsName = tabs.at(dn).getAttribute("hasOne");
							else if(relation.type==2)
								relation.clsName = tabs.at(dn).getAttribute("hasMany");
							else if(relation.type==3)
								relation.clsName = tabs.at(dn).getAttribute("many");
							relation.fk = tabs.at(dn).getAttribute("fk");
							relation.pk_rel = tabs.at(dn).getAttribute("pk");*/
							maptcl[tabs.at(dn).getAttribute("class")] = tabs.at(dn).getAttribute("name");
							ElementList cols = tabs.at(dn).getChildElements();
							for (unsigned int cn = 0; cn < cols.size(); cn++)
							{
								if(cols.at(cn).getTagName()=="hasOne")
								{
									DBRel relation;
									relation.clsName = cols.at(cn).getText();
									relation.type = 1;
									relation.fk = cols.at(cn).getAttribute("fk");
									relation.pk = cols.at(cn).getAttribute("pk");
									relv.push_back(relation);
								}
								else if(cols.at(cn).getTagName()=="hasMany")
								{
									DBRel relation;
									relation.clsName = cols.at(cn).getText();
									relation.type = 2;
									relation.fk = cols.at(cn).getAttribute("fk");
									relation.pk = cols.at(cn).getAttribute("pk");
									relation.field = cols.at(cn).getAttribute("field");
									relv.push_back(relation);
								}
								else if(cols.at(cn).getTagName()=="many")
								{
									DBRel relation;
									relation.clsName = cols.at(cn).getText();
									relation.type = 3;
									relation.fk = cols.at(cn).getAttribute("fk");
									relation.pk = cols.at(cn).getAttribute("pk");
									relv.push_back(relation);
								}
								else if(cols.at(cn).getTagName()=="col")
								{
									maptc[cols.at(cn).getAttribute("obf")] = cols.at(cn).getAttribute("dbf");
								}
							}
							appTableColMapping[tabs.at(dn).getAttribute("class")] = maptc;
							appTableRelMapping[tabs.at(dn).getAttribute("class")] = relv;
						}
					}
				}
			}
		}
		mapping->setAppTableColMapping(appTableColMapping);
		mapping->setAppTableClassMapping(maptcl);
		mapping->setAppTableRelMapping(appTableRelMapping);
		CibernateConnPools::addMapping(name,mapping);
		//cout << (defpath+"config/app.prop") << flush;
		propMap afc = pread.getProperties(defpath+"config/afc.prop");

		string filepath;
		if(afc.size()>0)
		{
			string objs = afc["PROP"];
			strVec objv;
			boost::iter_split(objv, objs, boost::first_finder(","));
			for (unsigned int var1 = 0;var1<objv.size();var1++)
			{
				if(objv.at(var1)!="")
				{
					//strVec info = ref.getAfcObjectData(usrincludes+objv.at(var)+".h", true);
					pathvec.push_back(name);
					vecvp.push_back(usrincludes);
					stat.push_back(true);
					afcd.push_back(objv.at(var1));
				}
			}
			objs = afc["INTF"];
			objv.clear();
			boost::iter_split(objv, objs, boost::first_finder(","));
			for (unsigned int var1 = 0;var1<objv.size();var1++)
			{
				if(objv.at(var1)!="")
				{
					//strVec info = ref.getAfcObjectData(usrincludes+objv.at(var)+".h", false);
					pathvec.push_back(name);
					vecvp.push_back(usrincludes);
					stat.push_back(false);
					afcd.push_back(objv.at(var1));
				}
			}
		}
		root = parser.getDocument(defpath+"config/fviews.xml").getRootElement();
		if(root.getTagName()=="fview" && root.getChildElements().size()>0)
		{
			ElementList eles = root.getChildElements();
			for (unsigned int apps = 0; apps < eles.size(); apps++)
			{
				if(eles.at(apps).getTagName()=="page")
				{
					string fvw = eles.at(apps).getAttribute("htm");
					boost::replace_first(fvw,".html",".fview");
					configurationData.fviewmap[eles.at(apps).getAttribute("htm")] = eles.at(apps).getAttribute("class");
					pathvec.push_back(name);
					vecvp.push_back(usrincludes);
					stat.push_back(false);
					afcd.push_back(eles.at(apps).getAttribute("class"));
					ElementList elese = eles.at(apps).getChildElements();
					string nsfns = "\nvar _fview_namespace = {";
					string js = "\n\nwindow.onload = function(){";
					for (unsigned int appse = 0; appse < elese.size(); appse++)
					{
						if(elese.at(appse).getTagName()=="event")
						{
							nsfns += "\n\"_fview_cntxt_global_js_callback"+boost::lexical_cast<string>(appse)+"\" : function(response){" + elese.at(appse).getAttribute("cb") + "},";
							js += "\ndocument.getElementById('"+elese.at(appse).getAttribute("eid")+"').";
							js += elese.at(appse).getAttribute("type") + " = function(){";
							js += eles.at(apps).getAttribute("class")+"."+elese.at(appse).getAttribute("func")+"(";
							string args = elese.at(appse).getAttribute("args");
							if(args!="")
								args += ",";
							js += args + "\"_fview_cntxt_global_js_callback"+boost::lexical_cast<string>(appse)+"\",\"/"+name+"/"+fvw+"\",_fview_namespace);}";
						}
						else if(elese.at(appse).getTagName()=="form")
						{
							pathvec.push_back(name);
							vecvp.push_back(usrincludes);
							stat.push_back(true);
							afcd.push_back(elese.at(appse).getAttribute("bean"));
							configurationData.formMap[elese.at(appse).getAttribute("name")] = elese.at(appse);
						}
					}
					js += "\n}\n\n";
					nsfns = nsfns.substr(0,nsfns.length()-1) + "\n}\n";
					js = nsfns + js;
					for (unsigned int appse = 0; appse < elese.size(); appse++)
					{
						if(elese.at(appse).getTagName()=="functions")
						{
							js += elese.at(appse).getText();
						}
					}
					AfcUtil::writeTofile(pubpath+eles.at(apps).getAttribute("class")+".js",js,true);
				}
			}
		}
	}
	cout << endl<< "started generating compoenent code" <<endl;
	for (unsigned int var1 = 0;var1<compnts.size();var1++)
	{
		string cudata,cuheader,curemote,curemoteheaders;
		string file = gen.generateComponentCU(compnts.at(var1),cudata,cuheader,curemote,curemoteheaders);
		AfcUtil::writeTofile(rtdcfpath+file+".h",cuheader,true);
		AfcUtil::writeTofile(rtdcfpath+file+".cpp",cudata,true);
		AfcUtil::writeTofile(rtdcfpath+file+"_Remote.h",curemoteheaders,true);
		AfcUtil::writeTofile(rtdcfpath+file+"_Remote.cpp",curemote,true);
		isrcs += "./"+file+".cpp \\\n"+"./"+file+"_Remote.cpp \\\n";
		iobjs += "./"+file+".o \\\n"+"./"+file+"_Remote.o \\\n";
		ideps += "./"+file+".d \\\n"+"./"+file+"_Remote.d \\\n";
		configurationData.cmpnames.push_back(file);
		cout << endl<< compnts.at(var1) <<endl;
	}
	for (unsigned int cntn = 0; cntn < handoffVec.size(); cntn++)
	{
		boost::replace_first(libs, handoffVec.at(cntn), "");
	}
	cout << endl<< "done generating compoenent code" <<endl;
	string ret = ref.generateClassDefinitionsAll(all,includeRef);
	AfcUtil::writeTofile(rtdcfpath+"ReflectorInterface.cpp",ret,true);
	ret = ref.generateSerDefinitionAll(all,includeRef);
	AfcUtil::writeTofile(rtdcfpath+"SerializeInterface.cpp",ret,true);
	cntxt["RUNTIME_LIBRARIES"] = libs;
	ret = templ.evaluate(rtdcfpath+"objects.mk.template",cntxt);
	AfcUtil::writeTofile(rtdcfpath+"objects.mk",ret,true);
	cntxt.clear();
	cntxt["USER_DEFINED_INC"] = ilibs;
	cntxt["RUNTIME_COMP_SRCS"] = isrcs;
	cntxt["RUNTIME_COMP_OBJS"] = iobjs;
	cntxt["RUNTIME_COMP_DEPS"] = ideps;
	ret = templ.evaluate(rtdcfpath+"subdir.mk.template",cntxt);
	AfcUtil::writeTofile(rtdcfpath+"subdir.mk",ret,true);
	configurationData.dcpsss = dcps;
	ret = DCPGenerator::generateDCPAll(dcps);
	AfcUtil::writeTofile(rtdcfpath+"DCPInterface.cpp",ret,true);
	string headers,objs,infjs;
	ret = AfcUtil::generateJsObjectsAll(vecvp,afcd,stat,headers,objs,infjs,pathvec);
	AfcUtil::writeTofile(rtdcfpath+"AjaxInterface.cpp",ret,true);
	AfcUtil::writeTofile(pubpath+"_afc_Objects.js",objs,true);
	AfcUtil::writeTofile(pubpath+"_afc_Interfaces.js",infjs,true);
	AfcUtil::writeTofile(incpath+"AfcInclude.h",headers,true);
	ApplicationUtil apputil;
	webdirs.clear();
	ret = apputil.buildAllApplications(appf,webdirs1,configurationData.appMap);
	AfcUtil::writeTofile(rtdcfpath+"ApplicationInterface.cpp",ret,true);
	WsUtil wsu;
	ret = wsu.generateAllWSDL(wspath,respath,configurationData.wsdlmap);
	AfcUtil::writeTofile(rtdcfpath+"WsInterface.cpp",ret,true);
	TemplateEngine engine;
	cntxt.clear();
	cntxt["Dynamic_Public_Folder_Copy"] = rundyncontent;
	string cont = engine.evaluate(respath+"/rundyn_template.sh", cntxt);
	AfcUtil::writeTofile(respath+"/rundyn.sh", cont, true);
	return configurationData;
}


