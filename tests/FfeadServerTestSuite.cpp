/*
	Copyright 2009-2020, Sumeet Chhetri

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
 * FfeadServerTestSuite.cpp
 *
 *  Created on: 29-Jan-2013
 *      Author: sumeetc
 */

#include "AppDefines.h"
#include "Compatibility.h"
#include "HttpResponseParser.h"
#include "SimpleCsvFileReader.h"
#include "PropFileReader.h"
#include "Timer.h"
#include "Thread.h"
#include "sstream"
#include "Client.h"
#include <chrono>
#ifdef HAVE_SSLINC
#include "SSLClient.h"
#endif
int main()
{
	#ifdef OS_MINGW
		// startup WinSock in Windows
		WSADATA wsa_data;
		WSAStartup(MAKEWORD(1,1), &wsa_data);
	#endif
	
	HTTPResponseStatus::init();

	PropFileReader propFileReader;
	propMap props = propFileReader.getProperties("testValues.prop");

	strVecVec testCases = SimpleCsvReader::getRows("test.csv");

	Timer timer, timerc;
	std::string cookies, result;
	int total, skipped = 0, passed = 0, failed = 0, header = 0, counter = 0;

	total = (int)testCases.size();

	std::string ip = props["SERVER_IP_ADDRESS"];
	if(ip=="")
		ip = "localhost";
	int port = 8080;
	if(props["SERVER_PORT"]!="")
	{
		try{
			port = CastUtil::lexical_cast<int>(props["SERVER_PORT"]);
		} catch(const std::exception& e) {
		}

	}
	bool sslEnabled = false;
	if(props["SERVER_SSL_ENABLED"]!="")
	{
		try{
			sslEnabled = CastUtil::lexical_cast<bool>(props["SERVER_SSL_ENABLED"]);
		} catch(const std::exception& e) {
		}
	}

	std::string sslFile = props["SERVER_SSL_FILE"];

	bool isDebug = false;
	if(StringUtil::toLowerCopy(props["DEBUG"])=="true") {
		isDebug = true;
	}

	std::cout << "Server IP - " << ip <<std::endl;
	std::cout << "Server Port - " << port <<std::endl;
	std::cout << "Debug Mode - " << isDebug <<std::endl;
	std::cout << "Server SSL Enabled - " << CastUtil::lexical_cast<std::string>(sslEnabled) <<std::endl;
	if(sslEnabled)
	{
		std::cout << "Server SSL File - " << sslFile <<std::endl;
	}

	timerc.start();
	for (int var = 0; var < total; var++)
	{
		ClientInterface* client;
#ifdef HAVE_SSLINC
		if(sslEnabled)
		{
			if(sslFile!="")
				client = new SSLClient(sslFile);
			else
				client = new SSLClient;
		}
		else
#endif
			client = new Client;
		if(testCases[var].size()>=4)
		{
			if(testCases[var][0]=="ENABLED")
			{
				header++;
				continue;
			}
			counter = var;
			std::string request = testCases[var][2];
			if(testCases[var][0]=="N" || testCases[var][0]=="n")
			{
				std::cout << "Request " << counter << " " << request << " was Skipped" << std::endl;
				skipped++;
				continue;
			}

			bool debugCont = false;
			std::string debugContStr = testCases[var][1];
			if(debugContStr=="Y" || debugContStr=="y")
				debugCont = true;

			std::string responseCode = testCases[var][3];
			std::string file;
			if(testCases[var].size()>4)
				file = testCases[var][4];
			std::string fileCntlen;
			if(testCases[var].size()>5)
				fileCntlen = testCases[var][5];
			std::string reqContTyp, content, headers, respCntType;
			if(testCases[var].size()>6)
			{
				reqContTyp = testCases[var][6];
			}
			if(testCases[var].size()>7)
			{
				content = testCases[var][7];
			}
			if(testCases[var].size()>8)
			{
				headers = testCases[var][8];
				if(headers!="" && headers.find("HEADERVALS_")!=std::string::npos)
				{
					headers = props[headers];
				}
				else
				{
					std::vector<std::string> headerVec;
					StringUtil::split(headerVec, headers, ";");
					headers = "";
					for (int var = 0; var < (int)headerVec.size(); ++var) {
						std::vector<std::string> param;
						StringUtil::split(param, headerVec.at(var), "=");
						if(param.size()==2)
						{
							headers += param.at(0) + ": " + param.at(1) + "\r\n";
						}
					}
				}
			}
			if(testCases[var].size()>9)
			{
				respCntType = testCases[var][9];
			}

			std::string data = request;
			data += " HTTP/1.1\r\nHost: "+ip+":"+CastUtil::lexical_cast<std::string>(port)+"\r\nUser-Agent: Program\r\n";

			if(content!="" && content.find("TSTVALUES_")!=std::string::npos)
				content = props[content];

			if(reqContTyp!="")
			{
				data += "content-type: " + reqContTyp + "\r\n";
			}
			if(content.length()>0)
			{
				data += "content-length: " + CastUtil::lexical_cast<std::string>((int)content.length()) + "\r\n";
			}
			if(cookies!="")
			{
				data += "cookie: " + cookies + "\r\n";
			}
			if(headers!="")
			{
				data += headers;
			}
			data += "\r\n";

			if(content!="")
			{
				data += content;
			}

			timer.start();

			if(isDebug) {
				std::cout << "\n[[\nHTTP Request Is=>\n" << data << "\n" << std::endl;
			}

			if(!client->connectionNB(ip,port)) {
				std::cout << "Unable to connect to server at " << ip << ":" << port << std::endl;
				delete client;
				return 0;
			}
			client->sendData(data);
			int c = 0;
			while(!client->isReady(0)) {
				//std::this_thread::sleep_for (std::chrono::milliseconds(100));
				Thread::mSleep(100);
				c += 100;
				if(c>5000) {
					std::cout << "Timedout waiting for response after 5 seconds" << std::endl;
					exit(0);
				}
			}
			std::string tot = client->getTextData("\r\n\r\n","content-length");
			long long millis = timer.elapsedMilliSeconds();

			if(isDebug) {
				std::cout << "HTTP Response Is=>\n" << tot << std::endl;
			}

			HttpResponse res;
			HttpResponseParser parser(tot, res);

			if(res.getHeader("set-cookie")!="")
			{
				cookies = res.getHeader("set-cookie");
				cookies = cookies.substr(0, cookies.find(";"));
			}

			std::string debugContentValue;
			if(debugCont)
			{
				if(isDebug) {
					std::cout << parser.getContent() << "\n]]\n" << std::endl;
				} else {
					debugContentValue = ", Content => " + parser.getContent();
				}
			}

			std::string ss;
			bool passedFlag = false, done = false;
			if(res.getStatusCode()==responseCode)
			{
				if(respCntType!="")
				{
					if(res.getHeader("content-type").find(respCntType)==0)
					{
						ss.clear();
						ss = "Test " + CastUtil::lexical_cast<std::string>(counter) + " " + request + " was Successfull, Response Time = " + CastUtil::lexical_cast<std::string>(millis) + "ms" + debugContentValue;
						passedFlag = true;
					}
					else
					{
						ss.clear();
						ss = "Test " + CastUtil::lexical_cast<std::string>(counter) + " " + request + " Failed, Response Time = " + CastUtil::lexical_cast<std::string>(millis) + "ms"
								+ ", Expected ContentType = " + respCntType + ", Actual ContentType = "  + res.getHeader("content-type");
						passedFlag = false;
					}
					done = true;
				}
				if(!done)
				{
					std::string cntlen = res.getHeader("content-length");
					if(file!="")
					{
						std::ifstream myfile (&file[0], std::ios::binary | std::ios::ate);
						if (myfile.is_open() && cntlen!="" && myfile.tellg()==CastUtil::lexical_cast<int>(cntlen))
						{
							ss.clear();
							ss = "Test " + CastUtil::lexical_cast<std::string>(counter) + " " + request + " was Successfull, Response Time = " + CastUtil::lexical_cast<std::string>(millis) + "ms" + debugContentValue;
							passedFlag = true;
						}
						else
						{
							ss.clear();
							ss = "Test " + CastUtil::lexical_cast<std::string>(counter) + " " + request + ", Invalid Content Length, Response Time = " + CastUtil::lexical_cast<std::string>(millis) + "ms" + debugContentValue;
							passedFlag = false;
						}
					}
					else if((file=="" && fileCntlen=="") || (fileCntlen!="" && fileCntlen==cntlen))
					{
						ss.clear();
						ss = "Test " + CastUtil::lexical_cast<std::string>(counter) + " " + request + " was Successfull, Response Time = " + CastUtil::lexical_cast<std::string>(millis) + "ms" + debugContentValue;
						passedFlag = true;
					}
					else
					{
						ss.clear();
						ss = "Test " + CastUtil::lexical_cast<std::string>(counter) + " " + request + ", Invalid Content Length, Response Time = " + CastUtil::lexical_cast<std::string>(millis) + "ms" + debugContentValue;
						passedFlag = false;
					}
				}
			}
			else
			{
				ss.clear();
				ss = "Test " + CastUtil::lexical_cast<std::string>(counter) + " " + request + " Failed, Response Time = " + CastUtil::lexical_cast<std::string>(millis) + "ms" + ", Expected Status = " +
						responseCode + ", Actual Status = "  + res.getStatusCode();
				passedFlag = false;
			}
			std::cout << ss << std::endl;
			if(passedFlag)
				passed++;
			else
				failed++;
		}
		else
		{
			skipped++;
		}
		if(client!=NULL)
		{
			client->closeConnection();
			delete client;
		}
	}

	std::cout << "Total Tests = " << total-1 << ", Passed = " << passed << ", Failed = " << failed
			<< ", Skipped = " << skipped << ", Time taken = " << timerc.elapsedMilliSeconds() << "ms" << std::endl;
	
	#ifdef OS_MINGW
		WSACleanup();
	#endif

	return 0;
}
