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
 * HttpResponse.h
 *
 *  Created on: Aug 19, 2009
 *      Author: sumeet
 */

#ifndef HTTPRESPONSE_H_
#define HTTPRESPONSE_H_
#include "string"
#include "vector"
using namespace std;
typedef vector<unsigned char> Cont;
class HttpResponse {
public:
	HttpResponse();
	virtual ~HttpResponse();
    string getContent_str() const
    {
        return content_str;
    }

    void setContent_str(string content_str)
    {
        this->content_str = content_str;
    }

    string getHttpVersion() const
    {
        return httpVersion;
    }

    void setHttpVersion(string httpVersion)
    {
        this->httpVersion = httpVersion;
    }

    string getStatusCode() const
    {
        return statusCode;
    }

    void setStatusCode(string statusCode)
    {
        this->statusCode = statusCode;
    }

    string getStatusMsg() const
    {
        return statusMsg;
    }

    void setStatusMsg(string statusMsg)
    {
        this->statusMsg = statusMsg;
    }

    string getDate() const
    {
        return date;
    }

    void setDate(string date)
    {
        this->date = date;
    }

    string getConnection() const
    {
        return connection;
    }

    void setConnection(string connection)
    {
        this->connection = connection;
    }

    string getServer() const
    {
        return server;
    }

    void setServer(string server)
    {
        this->server = server;
    }

    string getAccept_ranges() const
    {
        return accept_ranges;
    }

    void setAccept_ranges(string accept_ranges)
    {
        this->accept_ranges = accept_ranges;
    }

    string getContent_type() const
    {
        return content_type;
    }

    void setContent_type(string content_type)
    {
        this->content_type = content_type;
    }

    string getContent_len() const
    {
        return content_len;
    }

    void setContent_len(string content_len)
    {
        this->content_len = content_len;
    }

    Cont getContent() const
	{
		return content;
	}

	void setContent(Cont content)
	{
		this->content = content;
	}

	void setContent(string content)
	{
		this->content.clear();
		for(unsigned int i=0;i<content.length();i++)
			this->content.push_back(content[i]);
	}

    string getLast_modified() const
    {
        return last_modified;
    }

    void setLast_modified(string last_modified)
    {
        this->last_modified = last_modified;
    }

private:
	string httpVersion;
	string statusCode;
	string statusMsg;
	string date;
	string connection;
	string server;
	string accept_ranges;
	Cont content;
	string content_type;
	string content_len;
	string last_modified;
	string content_str;
};

#endif /* HTTPRESPONSE_H_ */
