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
 * FfeadServerTestSuite.cpp
 *
 *  Created on: 29-Jan-2013
 *      Author: sumeetc
 */


#include "HttpResponseParser.h"
#include "CsvFileReader.h"
#include "PropFileReader.h"
#include "Timer.h"
#include "sstream"
#include "Client.h"
#include "SSLClient.h"
int main()
{
	PropFileReader propFileReader;
	propMap props = propFileReader.getProperties("testValues.prop");

	CsvFileReader csvFileReader;
	strVecVec testCases = csvFileReader.getRows("test.csv");

	Timer timer, timerc;
	string cookies, result;
	int total, skipped = 0, passed = 0, failed = 0, header = 0, counter = 0;

	total = (int)testCases.size();

	string ip = props["SERVER_IP_ADDRESS"];
	if(ip=="")
		ip = "localhost";
	int port = 8080;
	if(props["SERVER_PORT"]!="")
	{
		try{
			port = CastUtil::lexical_cast<int>(props["SERVER_PORT"]);
		} catch(...) {
		}

	}
	bool sslEnabled = false;
	if(props["SERVER_SSL_ENABLED"]!="")
	{
		try{
			sslEnabled = CastUtil::lexical_cast<bool>(props["SERVER_SSL_ENABLED"]);
		} catch(...) {
		}
	}

	string sslFile = props["SERVER_SSL_FILE"];

	cout << "Server IP - " << ip <<endl;
	cout << "Server Port - " << port <<endl;
	cout << "Server SSL Enabled - " << CastUtil::lexical_cast<string>(sslEnabled) <<endl;
	if(sslEnabled)
	{
		cout << "Server SSL File - " << sslFile <<endl;
	}

	timerc.start();
	for (int var = 0; var < total; ++var)
	{
		ClientInterface* client;
		if(sslEnabled)
		{
			if(sslFile!="")
				client = new SSLClient(sslFile);
			else
				client = new SSLClient;
		}
		else
			client = new Client;
		if(testCases[var].size()>=4)
		{
			if(testCases[var][0]=="ENABLED")
			{
				header++;
				continue;
			}
			counter = var;
			string request = testCases[var][2];
			if(testCases[var][0]=="N" || testCases[var][0]=="n")
			{
				cout << "Request " << counter << " " << request << " was Skipped" << endl;
				skipped++;
				continue;
			}

			bool debugCont = false;
			string debugContStr = testCases[var][1];
			if(debugContStr=="Y" || debugContStr=="y")
				debugCont = true;

			string responseCode = testCases[var][3];
			string file;
			if(testCases[var].size()>4)
				file = testCases[var][4];
			string fileCntlen;
			if(testCases[var].size()>5)
				fileCntlen = testCases[var][5];
			string reqContTyp, content, headers, respCntType;
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
				if(headers!="" && headers.find("HEADERVALS_")!=string::npos)
				{
					headers = props[headers];
				}
				else
				{
					vector<string> headerVec;
					StringUtil::split(headerVec, headers, ";");
					headers = "";
					for (int var = 0; var < (int)headerVec.size(); ++var) {
						vector<string> param;
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

			string data = request;
			data += " HTTP/1.1\r\nHost: "+ip+":"+CastUtil::lexical_cast<string>(port)+"\r\nUser-Agent: Program\r\n";

			if(content!="" && content.find("TSTVALUES_")!=string::npos)
				content = props[content];

			if(reqContTyp!="")
			{
				data += "Content-Type: " + reqContTyp + "\r\n";
			}
			if(content.length()>0)
			{
				data += "Content-Length: " + CastUtil::lexical_cast<string>((int)content.length()) + "\r\n";
			}
			if(cookies!="")
			{
				data += "Cookie: " + cookies + "\r\n";
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

			client->connectionUnresolv(ip,port);
			int bytes = client->sendData(data);
			string tot = client->getTextData("\r\n","Content-Length");
			long long millis = timer.elapsedMilliSeconds();

			HttpResponse res;
			HttpResponseParser parser(tot, res);

			if(res.getHeader("Set-Cookie")!="")
			{
				cookies = res.getHeader("Set-Cookie");
				cookies = cookies.substr(0, cookies.find(";"));
			}

			string debugContentValue;
			if(debugCont)
			{
				debugContentValue = ", Content => " + parser.getContent();
			}

			string ss;
			bool passedFlag = false, done = false;
			if(res.getStatusCode()==responseCode)
			{
				if(respCntType!="")
				{
					if(res.getHeader("Content-Type")==respCntType)
					{
						ss.clear();
						ss = "Test " + CastUtil::lexical_cast<string>(counter) + " " + request + " was Successfull, Response Time = " + CastUtil::lexical_cast<string>(millis) + "ms" + debugContentValue;
						passedFlag = true;
					}
					else
					{
						ss.clear();
						ss = "Test " + CastUtil::lexical_cast<string>(counter) + " " + request + " Failed, Response Time = " + CastUtil::lexical_cast<string>(millis) + "ms"
								+ ", Expected ContentType = " + respCntType + ", Actual ContentType = "  + res.getHeader("Content-Type");
						passedFlag = false;
					}
					done = true;
				}
				if(!done)
				{
					string cntlen = res.getHeader("Content-Length");
					if(file!="")
					{
						ifstream myfile (&file[0], ios::binary | ios::ate);
						if (myfile.is_open() && cntlen!="" && myfile.tellg()==CastUtil::lexical_cast<int>(cntlen))
						{
							ss.clear();
							ss = "Test " + CastUtil::lexical_cast<string>(counter) + " " + request + " was Successfull, Response Time = " + CastUtil::lexical_cast<string>(millis) + "ms" + debugContentValue;
							passedFlag = true;
						}
						else
						{
							ss.clear();
							ss = "Test " + CastUtil::lexical_cast<string>(counter) + " " + request + ", Invalid Content Length, Response Time = " + CastUtil::lexical_cast<string>(millis) + "ms" + debugContentValue;
							passedFlag = false;
						}
					}
					else if((file=="" && fileCntlen=="") || (fileCntlen!="" && fileCntlen==cntlen))
					{
						ss.clear();
						ss = "Test " + CastUtil::lexical_cast<string>(counter) + " " + request + " was Successfull, Response Time = " + CastUtil::lexical_cast<string>(millis) + "ms" + debugContentValue;
						passedFlag = true;
					}
					else
					{
						ss.clear();
						ss = "Test " + CastUtil::lexical_cast<string>(counter) + " " + request + ", Invalid Content Length, Response Time = " + CastUtil::lexical_cast<string>(millis) + "ms" + debugContentValue;
						passedFlag = false;
					}
				}
			}
			else
			{
				ss.clear();
				ss = "Test " + CastUtil::lexical_cast<string>(counter) + " " + request + " Failed, Response Time = " + CastUtil::lexical_cast<string>(millis) + "ms" + ", Expected Status = " +
						responseCode + ", Actual Status = "  + res.getStatusCode();
				passedFlag = false;
			}
			cout << ss << endl;
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

	cout << "Total Tests = " << total-1 << ", Passed = " << passed << ", Failed = " << failed
			<< ", Skipped = " << skipped << ", Time taken = " << timerc.elapsedMilliSeconds() << "ms" << endl;

	return 0;
}
